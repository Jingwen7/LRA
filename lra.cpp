#include "htslib/hts.h"
#include "htslib/kseq.h"
#include <stdio.h>
#include <stdlib.h>


#include <thread>
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <zlib.h>
#include <vector>
#include "htslib/kseq.h"
#include "htslib/sam.h"
#include "Input.h"
#include "MMIndex.h"
#include "TupleOps.h"
#include "MinCount.h"
#include "MapRead.h"
using namespace std;
#include <algorithm>
#include "SeqUtils.h"
#include "Options.h"
#include "Alignment.h"
#include "LogLookUpTable.h"

#include <math.h>



const char* version="0.1-alpha";

bool ArgIs(const char* a, const char* b) {
	return strcmp(a,b) == 0;
}
const char* GetArgv(const char* argv[], int argc, int argi) {
	if (argi +1 >= argc) {
		cout << "ERROR, argument " << argv[argi] << " requires a value." << endl;
		exit(1);
	}
	return argv[argi+1];
}


void HelpMap() {
	cout << "Usage: lra align genome.fa reads [reads2 ...] [options]" << endl << endl;
	cout << "   The genome should be indexed using the 'lra index' program." << endl
			 << "   'reads' may be either fasta, sam, or bam, and multiple input files may be given." << endl << endl;
	cout << "Options:" << endl
			 << "   -p  [FMT]   Print alignment format FMT='b' bed, 's' sam 'p' pair ." << endl
			 << "   -H          Use hard-clipping for SAM output format" << endl
			 << "   -M  M(int)  Do not refine clusters with fewer than M global matches (20)." << endl
			 << "   -m  m(int)  Do not align clusters with fewer than m refined"<< endl
			 << "               matches (40). Typically m > 3*M" << endl
			 << "   -a  (flag)  Query all positions in a read, not just minimizers. " << endl
			 << "               This is 10-20% slower, with an increase in specificity. " << endl
			 << "   -b  (flag)  Skip banded alignment. This is about a 15% speedup." << endl
			 << "   -R  (flag)  MeRge clusters before sparse dynamic programming." << endl
			 << "   -N  (flag)  Use Naive dynamic programming to find the global chain." << endl
			 << "	-S 	(flag)  Use Sparse dynamic programming to find the global chain." << endl
			 << "	-T 	(flag)  Use log LookUpTable when gap length is larger than 501." << endl
		         << "   -t n(int)   Use n threads (1)" << endl
			 << "   --start  (int)   Start aligning at this read." << endl
			 << "   --stride (int)   Read stride (for multi-job alignment of the same file)." << endl;
}
		
class MapInfo {
public:
	std::vector<float> *LookUpTable;
	Header*header;
	Genome *genome;
	vector<GenomeTuple> *genomemm;
	LocalIndex *glIndex;
	Input *reader;
	Options *opts;
	ostream *out;
	int thread;
	int numThreads;
	pthread_mutex_t *semaphore;
};

void MapReads(MapInfo *mapInfo) {
	Read read;
	while (mapInfo->reader->GetNext(read)) {
		if (mapInfo->opts->readStride != 1 and
				mapInfo->reader->nReads % mapInfo->opts->readStride != mapInfo->opts->readStart ) {
			continue;
		}
		else {
			MapRead(*mapInfo->LookUpTable, read, *mapInfo->genome, *mapInfo->genomemm, *mapInfo->glIndex, *mapInfo->opts, mapInfo->out, mapInfo->semaphore);
		}
	}
	pthread_exit(NULL);
}



