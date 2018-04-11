/*************************************************************************
	I pledge my honor that I have abided by the Stevens Honor System.
	Jacob Manzelmann, Gavin Burke
*************************************************************************/

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <string>
#include <tuple>
#include <fstream>
#include <math.h>

using namespace std;

tuple<bool, int>** page_tables;
long counter = 0;

int FIFO(int page_size, bool prepaging) {
	ifstream ifs ("ptrace.txt");

	int faults = 0;
	string process_temp, location_temp;
	while (getline(ifs, process_temp, ' ')) {
		getline(ifs, location_temp);

	}

	return faults;
}

int LRU(int page_size, bool prepaging) {
	ifstream ifs ("ptrace.txt");

	int faults = 0;
	string process_temp, location_temp;
	while (getline(ifs, process_temp, ' ')) {
		getline(ifs, location_temp);

	}

	return faults;
}

int clock(int page_size, bool prepaging) {
	ifstream ifs ("ptrace.txt");

	int faults = 0;
	string process_temp, location_temp;
	while (getline(ifs, process_temp, ' ')) {
		getline(ifs, location_temp);

	}

	return faults;
}

bool power_of_2(int num) {
	return x && !(x & (x - 1));
}

int main(int argc, char** argv) {
	int page_size;
	string algo, paging;
	bool prepaging;
	istringstream iss;

	if (argc != 4) {
		cerr << "Usage: ./assign2 <page size> <algorithm> <prepaging?>" << endl;
	}
	iss.clear();

	iss.str(argv[1]);
	if (!(iss >> page_size) || page_size > 32 || page_size < 1 || !(power_of_2(page_size))) {
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
	if (paging == "+") {
		prepaging = true;
	}
	else {
		prepaging = false;
	}

	int plist[10];
	string temp;
	ifstream ifs ("plist.txt");
	for (int i=0; i<10; ++i) {
		getline(ifs, temp, ' ');
		getline(ifs, temp);
		plist[i] = stoi(temp);
	}

	int sum = 0;
	for (int i=0; i<10; ++i) {
		sum += plist[i];
	}

	page_tables = new tuple<bool, int>*[10];
	int size, allocated;
	for (int i=0; i<10; ++i) {
		size = plist[i]/page_size;
		allocated = (plist[i]/sum) * 512;
		page_tables[i] = new tuple<bool, int>*[size];
		for (int j=0; j<allocated; ++j) {
			page_tables[i][j] = tuple<bool, int> (true, 0);
		}
		for (int j=allocated; j<size; ++j) {
			page_tables[i][j] = tuple<bool, int> (false, 0);
		}
	}

	//run code
	int faults;
	if (algo == "FIFO") {
		faults = FIFO(page_size, prepaging);
	}
	else if (algo == "LRU") {
		faults = LRU(page_size, prepaging);
	}
	else {
		faults = clock(page_size, prepaging);
	}

	cout << "Number of page faults: " << faults << endl;

	for (int i=0; i<10; ++i) {
		delete page_tables[i];
	}
	delete page_tables;

	return 0;
}