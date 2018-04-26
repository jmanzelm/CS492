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
#include <queue>

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

void change_dir(string new_dir){
	for (int i=0; i<cur_dir->branches.size(); ++i) {
		if (cur_dir->branches[i]->name == new_dir) {
			cur_dir = cur_dir->branches[i];
			path.push_back(cur_dir->name);
			return;
		}
	}
	cerr << "Error: No such directory " << new_dir << endl;	
}

void change_dir_parent(){
	if(cur_dir == root){
		return;
	}
	cur_dir = root;
	path.pop_back();
	for (int i=0; i<path.size() - 1; ++i) {
		for (int j=0; j<cur_dir->branches.size(); ++j) {
			if (cur_dir->branches[j]->name == path[i]) {
				cur_dir = cur_dir->branches[j];
				break;
			}
		}
	}
}

void list_dir(){
	for (int i=0; i<cur_dir->branches.size(); ++i) {
		cout << cur_dir->branches[i]->name << " ";
	}
}

void make_dir(string new_dir){
	for (int i=0; i<cur_dir->branches.size(); ++i) {
		if (cur_dir->branches[i]->name == new_dir) {
			cerr << "Error: Directory " << new_dir << " already exists" << endl;
			return;
		}
	}
	tree* temp = new tree;
	temp->file = false;
	temp->name = new_dir;
	temp->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());	
	cur_dir->branches.push_back(temp);
}

void create_file(string new_file){
	for (int i=0; i<cur_dir->branches.size(); ++i) {
		if (cur_dir->branches[i]->name == new_file) {
			cerr << "Error: File " << new_file << " already exists" << endl;
			return;
		}
	}	
	tree* temp = new tree;
	temp->file = true;
	temp->name = new_file;
	temp->size = 0;
	temp->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());	
	cur_dir->branches.push_back(temp);
}

void append_file(tree* file, int file_size, int block_size){
	disk_list* TD_list = disk;
	disk_list* t;
	while (TD_list != NULL) {
		if (TD_list->used == false) {

			if ((TD_list->max - TD_list->min) + 1 < file_size) {
				for (int i=TD_list->min; i<=TD_list->max; ++i) {
					file->memory.push_back(i * block_size);
				}

				file_size -= (TD_list->max - TD_list->min) + 1;

				if (TD_list->prev != NULL) {
					if (TD_list->next != NULL) {
						TD_list->prev->max = TD_list->next->max;
						TD_list->prev->next = TD_list->next->next;
						if (TD_list->next->next != NULL) {
							TD_list->next->next->prev = TD_list->prev;
						}
						delete TD_list->next;
						delete TD_list;
					}
					else{
						cerr << "File too big" << endl;
						return;
					}
				}
				else if (TD_list->next != NULL) {
					TD_list->max = TD_list->next->max;
					TD_list->used = true;
					if (TD_list->next->next != NULL) {
						TD_list->next->next->prev = TD_list;
					}
					t = TD_list->next->next;
					delete TD_list->next;
					TD_list->next = t;
				}
				else {
					cerr << "File too big" << endl;
					return;
				}
			}

			else if ((TD_list->max - TD_list->min) + 1 == file_size) {
				for (int i=TD_list->min; i<=TD_list->max; ++i) {
					file->memory.push_back(i * block_size);
				}

				if (TD_list->prev != NULL) {
					if (TD_list->next != NULL) {
						TD_list->prev->max = TD_list->next->max;
						TD_list->prev->next = TD_list->next->next;
						if (TD_list->next->next != NULL) {
							TD_list->next->next->prev = TD_list->prev;
						}
						delete TD_list->next;
						delete TD_list;
					}
					else{
						cerr << "File too big" << endl;
						return;
					}
				}
				else if (TD_list->next != NULL) {
					TD_list->max = TD_list->next->max;
					TD_list->used = true;
					if (TD_list->next->next != NULL) {
						TD_list->next->next->prev = TD_list;
					}
					t = TD_list->next->next;
					delete TD_list->next;
					TD_list->next = t;
				}
				else {
					cerr << "File too big" << endl;
					return;
				}
				break;
			}
				
			else {
				for (int i=TD_list->min; i < TD_list->min + file_size; ++i) {
					file->memory.push_back(i * block_size);
				}

				if (TD_list->prev == NULL) {
					disk_list* ND_block = new disk_list;
					ND_block->min = TD_list->min + file_size;
					ND_block->max = TD_list->max;
					ND_block->used = false;
					ND_block->next = TD_list->next;
					ND_block->prev = TD_list;
					if (TD_list->next != NULL) {
						TD_list->next->prev = ND_block;
					}
					TD_list->next = ND_block;
					TD_list->used = true;
					TD_list->max = TD_list->min + file_size - 1;
				}

				else {
					for (int i=TD_list->min; i < TD_list->min + file_size; ++i) {
						file->memory.push_back(i * block_size);
					}

					TD_list->prev->max = TD_list->prev->max + file_size;
					TD_list->min = TD_list->min + file_size;
				}
				break;
			}
		}

		TD_list = TD_list->next;
	}

	file->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
}

