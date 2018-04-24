/*************************************************************************
	I pledge my honor that I have abided by the Stevens Honor System.
	Jacob Manzelmann, Gavin Burke
*************************************************************************/

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <regex>
#include <math.h>

using namespace std;

// I thought doubly linked would be better for this
typedef struct d{
	// smallest block in node
	int min;
	//largest block in node
	int max;
	bool used;
	d* next;
	d* prev;
} disk_list;

typedef struct tree{
	// true if a file, false if a directory
	bool file;
	string name;
	// list of memory locations if it is a file
	vector<long> memory;
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

	vector<long> fake;

	root = new tree;
	root->file = false;
	root->name = "/";
	root->memory = fake;
	root->size = 0;
	root->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());

	cur_dir = root;

	disk = new disk_list;
	disk->min = 0;
	disk->max = disk_size - 1;
	disk->used = false;
	disk->next = NULL;
	disk->prev = NULL;

	string line;
	string dir;
	int pos;
	bool found = false;
	getline(ifs2, line);
	while (getline(ifs2, line)) {
		line.erase(0,2);
		while (pos = line.find("/") != string::npos) {
			dir = line.substr(0, line.find("/"));
			found = false;
			for (int i=0; i<cur_dir->branches.size(); ++i) {
				if (cur_dir->branches[i]->name == dir) {
					cur_dir = cur_dir->branches[i];
					found = true;
					break;
				}
			}

			if (!found) {
				tree* temp = new tree;
				temp->file = false;
				temp->name = dir;
				temp->memory = fake;
				temp->size = 0;
				temp->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
				
				cur_dir->branches.push_back(temp);
				cur_dir = cur_dir->branches[cur_dir->branches.size() - 1];
			}

			line.erase(0, pos+1);
		}

		tree* temp = new tree;
		temp->file = false;
		temp->name = line;
		temp->memory = fake;
		temp->size = 0;
		temp->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
		
		cur_dir->branches.push_back(temp);
		cur_dir = root;
	}

	int file_size;
	while (getline(ifs, line)) {
		// erase duplicate spaces
		for(int i=0; i<line.length(); i++) {
			if(line[i]==' ' && line[i+1]==' ') {
				line.erase(i,1);	
				--i;
			}
		}

		// erase up until size
		for (int i=0; i<7; ++i) {
			pos = line.find(" ");
			line.erase(0, pos+1);
		}

		// get size
		pos = line.find(" ");
		file_size = (int)ceil((double)stoi(line.substr(0, pos)) / block_size);
		line.erase(0, pos+1);

		// erase up until path
		pos = line.find("./");
		line.erase(0, pos+2);

		while (pos = line.find("/") != string::npos) {
			dir = line.substr(0, line.find("/"));
			found = false;
			for (int i=0; i<cur_dir->branches.size(); ++i) {
				if (cur_dir->branches[i]->name == dir) {
					cur_dir = cur_dir->branches[i];
					found = true;
					break;
				}
			}

			if (!found) {
				cerr << "Improper file setup." << endl;
				return 1;
			}

			line.erase(0, pos+1);
		}

		tree* temp = new tree;
		temp->file = true;
		temp->name = line;
		temp->size = file_size;
		temp->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());

		disk_list* TD_list = disk;
		disk_list* t;
		int t_max;
		vector<long> locations;
		while (TD_list != NULL) {
			if (TD_list->used == false) {

				if (TD_list->max - TD_list->min + 1 < file_size) {
					TD_list->used = true;
					file_size -= TD_list->max - TD_list->min + 1;
					locations.push_back(TD_list->min * block_size);
				}
				else {
					t = TD_list->next;
					t_max = TD_list->max;

					TD_list->max = TD_list->min + file_size - 1;
					TD_list->used = true;

					if (TD_list->max != t_max) {
						disk_list* ND_block = new disk_list;
						ND_block->min = TD_list->max + 1;
						ND_block->max = t_max;
						ND_block->used = false;
						ND_block->next = t;
						ND_block->prev = TD_list;
						TD_list->next = ND_block;
						t->prev = ND_block;
					}

					locations.push_back(TD_list->min * block_size);
					break;
				}
			}

			TD_list = TD_list->next;
		}

		temp->memory = locations;

		cur_dir->branches.push_back(temp);
		cur_dir = root;
	}

	ifs.close();
	ifs2.close();
	return 0;
}