#include "AffineOneGapAlign.h"
#include <iostream>
#include <string>

using namespace std;

void Test(string query, string target) {
	Alignment aln;

	AffineOneGapAlign(query, target, 4, -4, -3, 15, aln);
	aln.genomeLen = target.size();
	aln.read=(char*) query.c_str();
	aln.genome=(char*) target.c_str();
	aln.CalculateStatistics();
	aln.PrintPairwise(cout);
	
	aln.Clear();
}
int main(int argc, char* argv[]) {
	Test("CATC",
			 "CAT");
	exit(0);
	Test("AAA", "A");
	Test("AGGTATCAGTCCCCTTTGACCTAGGTCTGGCCAGCGGATGTGGTCCTATGGGAAGATCTTAACTCAACTCGCTAACGTCACGTTGCTCCTTTGTCTTTTCCACTGTCACGGAGCTTTGC",
			 "GTATCAGTCCCCTTGACAGGTCTGCAGGGATGGCCCTTATGGAGATCTAACCAACTCGCTACGTCAGTTTCTCCTTTGTCTTTCCATGTAGGCTTGC");
		Test("ATGACTAAAAGCTTCTCAAATACCGGACGATCACTGTGCGAAATTGACATATTCTGAGAACGCAAAGTAAAACACTGATT",
				 "ACTAAAGCTTCCAAAAAGGAGAACTGGAAAAATGACAATTTAGAGGTGGGAAAGAAACATGAGT");

		Test("CTGTCTCACTGTGTCTGTCTTCTGTCTTACTCTCTTTCTCTCCCCGTCTGTCTCTCTCTCTCTCTCTCCCTCCCTGTTTGTTTCTCTCTCTCCCTCCCTGTCTGTTTCTCTCTCTCTCTTTCTGTCTGTTTCTGTCTCTCTCTGTCTGTCTATGTCTTTCTCTGTCTGTCTCTTTCTCTGTCTGTCTGCCTCTCTCTTTCTTTTTCTGTGTCTCTCTGTCGGTCTCTCTCTCTCTGTCTGTCTGTCTGTCTCTCTCTCTCTCTCTCTGTGCCTATCTTCTGTCTTACTCTCTTTCTCTGCCTGTCTGTCTGTCTCTCCCTCCCTTTCTGTTTCTCTCTCTCTCTCTCTCTCTCTCCCCCTCTCCCTGTCTGTTTCTCTCCGTCTCTCTCTCTTTCTGTCTGTTTCTCACTGTCTCTCTCTGTCCATCTCTCTCTCTCTCTGTCTGTCTCTTTCGTTCTCTCTGTCTGTCTGTCTCTCTCTCTCTCTCTCTCTCTCTCTCTCTTTCTGTCTCTCACTCTCTGTGTGTATCTTCTGTCTTACTCTCCTTCTCTGCCTGTCCGTCTGTCTGTCTGTCTGTCTGTCTCTCTCTCCCTTTCTGTCTCTCTCTCTCTCTGTCCCTCTCTCTTTCTGTCTGTTCCTCTCTCTCTCTCTGTCTCTGTCTTTCTCTGTCTGTCTGCCTCTCTCTTTCTTTCTCTTTCTGTGTCTCTCTGTCTCTCTCTCTGTGCCTATCTTCTGTCTTACTCTCTTTCTCTGCCTGCCTGCCTGTCTGTCTGTCTGTCTCTCTCTGTCTCTCTCCCTGCCTT", "CGTAGGTTGACTCCTTGAGTCCCCTAGGCCATTGCACTGTAGCCTGGGCAGCAAGAGCCAAACTCCGTCCCCCCACCTCCCCGCGCACATAATAACTAACTAACAAACTAACTAACTAACTAAACTAACTAAATAAATAAAATCTCTACACGTCACCTCTAAGTGTGTGTTCCCGTGAGGAGTGATTTCTAAGAAATGGCACTGTACACTGAACGCAGTGGCTCACGTCTGTCATCCCGAGGTCAGGAGTTCGAGACCAGCCCGGCCAACGTGGTGAAACCCCCGTCTCTACTGAAAATACGAAATGGAGTCAGGCGCCGTGGGGCAGGCACCTGTAACCCCAGCTACTCGGGAGGCTGGGGTGGAAGAATTGCTTGAACCTGGCAGGCGGAGGCTGCAGTGACCCAAGATCGCACCACTGCACTACAGCCTGGGCGACAGAGTGAGACCCGGTCTCCAGATAAATACGTACATAAATAAATACACACATACATACATACATACATACATACATACATACATACATACATCCATGCATACAGATATACAAGAAAGAAAAAAAGAAAAGAAAAGAAAGAGAAAATGAAAGAAAAGGCACTGTATTGCTACTGGGCTAGGGCCTTCTCTCTG");
		Test("TTCGCATGATCTAGCTAGCTACGACTGAACGTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATCGTACGTACGTAGCTAGCTAGCTACTAGTAGCGTATCATC", "TTCGCATGATCTAGCTAGCTACGACTGAACGTATCGTACGTACGTAGCTAGCTAGCTACTAGTAGCGTATCATC");
		Test("TTCG", "TTCG");

	/*	Test("AAAAATGGTTTGACATTTCTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTCTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTAAAAATGGGAATCCCCCACCCATATCCATATTAAATAGAAAGAACTTGGGAGTCTAGAGTTTAGATTAATAGTGAAATGGA",
			 "AAAAGATGGTTTGACCTTTTAAAATGGGAACTCCCCCACCCCATATCCATATTAAATAGACGAACCTTGGGAGCTCTAGAGTTTTAGATTAATAAGTGAAATGAGGTTGAATTTCCAGAAAAAGGGTTTCCCTGTAGGCTGCAGACTGGCTAATTAGGGTGTTCTAGACCTACATTGTAACAGCAGGTAGAAGGCAGGCCCTAAACCTTTTAGCTATTGTCGTTGCCACCTGCTCAGAGTCTAGTTCTGAAAGGAGGCGGGTGCGGAGTTATGAAAATTTGGTAAATATAAAAGTCCCGGACGCGACCGCCCGTAGCCTGCGGCCCCTGGC");

	Test("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC",
			 "taagggtggcaacagcttttcactgtgttagtctccaggtacatcacaatcgtgtctgcttctttaaccctgtccataaagtctcttcattTGAGCCATTTCCTAAGAGGAG");

	Test("TTTTTTTTTTTTTTGAGACGGAATTCTTTCTTTGTTGCTAGGCCTGTAGCTGGCATGGCACAATCTCAGCTCACTGCGCCCTCCACCTCCCAAGTTCAAAGTGATTCTTGTCTCAGCCCCTGAGTACTGGGATTACAGGCAAATGCCACACACAACCATCGCTGATTTTTGTAATTTTTAGTAGAGACGGGTTCACCACGGGGGGGGGGGGGGGG",
			 "GTAGTGCAAATCCTCCTGTAAAAAGCCTACttttttttttttttttttgagacggagtcttgctttgttgcctaggctgtagtgcagtggcacaatctcagctcactgcgccctccacctcccaagttcaagtgattctcctgtctcagcctcctgagtagctgggattacaggcaaatgccaccaccccggctgatttttgtatttttagtagagacggggtttcacca");
	Test("TCGCCGCGGAGGGCGAAGCGATTTCTGGAAAGGCGCCCGGAGAAACCAGTGTGGATACTGAGGTTGGGTGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG",
			 "GTGTTTCTAACCCAGTTCGTGGATTCAAAGGTGGCTCCGCGCCGAGCGCGGCCGGCGACTTGTAGGACCTCAGCCCTGGCCGCGGCCGCCGCGCACGCCCTCGGAAGACTCGGC");

	Test("GGGTGGGGGGGGGGGGGGGGGGGGTTTGGGGGGGGGGGGGGGGGGGGGGGGGAAGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGTGACAGCATGAGGGAGAATGACCCAGCCTGGCGAAGCAGCGGCGCGTGGAGTGACATAGCGGCAGCCGAATCCAGCCGCGGCGCTCGGCGCGGCGCGGTAGAGCCTGGCGAAGCGGACACCGGCGCCGGCGGGCTCTGAGCTGCCGCTCCTGCAAGGCCCGCAACCATCCACACCCGCACGCATCACCAACTCTTCACTGACAACATCTGCGGCCCGAGTTTCGGCCGCGAAAGACGCGGGGACTGCTGTGTCG", 
			 "GATTTACTGTGAACAGCATGGAGGAGAATGACCCCAAGCCTGGCGAAgcagcggcggcggtggagggacagcggcagccggaatccagccccggcggcggctcgggcggcggcggcggtagcagcccgggcgaagcggacaccgggcgccggcggGCTCTGATGCTGCCCGCGGTCCTGCAGGCGCCCGGCAACCACCAGCACCCGCACCGCATCACCAACTTCTTCATCGACAACATCCTGCGGCCCGAGTTCGGCCGGCGAAAGGACGCGGGGACCTGCTGTGCGggcgcgggaggaggaaggggcggcggagccggcggcgaaggcggcgcgagcggtgcggagggaggcggcggcgcgggcggcTCGGAGCAGCTCTTGGGCTCGGGCTCCCGAGAGCCCCGGCAGAACCCGCCAT");

	Test("CTTGATTAAAGCTCTGTGTTGCATCATTGGCTTGTGTACATGCGGTGT",
			 "GGCACTTGATGTACATGCGGATGTG");
	
	Test("TAAATGTTGTTCCAGGAGTTGTATTAAAAAAAAAAAAATAAAAAAAAAAAATTGCTTAATCGCAGTTTCAAGGTGT",
			 "TAAAATGACTTGTTCCAGGAAGTTGTAATGCATTCATCCCGCAGTTTTCACAGGTGT");

	Test("AGA","agccatcaagtttgcggcaatttgtcacagcagcaagagg");

	Test("AGCAGAGCAAGCTTTGATCCAGGATTATCTAATACAAACGCTGTCTTTCCCATTATGCCGAGTCC",
			 "atgagtgtagc");

	Test("AATCCTGGGCACACTAGACGAAACAACAGGGGATGTTCCGGACGCATT","T");

	Test("CAGGCGCTCATCGCACCGTGACCCCCCTCAATTCTTAACATGATATCCAATTT",
			 "TTT");

	Test("CAATTCAGCAGCCTCTTGCCCCATCCTGACAAGGAAACCTCGCTCAAAATCAACGAAGCCAGGCGCTCATCGCACCGTGACCCCCCTCAATTCTTAACATGATATCCAATTT",
			 "TTT");

  Test("AACATACCCATTACCTCACTATCTACTGGCGGAGCCTCACAATCGTATGAAAAAGAACACTCT",
			 "ATGGGCTTCTTTTGTCAGTGACCTGGTGCTGAAAACATCTGTAATTCAGGGGAAT");

	Test("AGA","agccatcaagtttgcggcaatttgtcacagcagcaagagg");


	*/

}
/*
	string target("CACACTTAAAAATGAATCATAGC");
	string query("CAtCACATAGC");


	AffineOneGapAlign(query, target, 4, -8, -3, 2, aln);

	aln;
	AffineOneGapAlign(target, query, 4, -4, -3, 2, aln);

	string target2("CACACTTAAAAAAATAACCAATAAGGATCATACAACAATGAAACCTTGAAACATAAAGGTTCAGGAAGAAGCACAAGAAAAACTCTGCCCGCCTATCTTCACTTTGCCTCCAGAACAAAGACAATTCAGTCAATAACATTGTCCTCCTAACTGTGACGTCTGAATCATAGC");
	string query2("CACATTAAAgAAAATgACCGTCTGAATCATAGC");
	Alignment aln2;
	AffineOneGapAlign(query2, target2, 4, -4, -3, 7, aln2);


	string target3("CACtTTAAAgAAATgACCGTCTGtATCAAGC");
	string query3("CACATTAAAgAAAATgACCGTCTGAATCATAGC");
	AffineOneGapAlign(query3, target3, 4, -4, -3, 7, aln);
*/
