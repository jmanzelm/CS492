/*************************************************************************
	I pledge my honor that I have abided by the Stevens Honor System.
	Jacob Manzelmann, Gavin Burke
*************************************************************************/

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <string>

using namespace std;

bool power_of_2(int num) {
	return x && !(x & (x - 1));
}

int main(int argc, char** argv) {
	int page_size;
	string algo, paging;
	istringstream iss;

	if (argc != 4) {
		cerr << "Usage: ./assign2 <page size> <algorithm> <prepaging?>" << endl;
	}
	iss.clear();

	iss.str(argv[1]);
	if (!(iss >> page_size) || page_size > 32 || page_size < 2 || !(power_of_2(page_size))) {
		cerr << "Error: Invalid page size '" << argv[1] << "'." << endl;
	}
	iss.clear();

	iss.str(argv[2]);
	if (!(iss >> algo) || (algo != "FIFO" && algo != "LRU" && algo != "Clock")) {
		cerr << "Error: Invalid algorithm '" << argv[2] << "'." << endl;
	}
	iss.clear();

	iss.str(argv[3]);
	if (!(iss >> paging) || (paging != "+" && paging != "-")) {
		cerr << "Error: Invalid paging input '" << argv[3] << "'." << endl;
	}
	iss.clear();

	return 0;
}