void RunAlign(int argc, const char* argv[], Options &opts ) {
// open query file for reading; you may use your favorite FASTA/Q parser
	int argi = 0;
	string genomeFile = "", reads = "";
	string indexFile="";
	int w=10;
	vector<string> allreads;
	for (argi = 0; argi < argc; ) {
		if (ArgIs(argv[argi], "-a")) {
			++argi;
			opts.storeAll=true;
		}		
		else if (ArgIs(argv[argi], "-W")) {
			opts.globalW=atoi(GetArgv(argv, argc, argi));
			++argi;
		}		
		else if (ArgIs(argv[argi], "-M")) {
			opts.minClusterSize=atoi(GetArgv(argv, argc, argi));
			++argi;
		}		
		else if (ArgIs(argv[argi], "-m")) {
			opts.minRefinedClusterSize=atoi(GetArgv(argv, argc, argi));
			++argi;
		}		
		else if (ArgIs(argv[argi], "-f")) {
			opts.globalMaxFreq=atoi(GetArgv(argv, argc, argi));
			++argi;
		}		
		else if (ArgIs(argv[argi], "-K")) {
			opts.globalK=atoi(GetArgv(argv, argc, argi));
			++argi;
		}		
		else if (ArgIs(argv[argi], "-H")) {
			opts.hardClip=true;
		}
		else if (ArgIs(argv[argi], "-p")) {			
			opts.printFormat = GetArgv(argv, argc, argi)[0];
			++argi;
		}
		else if (ArgIs(argv[argi], "--maxDiag")) {
			opts.maxDiag = atoi(GetArgv(argv, argc, argi));
		}
		else if (ArgIs(argv[argi], "-n")) {
			opts.bestn=atoi(GetArgv(argv, argc, argi));
		}
		else if (ArgIs(argv[argi], "-t")) {
			opts.nproc=atoi(GetArgv(argv, argc, argi));
			++argi;
		}		
		else if (ArgIs(argv[argi], "-r")) {
			opts.refineLevel=atoi(GetArgv(argv, argc, argi));
			++argi;
		}
		else if (ArgIs(argv[argi], "--stride")) {
			opts.readStride=atoi(GetArgv(argv, argc, argi));
			++argi;
		}
		else if (ArgIs(argv[argi], "--seqan")) {
			opts.seqan=true;
			++argi;
		}
		else if (ArgIs(argv[argi], "--start")) {
			opts.readStart=atoi(GetArgv(argv, argc, argi));
			++argi;
		}

		else if (ArgIs(argv[argi], "-R")) {
			opts.mergeClusters=true;
		}
		else if (ArgIs(argv[argi], "-N")) {
			opts.NaiveDP = true;
		}
		else if (ArgIs(argv[argi], "-S")) {
			opts.SparseDP = true;
		}
		else if (ArgIs(argv[argi], "-T")) {
			opts.LookUpTable = true;
		}
		else if (ArgIs(argv[argi], "-o")) {
			opts.outfile = argv[++argi];
		}
		else if (ArgIs(argv[argi], "-d")) {
			opts.dotPlot = true;
		}

		else if (ArgIs(argv[argi], "--locMatch")) {
			opts.localMatch=atoi(GetArgv(argv,argc,argi));
			++argi;
		}
		else if (ArgIs(argv[argi], "--locBand")) {
			opts.localBand=atoi(GetArgv(argv,argc,argi));
			++argi;
		}
		else if (ArgIs(argv[argi], "--locIndel")) {
			opts.localIndel=atoi(GetArgv(argv,argc,argi));
			++argi;
		}
		else {
			if (genomeFile == "") {
				genomeFile = argv[argi];
			}
			else {
				allreads.push_back(string(argv[argi]));
			}
		}
		++argi;
	}

	if (genomeFile == "" || allreads.size() == 0) {
		HelpMap();
		exit(1);
	}
	if (indexFile == "") {
		indexFile = genomeFile + ".mmi";
	}
	Header header;
	vector<GenomeTuple> genomemm;
	LocalIndex glIndex;


	if (ReadIndex(indexFile, genomemm, header, opts) == 0) {
		StoreIndex(genomeFile, genomemm, header, opts);
	}
	
	if (glIndex.Read(genomeFile+".gli") == 0) {
		glIndex.IndexFile(genomeFile);
	}
	GenomePos mm=0;
	for(GenomePos mi =0; mi < genomemm.size(); mi++) {
		if (genomemm[mi].pos > mm) {
			mm = genomemm[mi].pos;
		}
	}
	Genome genome;
	genome.Read(genomeFile);

	Input reader;
	reader.Initialize(allreads);
	int offset=0;
	Read read;
	ostream *outPtr;
	ofstream outfile;
	if (opts.outfile == "" or opts.outfile=="-") {
		outPtr = &cout;
	}
	else {
		outfile.open(opts.outfile.c_str());
		outPtr = &outfile;
	}

	if (opts.printFormat == 's') {
		stringstream cl;
		cl << "lra align";
		for (int i=0; i < argc; i++) {
			cl << " " << argv[i];
		}
		*outPtr << "@PG\tID:lra\tPN:lra\tVN:"<<version<<"\tCL:"<<cl.str() << endl;
		genome.header.WriteSAMHeader(*outPtr);
	}

	vector<float> LookUpTable;
	CreateLookUpTable(LookUpTable);

	if (opts.nproc > 1) {
		pthread_attr_t *threadAttr = new pthread_attr_t[opts.nproc];
		for (int procIndex = 0; procIndex < opts.nproc; procIndex++ ){
			pthread_attr_init(&threadAttr[procIndex]);
		}

		pthread_t *threads = new pthread_t[opts.nproc];
		vector<MapInfo> mapInfo(opts.nproc);
		
		pthread_mutex_t semaphore;		
		pthread_mutex_init(&semaphore, NULL);
		for (int procIndex = 0; procIndex < opts.nproc; procIndex++ ){ 
			mapInfo[procIndex].LookUpTable = &LookUpTable;
			mapInfo[procIndex].genome = &genome;
			mapInfo[procIndex].genomemm = &genomemm;
			mapInfo[procIndex].glIndex = &glIndex;
			mapInfo[procIndex].reader = &reader;
			mapInfo[procIndex].opts= &opts;
			mapInfo[procIndex].out = outPtr;
			mapInfo[procIndex].thread=procIndex;
			mapInfo[procIndex].semaphore=&semaphore;
			pthread_create(&threads[procIndex], &threadAttr[procIndex], (void* (*)(void*))MapReads, &mapInfo[procIndex]);
		}

		for (int procIndex = 0; procIndex < opts.nproc; procIndex++) {
			pthread_join(threads[procIndex], NULL);
		}
	
	
	}
	else {
		while (reader.GetNext(read)) {
			MapRead(LookUpTable, read, genome, genomemm, glIndex, opts, outPtr);
		}
	}
}
void HelpStoreIndex() {
	cout << "Usage: lra index file.fa [options]" << endl
			 << "  Global index options " << endl
			 << "   -W (int) Minimizer window size (10)." << endl
			 << "   -F (int) Maximum minimizer frequency (200)." << endl
			 << "   -K (int) Word size" << endl
			 << "  Local index options: "<< endl
			 << "   -w (int) Local minimizer window size (10)." << endl
			 << "   -f (int) Local maximum minimizer frequency (5)." << endl
			 << "   -k (int) Local word size (10)" << endl
			 << "   -h Print help." << endl;
}

