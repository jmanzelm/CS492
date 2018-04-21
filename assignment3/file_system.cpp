/*************************************************************************
	I pledge my honor that I have abided by the Stevens Honor System.
	Jacob Manzelmann, Gavin Burke
*************************************************************************/

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {
	int disk_size, block_size;
	string file_list, dir_list;
	istringstream iss;

	if (argc != 5) {
		cerr << "Usage: ./assign3 <file_list> <dir_list> <disk_size> <block_size>" << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[1]);
	if (!(iss >> file_list)) {
		cerr << "Error: Invalid file list '" << argv[1] << "'." << endl;
		return 1;
	}
	ifstream ifs (file_list);
	if (!(ifs.good())) {
		cerr << "Error: Invalid file list '" << argv[1] << "'." << endl;
		ifs.close();
		return 1;
	}
	iss.clear();
	ifs.close();

	iss.str(argv[2]);
	if (!(iss >> dir_list)) {
		cerr << "Error: Invalid directory list '" << argv[2] << "'." << endl;
		return 1;
	}
	ifstream ifs2 (dir_list);
	if (!(ifs2.good())) {
		cerr << "Error: Invalid directory list '" << argv[2] << "'." << endl;
		ifs2.close();
		return 1;
	}
	iss.clear();
	ifs2.close();

	iss.str(argv[3]);
	if (!(iss >> disk_size) || disk_size < 1) {
		cerr << "Error: Invalid disk size '" << argv[3] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[4]);
	if (!(iss >> block_size) || block_size < 1) {
		cerr << "Error: Invalid bloack size '" << argv[4] << "'." << endl;
		return 1;
	}
	iss.clear();

	return 0;
}