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

int FIFO(int page_size, bool prepaging, string ptrace) {
	ifstream ifs (ptrace);
	int faults = 0;
	if(prepaging) {
		string process_temp1, location_temp1, process_temp2, location_temp2;
		while (getline(ifs, process_temp1, ' ')) {
			getline(ifs, location_temp1);
			if (getline(ifs, process_temp2, ' ')) {
				getline(ifs, location_temp2);

			}
			else {
				// code if odd number of requests during prepaging
			}

		}
	}
	else {
		string process_temp, location_temp;
		while (getline(ifs, process_temp, ' ')) {
			getline(ifs, location_temp);

		}
	}

	return faults;
}

int LRU(int page_size, bool prepaging, string ptrace) {
	ifstream ifs (ptrace);
	int faults = 0;
	if(prepaging) {
		string process_temp1, location_temp1, process_temp2, location_temp2;
		while (getline(ifs, process_temp1, ' ')) {
			getline(ifs, location_temp1);
			if (getline(ifs, process_temp2, ' ')) {
				getline(ifs, location_temp2);

			}
			else {
				// code if odd number of requests during prepaging
			}

		}
	}
	else {
		string process_temp, location_temp;
		while (getline(ifs, process_temp, ' ')) {
			getline(ifs, location_temp);

		}
	}

	return faults;
}

int clock(int page_size, bool prepaging, string ptrace) {
	ifstream ifs (ptrace);
	int faults = 0;
	if(prepaging) {
		string process_temp1, location_temp1, process_temp2, location_temp2;
		while (getline(ifs, process_temp1, ' ')) {
			getline(ifs, location_temp1);
			if (getline(ifs, process_temp2, ' ')) {
				getline(ifs, location_temp2);

			}
			else {
				// code if odd number of requests during prepaging
			}

		}
	}
	else {
		string process_temp, location_temp;
		while (getline(ifs, process_temp, ' ')) {
			getline(ifs, location_temp);

		}
	}

	return faults;
}

bool power_of_2(int x) {
	return x && !(x & (x - 1));
}

int main(int argc, char** argv) {
	int page_size;
	string plist, ptrace, algo, paging;
	bool prepaging;
	istringstream iss;

	if (argc != 6) {
		cerr << "Usage: ./assign2 <plist> <ptrace> <page size> <algorithm> <prepaging?>" << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[1]);
	if (!(iss >> plist)) {
		cerr << "Error: Invalid process list '" << argv[1] << "'." << endl;
		return 1;
	}
	ifstream ifs (plist);
	if (!(ifs.good())) {
		cerr << "Error: Invalid process list '" << argv[1] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[2]);
	if (!(iss >> ptrace)) {
		cerr << "Error: Invalid process trace '" << argv[2] << "'." << endl;
		return 1;
	}
	ifstream ifs2 (ptrace);
	if (!(ifs2.good())) {
		cerr << "Error: Invalid process trace '" << argv[2] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[3]);
	if (!(iss >> page_size) || page_size > 32 || page_size < 1 || !(power_of_2(page_size))) {
		cerr << "Error: Invalid page size '" << argv[3] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[4]);
	if (!(iss >> algo) || (algo != "FIFO" && algo != "LRU" && algo != "Clock")) {
		cerr << "Error: Invalid algorithm '" << argv[4] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[5]);
	if (!(iss >> paging) || (paging != "+" && paging != "-")) {
		cerr << "Error: Invalid paging input '" << argv[5] << "'." << endl;
		return 1;
	}
	iss.clear();

	if (paging == "+") {
		prepaging = true;
	}
	else {
		prepaging = false;
	}

	vector<int> list_vector;
	string temp;
	while (getline(ifs, temp, ' ')) {
		getline(ifs, temp);
		list_vector.push_back(stoi(temp));
	}

	int sum = 0;
	for (int i=0; i<list_vector.size(); ++i) {
		sum += list_vector[i];
	}

	page_tables = new tuple<bool, int>*[list_vector.size()];
	int size, allocated;
	for (int i=0; i<list_vector.size(); ++i) {
		size = ceil((double)list_vector[i]/page_size);
		allocated = 512 / list_vector.size();
		page_tables[i] = new tuple<bool, int>[size];
		if (allocated > size) {
			for (int j=allocated; j<size; ++j) {
				page_tables[i][j] = tuple<bool, int> (true, 0);
			}
		}
		else {
			for (int j=0; j<allocated; ++j) {
				page_tables[i][j] = tuple<bool, int> (true, 0);
			}
			for (int j=allocated; j<size; ++j) {
				page_tables[i][j] = tuple<bool, int> (false, 0);
			}
		}
	}

	//run code
	int faults;
	if (algo == "FIFO") {
		faults = FIFO(page_size, prepaging, ptrace);
	}
	else if (algo == "LRU") {
		faults = LRU(page_size, prepaging, ptrace);
	}
	else {
		faults = clock(page_size, prepaging, ptrace);
	}

	cout << "Number of page faults: " << faults << endl;

	for (int i=0; i<10; ++i) {
		delete page_tables[i];
	}
	delete page_tables;

	return 0;
}