void remove_file(tree* file, int file_size, int disk_size, int block_size){
	disk_list* TD_list = disk;
	disk_list* t;
	int location;

	for (int i=0; i<file_size; ++i) {
		TD_list = disk;
		location = (file->memory[file->memory.size() - 1]) / block_size;

		while (TD_list->max < location) {
			TD_list = TD_list->next;
		}

		if (location == TD_list->min) {
			if (TD_list->prev != NULL) {
				TD_list->prev->max += 1;
				TD_list->min += 1;
				if (TD_list->min == TD_list->max) {
					TD_list->prev->next = TD_list->next;
					if (TD_list->next != NULL) {
						TD_list->next->prev = TD_list->prev;
					}
					delete TD_list;
				}
			}

			else {
				disk_list* ND_block = new disk_list;
				ND_block->min = 1;
				ND_block->max = TD_list->max;
				ND_block->used = true;
				ND_block->next = TD_list->next;
				ND_block->prev = TD_list;

				TD_list->max = 0;
				TD_list->next = ND_block;
				TD_list->used = false;

			}
		}

		else if (location == TD_list->max) {
			if (TD_list->next != NULL) {
				TD_list->next->min -= 1;
				TD_list->max -= 1;
				if (TD_list->min == TD_list->max) {
					TD_list->next->prev = TD_list->prev;
					if (TD_list->prev != NULL) {
						TD_list->prev->next = TD_list->next;
					}
					else {
						disk = TD_list->next;
					}
					delete TD_list;
				}
			}

			else {
				disk_list* ND_block = new disk_list;
				ND_block->min = (disk_size / block_size) - 1;
				ND_block->max = ND_block->max;
				ND_block->used = false;
				ND_block->next = NULL;
				ND_block->prev = TD_list;

				TD_list->max = ND_block->min - 1;
				TD_list->next = ND_block;

			}
		}

		else {
			disk_list* ND_block1 = new disk_list;
			ND_block1->min = location;
			ND_block1->max = location;
			ND_block1->used = false;
			ND_block1->prev = TD_list;

			disk_list* ND_block2 = new disk_list;
			ND_block2->min = location + 1;
			ND_block2->max = TD_list->max;
			ND_block2->used = true;
			ND_block2->prev = ND_block1;

			ND_block1->next = ND_block2;

			ND_block2->next = TD_list->next;

			TD_list->max = location - 1;
			if (TD_list->next != NULL) {
				TD_list->next->prev = ND_block2;
			}
			TD_list->next = ND_block1;
		}


		file->memory.pop_back();
	}

	file->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
}

void delete_node(tree* node, int disk_size, int block_size){
	if (node->file) {
		remove_file(node, node->memory.size(), disk_size, block_size);
	}

	for (int i=0; i<cur_dir->branches.size(); ++i) {
		if (cur_dir->branches[i]->name == node->name) {
			delete node;
			cur_dir->branches.erase(cur_dir->branches.begin() + i);
			break;
		}
	}

}

//Breadth First Search of the root directory - Prints the traversal containing only directories
void dir_tree() {
	queue<tree*> Q;
	vector<tree*> children;
	string path = "";
	tree* dir = new tree;
	Q.push(root);

	while(!Q.empty()){
		dir = Q.front();
		if(dir->file == false){
			path += dir->name + " ";

			Q.pop();

			for (int i = 0; i < dir->branches.size(); ++i){
				Q.push(dir->branches[i]);
			}
		}
		//skip if file
		else{

			Q.pop();

			for (int i = 0; i < dir->branches.size(); ++i){
				Q.push(dir->branches[i]);
			}
		}

	}
	cout << path << endl;
}

//Prints every file's attributes in the root directory
void print_files() {
	queue<tree*> Q;
	vector<tree*> children;
	tree* dir = new tree;
	vector<string> file_att;
	string attributes = "";

	Q.push(root);

	while(!Q.empty()){
		dir = Q.front();
		// If the node is actually a directory
		if(dir->file == false){

			Q.pop();

			for (int i = 0; i < dir->branches.size(); ++i){
				Q.push(dir->branches[i]);
			}
		}
		else{
			string memoryloc = "";
			for(int j = 0; j < dir->memory.size(); ++j){
				memoryloc += dir->memory[j] + " ";
			}
			string filesize = to_string(dir->size);
			string time = to_string(dir->timestamp);
			attributes = "FileName: " + dir->name + ", Memory Locations: " + memoryloc + ", File Size: " + filesize + ", Time " + time;
			file_att.push_back(attributes);

			Q.pop();

			for (int i = 0; i < dir->branches.size(); ++i){
				Q.push(dir->branches[i]);
			}
		}

	}
	for(int x = 0; x < file_att.size(); ++x){
		cout << file_att[x];
	}
}