void HelpStoreGlobal() {
	cout << "Usage: lra index file.fa [options]" << endl
			 << "   -W (int) Minimizer window size (10)." << endl
			 << "   -F (int) Maximum minimizer frequency (200)." << endl
			 << "   -K (int) Word size" << endl
			 << "   -h Print help." << endl;
	
}
void HelpStoreLocal() {
	cout << "Usage: lra local file.fa [options]" << endl
			 << "   -w (int) Local minimizer window size (10)." << endl
			 << "   -f (int) Local maximum minimizer frequency (5)." << endl
			 << "   -k (int) Local word size (10)" << endl
			 << "   -h Print help." << endl;
}

void RunStoreLocal(int argc, const char* argv[], 
									 LocalIndex &glIndex, Options &opts) {
	int argi = 0;
	string genome;
	string indexFile="";
	bool printIndex = false;
	opts.localK=glIndex.k;
	for (argi = 0; argi < argc; ) {
		if (ArgIs(argv[argi], "-h")) {
			HelpStoreLocal();
			exit(1);
		}
		else if (ArgIs(argv[argi], "-k")) {
			opts.localK=atoi(argv[++argi]);
			glIndex.k=opts.localK;
		}
		else if (ArgIs(argv[argi], "-w")) {
			opts.localW=atoi(argv[++argi]);
			glIndex.w=opts.localW;
		}
		else if (ArgIs(argv[argi], "-f")) {
			opts.localMaxFreq=atoi(argv[++argi]);
			glIndex.maxFreq=opts.localMaxFreq;
		}
		else if (ArgIs(argv[argi], "-K") or ArgIs(argv[argi], "-W") or ArgIs(argv[argi], "-F")) {
			argi+=2;
			continue;
		}
		else if (strlen(argv[argi]) > 0 and argv[argi][0] == '-') {
			HelpStoreLocal();
			cout << "Invalid option " << argv[argi] << endl;
			exit(1);
		}
		else {
			genome = argv[argi];
			cerr << "genome " << genome << endl;
		}
		++argi;
	}
	if (genome == "") {
		HelpStoreGlobal();
		exit(1);
	}

	glIndex.IndexFile(genome);
	glIndex.Write(genome + ".gli");
}

