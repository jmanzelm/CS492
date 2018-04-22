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

// list of memory locations in the disk_list allocated for a certain file
typedef struct {
	// id of block in the disk_list
	// block_id = (disk_list index) * block_size
	long block_id;
	file_list* next;
} file_list;

// I thought doubly linked would be better for this
typedef struct {
	// smallest block in node
	int min;
	//largest block in node
	int max;
	bool used;
	disk_list* next;
	disk_list* prev;
} disk_list;

typedef struct {
	// true if a file, false if a directory
	bool file;
	string name;
	// list of memory locations if it is a file
	file_list* memory;
	// below only used if file
	int size;
	// I was looking at the chrono library for creating timestamps:
	// http://en.cppreference.com/w/cpp/chrono
	time_t timestamp;
	// vector of pointers to all other nodes
	// is empty if file or empty directory
	vector<tree*> branches;
} tree;

// the root directory for the system
tree* root;

// the current directory
// will point to a piece of the root directory, so modifying it will change root
tree* cur_dir;

// path to current directory using the names of each directory
vector<string> path;

// says what memory locations are in use
disk_list* disk;

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