void print_disk() {
	disk_list* TD_list = disk;
	vector<bool> bools;
	int counter = 0;
	int temp = 0;
	int frag = 0;
	int i = 0;
	while (TD_list !=NULL){
		if (TD_list->used == true){
			bools.push_back(true);
			TD_list = TD_list->next;
		}
		else{
			bools.push_back(false);
			TD_list = TD_list->next;
		}
	}
	while(counter < bools.size()){
		while(bools[i] = true){
			if(bools[i-1] == false){
				temp = counter;
			}
			++counter;
			++i;
		}
		string tempstring = to_string(temp);
		string counterstring = to_string(counter);
		cout << "In use: " + tempstring +"-" + counterstring;
		while(bools[i] = false){
			if(bools[i-1] == true){
				temp = counter;
			}
			++counter;
			++i;
		}
		frag = frag + (counter - temp) + 1;
		counterstring = to_string(counter);
		tempstring = to_string(temp);
		cout << "Free: " + tempstring +"-" + counterstring;
	}
	string fragstring = to_string(frag);
	cout << "fragmentation: " + fragstring + " bytes";
}

void delete_tree(tree* t) {
	if (t->branches.size() == 0) {
		delete t;
	}
	else {
		for (int i=0; i<t->branches.size(); ++i) {
			delete_tree(t->branches[i]);
		}
	}
}

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

	root = new tree;
	root->file = false;
	root->name = "/";
	root->size = 0;
	root->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());

	cur_dir = root;

	disk = new disk_list;
	disk->min = 0;
	disk->max = (disk_size/block_size) - 1;
	disk->used = false;
	disk->next = NULL;
	disk->prev = NULL;

	string line;
	string dir;
	int pos;
	bool found = false;
	getline(ifs2, line);
	// add directories into tree
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

		disk_list* TD_list = disk;
		disk_list* t;
		vector<long> locations;
		// load files into tree and onto disk
		while (TD_list != NULL) {
			if (TD_list->used == false) {

				if ((TD_list->max - TD_list->min) + 1 < file_size) {
					for (int i=TD_list->min; i<=TD_list->max; ++i) {
						locations.push_back(i * block_size);
					}

					file_size -= (TD_list->max - TD_list->min) + 1;

					if (TD_list->prev != NULL) {
						if (TD_list->next != NULL) {
							TD_list->prev->max = TD_list->next->max;
							TD_list->prev->next = TD_list->next->next;
							if (TD_list->next->next != NULL) {
								TD_list->next->next->prev = TD_list->prev;
							}
							delete TD_list->next;
							delete TD_list;
						}
						else{
							cerr << "File too big" << endl;
							return 1;
						}
					}
					else if (TD_list->next != NULL) {
						TD_list->max = TD_list->next->max;
						TD_list->used = true;
						if (TD_list->next->next != NULL) {
							TD_list->next->next->prev = TD_list;
						}
						t = TD_list->next->next;
						delete TD_list->next;
						TD_list->next = t;
					}
					else {
						cerr << "File too big" << endl;
						return 1;
					}
				}

				else if ((TD_list->max - TD_list->min) + 1 == file_size) {
					for (int i=TD_list->min; i<=TD_list->max; ++i) {
						locations.push_back(i * block_size);
					}

					if (TD_list->prev != NULL) {
						if (TD_list->next != NULL) {
							TD_list->prev->max = TD_list->next->max;
							TD_list->prev->next = TD_list->next->next;
							if (TD_list->next->next != NULL) {
								TD_list->next->next->prev = TD_list->prev;
							}
							delete TD_list->next;
							delete TD_list;
						}
						else{
							cerr << "File too big" << endl;
							return 1;
						}
					}
					else if (TD_list->next != NULL) {
						TD_list->max = TD_list->next->max;
						TD_list->used = true;
						if (TD_list->next->next != NULL) {
							TD_list->next->next->prev = TD_list;
						}
						t = TD_list->next->next;
						delete TD_list->next;
						TD_list->next = t;
					}
					else {
						cerr << "File too big" << endl;
						return 1;
					}
					break;
				}
					
				else {
					for (int i=TD_list->min; i < TD_list->min + file_size; ++i) {
						locations.push_back(i * block_size);
					}

					if (TD_list->prev == NULL) {
						disk_list* ND_block = new disk_list;
						ND_block->min = TD_list->min + file_size;
						ND_block->max = TD_list->max;
						ND_block->used = false;
						ND_block->next = TD_list->next;
						ND_block->prev = TD_list;
						if (TD_list->next != NULL) {
							TD_list->next->prev = ND_block;
						}
						TD_list->next = ND_block;
						TD_list->used = true;
						TD_list->max = TD_list->min + file_size - 1;
					}

					else {
						for (int i=TD_list->min; i < TD_list->min + file_size; ++i) {
							locations.push_back(i * block_size);
						}

						TD_list->prev->max = TD_list->prev->max + file_size;
						TD_list->min = TD_list->min + file_size;
					}
					break;
				}
			}

			TD_list = TD_list->next;
		}

		temp->timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
		temp->memory = locations;

		cur_dir->branches.push_back(temp);
		cur_dir = root;
	}

	regex cd("cd *");
	regex cd_back("cd\.\.");
	regex ls("ls");
	regex mkdir("mkdir *");
	regex create("create *");
	regex app("append * *");
	regex rem("remove * *");
	regex del("delete *");
	regex ex("exit");
	regex dirrect("dir");
	regex prfiles("prfiles");
	regex prdisk("prdisk");
	string command;
	// process user input
	while(1) {
		cout << "/";
		for (int i=0; i<path.size(); ++i) {
			cout << path[i] << "/";
		}
		cout << "$ "<< flush;

		cin >> command;

		if (regex_match(command, cd)) {
			command.erase(0,3);
			change_dir(command);
		}
		
		else if (regex_match(command, cd_back)) {
			change_dir_parent();
		}
		
		else if (regex_match(command, ls)) {
			list_dir();
		}
		
		else if (regex_match(command, mkdir)) {
			command.erase(0,6);
			make_dir(command);
		}
		
		else if (regex_match(command, create)) {
			command.erase(0,7);
			create_file(command);
		}
		
		else if (regex_match(command, app)) {
			command.erase(0,7);
			bool found = false;
			int pos = command.find(" ");
			string dir = command.substr(0, command.find(" "));
			found = false;
			tree* temp_tree;
			for (int i=0; i<cur_dir->branches.size(); ++i) {
				if (cur_dir->branches[i]->name == dir) {
					temp_tree = cur_dir->branches[i];
					found = true;
					break;
				}
			}

			if (!found) {
				cout << "Error: No such file" << dir << endl;
				continue;
			}

			line.erase(0, pos+1);

			int f_size = (int)ceil((double)stoi(command) / block_size);

			append_file(temp_tree, f_size, block_size);
		}
		
		else if (regex_match(command, rem)) {
			command.erase(0,7);
			bool found = false;
			int pos = command.find(" ");
			string dir = command.substr(0, command.find(" "));
			found = false;
			tree* temp_tree;
			for (int i=0; i<cur_dir->branches.size(); ++i) {
				if (cur_dir->branches[i]->name == dir) {
					temp_tree = cur_dir->branches[i];
					found = true;
					break;
				}
			}

			if (!found) {
				cout << "Error: No such file" << dir << endl;
				continue;
			}

			line.erase(0, pos+1);

			int f_size = (int)ceil((double)stoi(command) / block_size);

			if (f_size < 0) {
				cout << "Error: invalid removal size" << endl;
			}

			if (f_size > temp_tree->memory.size()) {
				f_size = temp_tree->memory.size();
			}

			remove_file(temp_tree, f_size, disk_size, block_size);
		}
		
		else if (regex_match(command, del)) {
			command.erase(0,7);

			bool found = false;
			tree* temp_tree;
			for (int i=0; i<cur_dir->branches.size(); ++i) {
				if (cur_dir->branches[i]->name == command) {
					temp_tree = cur_dir->branches[i];
					found = true;
					break;
				}
			}

			if (!found) {
				cout << "Error: No such file" << command << endl;
				continue;
			}

			if (!(temp_tree->file)) {
				if (temp_tree->branches.size() != 0) {
					cout << "Error: directory contains files" << endl;
				}
			}
			delete_node(temp_tree, disk_size, block_size);
		}
		
		else if (regex_match(command, ex)) {
			break;
		}
		
		else if (regex_match(command, dirrect)) {
			dir_tree();
		}
		
		else if (regex_match(command, prfiles)) {
			print_files();
		}
		
		else if (regex_match(command, prdisk)) {
			print_disk();
		}

		else {
			cout << "Invalid command: " << command << endl;
		}
	}

	disk_list* temp_disk = disk->next;
	while(disk != NULL) {
		delete disk;
		disk = temp_disk;
		if (temp_disk != NULL) {
			temp_disk = temp_disk->next;
		}
	}

	delete_tree(root);

	ifs.close();
	ifs2.close();
	return 0;
}