void RunStoreGlobal(int argc, const char* argv[], 
										vector<GenomeTuple> &minimizers, Header &header, Options &opts) {
	// open query file for reading; you may use your favorite FASTA/Q parser
	int argi = 0;
	string genome;
	string indexFile="";
	bool printIndex = false;
	bool compress=false;
	opts.globalW=10;
	for (argi = 0; argi < argc; ) {
		if (ArgIs(argv[argi], "-W")) {
			++argi;
			opts.globalW = atoi(argv[argi]);
		}
		else if (ArgIs(argv[argi], "-F")) {
			++argi;
			opts.globalMaxFreq = atoi(argv[argi]);
		}		
		else if (ArgIs(argv[argi], "-i")) {
			++argi;
			indexFile=argv[argi];
		}
		else if (ArgIs(argv[argi], "-c")) {
			cerr << "WARNING: Compressing index" << endl;
			compress = true;
		}
		else if (ArgIs(argv[argi], "-p")) {
			++argi;
			printIndex = true;
		}
		else if (ArgIs(argv[argi], "-K")) {
			++argi;
			opts.globalK=atoi(argv[argi]);
		}		
		else if (ArgIs(argv[argi], "-k") or ArgIs(argv[argi], "-w") or ArgIs(argv[argi], "-f")) {
			argi+=2;
			continue;
		}
		else if (ArgIs(argv[argi], "-h")) {
			++argi;
			HelpStoreGlobal();
			exit(0);
		}		

		else if (strlen(argv[argi]) > 0 && argv[argi][0] == '-') {
			HelpStoreGlobal();
			cout << "Invalid option " << argv[argi] << endl;
			exit(1);
		}
		else {
			genome = argv[argi];
			cerr << "genome " << genome << endl;
		}
		++argi;
	}
	if (genome == "") {
		HelpStoreGlobal();
		exit(1);
	}
	if (indexFile == "") {
		indexFile = genome + ".mmi";
	}

	if (printIndex and ReadIndex(indexFile, minimizers, header, opts)) {
		PrintIndex(minimizers, opts.globalK);
		exit(0);
	}

	StoreIndex(genome, minimizers, header, opts);
	WriteIndex(indexFile, minimizers, header, opts);
}

void RunStoreIndex(int argc, const char* argv[]) {
	LocalIndex glIndex;
	vector<GenomeTuple> minimizers;
	Header header;
	Options opts;

	RunStoreGlobal(argc, argv, minimizers, header, opts);
  RunStoreLocal(argc, argv, glIndex, opts);
}



void Usage() {
	cout << "Program: lra (long sequence alignment)" << endl;
	cout << "Version: " << version << endl;
	cout << "Contact: Mark Chaisson (mchaisso@usc.edu)" << endl << endl;
	cout << "Usage:   lra <command> [options]"<< endl << endl;
	cout << "Command: index   - Build global and local indexes on a genome." << endl;
	cout << "         align   - Map reads using the index." << endl;
	cout << "         global  - Build a global index." << endl;
	cout << "         local   - Build local index." << endl;
}

int main(int argc, const char *argv[]) {
	if (argc < 2) {
		Usage();
		return 1;
	}

	Options opts;

  int argi;
	vector<GenomeTuple>  minimizers;
	LocalIndex lIndex;
	Header header;
	for (argi = 1; argi < argc; ){
		if (ArgIs(argv[argi], "index")) {
			argc -=2;
      RunStoreIndex(argc,  &argv[2]);
			exit(0);
		}
		else if (ArgIs(argv[argi], "global")) {
			argc -=2;
      RunStoreGlobal(argc,  &argv[2], minimizers, header, opts);		
			exit(0);
		}
		else if (ArgIs(argv[argi], "local")) {
			argc -=2;
			RunStoreLocal(argc, &argv[2], lIndex, opts);
			exit(0);
		}
		else if (ArgIs(argv[argi], "align")) {
			argc -=2;
			RunAlign(argc, (const char**) &argv[2], opts);
			exit(0);
		}

		else {
			Usage();
			exit(1);
		}
	}

}
