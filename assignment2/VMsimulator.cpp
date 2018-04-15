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
#include <queue>
#include <limits>

using namespace std;

vector<vector<tuple<bool, int> > > page_tables;
int counter = 1;

int FIFO(int page_size, bool prepaging, string ptrace) {
	ifstream ifs (ptrace);
	int faults = 0;
	vector<queue<int> > QS;

	int k = 0;
	queue<int> TQ;
	for (int i=0; i<page_tables.size(); ++i) {
		QS.push_back(TQ);
		k = 0;
		while (get<0>(page_tables[i][k]) && k < page_tables[i].size()) {
			QS[i].push(k);
			++k;
		}
	}

	if(prepaging) {
		string process_temp, location_temp, process_temp2, location_temp2;
		int proc, proc2, loc, loc2, pop_loc, temp_counter;
		bool good = false;
		while (getline(ifs, process_temp, ' ')) {
			getline(ifs, location_temp);
			good = false;
			proc = stoi(process_temp);
			loc = stoi(location_temp) - 1;
			
			if (!(get<0>(page_tables[proc][(loc) / page_size]))) {
				temp_counter = counter;
				while (getline(ifs, process_temp2, ' ')) {
					++counter;
					getline(ifs, location_temp2);
					proc2 = stoi(process_temp2);
					loc2 = stoi(location_temp2) - 1;

					if (!(get<0>(page_tables[proc2][(loc2) / page_size])) && (proc != proc2 || loc != loc2)) {
						good = true;
						break;
					}
					else {
						page_tables[proc2][(loc2) / page_size] = tuple<bool, int>(true, counter);
					}
				}

				pop_loc	= QS[proc].front();
				QS[proc].pop();
				page_tables[proc][pop_loc] = tuple<bool, int>(false, get<1>(page_tables[proc][pop_loc]));
				QS[proc].push((loc) / page_size);
				page_tables[proc][(loc) / page_size] = tuple<bool, int>(true, temp_counter);

				if (good) {
					pop_loc	= QS[proc2].front();
					QS[proc2].pop();
					page_tables[proc2][pop_loc] = tuple<bool, int>(false, get<1>(page_tables[proc2][pop_loc]));
					QS[proc2].push((loc2) / page_size);
					page_tables[proc2][(loc2) / page_size] = tuple<bool, int>(true, counter);

					++faults;
				}
				++faults;
			}
			else {
				page_tables[proc][(loc) / page_size] = tuple<bool, int>(true, counter);
			}
			++counter;
		}
	}
	else {
		string process_temp, location_temp;
		int proc, loc, pop_loc;
		while (getline(ifs, process_temp, ' ')) {
			getline(ifs, location_temp);
			proc = stoi(process_temp);
			loc = stoi(location_temp) - 1;

			if (!(get<0>(page_tables[proc][(loc) / page_size]))) {
				pop_loc	= QS[proc].front();
				QS[proc].pop();
				page_tables[proc][pop_loc] = tuple<bool, int>(false, get<1>(page_tables[proc][pop_loc]));
				QS[proc].push((loc) / page_size);
				page_tables[proc][(loc) / page_size] = tuple<bool, int>(true, counter);

				++faults;
			}
			else {
				page_tables[proc][(loc) / page_size] = tuple<bool, int>(true, counter);
			}
			++counter;
		}
	}
	return faults;
}

int LRU(int page_size, bool prepaging, string ptrace) {
	ifstream ifs (ptrace);
	int faults = 0;

	if(prepaging) {
		string process_temp, location_temp, process_temp2, location_temp2;
		int proc, proc2, loc, loc2, temp_counter, oldest_time, oldest_index;
		bool good = false;
		while (getline(ifs, process_temp, ' ')) {
			getline(ifs, location_temp);
			good = false;
			proc = stoi(process_temp);
			loc = stoi(location_temp) - 1;
			
			if (!(get<0>(page_tables[proc][(loc) / page_size]))) {
				temp_counter = counter;
				while (getline(ifs, process_temp2, ' ')) {
					++counter;

					getline(ifs, location_temp2);
					proc2 = stoi(process_temp2);
					loc2 = stoi(location_temp2) - 1;

					if (!(get<0>(page_tables[proc2][(loc2) / page_size])) && (proc != proc2 || loc != loc2)) {
						good = true;
						break;
					}
					else {
						page_tables[proc2][(loc2) / page_size] = tuple<bool, int>(true, counter);
					}
				}

				oldest_time = numeric_limits<int>::max();
				for (int i=0; i<page_tables[proc].size(); ++i) {
					if (get<0>(page_tables[proc][i]) && get<1>(page_tables[proc][i]) < oldest_time) {
						oldest_time = get<1>(page_tables[proc][(loc) / page_size]);
						oldest_index = i;
					}
				}
				page_tables[proc][oldest_index] = tuple<bool,int>(false, get<1>(page_tables[proc][oldest_index]));
				page_tables[proc][(loc) / page_size] = tuple<bool,int>(true, temp_counter);

				if (good) {
					oldest_time = numeric_limits<int>::max();
					for (int i=0; i<page_tables[proc2].size(); ++i) {
						if (get<0>(page_tables[proc2][i]) && get<1>(page_tables[proc2][i]) < oldest_time) {
							oldest_time = get<1>(page_tables[proc2][(loc2) / page_size]);
							oldest_index = i;
						}
					}
					page_tables[proc2][oldest_index] = tuple<bool,int>(false, get<1>(page_tables[proc2][oldest_index]));
					page_tables[proc2][(loc2) / page_size] = tuple<bool,int>(true, counter);

					++faults;
				}
				++faults;
			}
			else {
				page_tables[proc][(loc) / page_size] = tuple<bool, int>(true, counter);
			}
			++counter;
		}
	}
	else {
		string process_temp, location_temp;
		int proc, loc, oldest_time, oldest_index = 0;
		while (getline(ifs, process_temp, ' ')) {
			getline(ifs, location_temp);
			proc = stoi(process_temp);
			loc = stoi(location_temp) - 1;

			if (!(get<0>(page_tables[proc][(loc) / page_size]))) {
				oldest_time = numeric_limits<int>::max();
				for (int i=0; i<page_tables[proc].size(); ++i) {
					if (get<0>(page_tables[proc][i]) && get<1>(page_tables[proc][i]) < oldest_time) {
						oldest_time = get<1>(page_tables[proc][(loc) / page_size]);
						oldest_index = i;
					}
				}
				page_tables[proc][oldest_index] = tuple<bool,int>(false, get<1>(page_tables[proc][oldest_index]));
				page_tables[proc][(loc) / page_size] = tuple<bool,int>(true, counter);

				++faults;
			}
			else {
				page_tables[proc][(loc) / page_size] = tuple<bool, int>(true, counter);
			}
			++counter;
		}
	}
	return faults;
}

int clock(int page_size, bool prepaging, string ptrace) {
	ifstream ifs (ptrace);
	int faults = 0;
	vector<vector<bool> > ref;
	vector<bool> TV;

	for (int i=0; i<page_tables.size(); ++i) {
		ref.push_back(TV);
		for (int k=0; k<page_tables[i].size(); ++k){
			ref[i].push_back(get<0>(page_tables[i][k]));
		}
	}
	int[] clock = new int[ref.size()];
	for (int x=0; x<ref.size(); ++x){
		clock[x] = 0;
	}
	if(prepaging) {
		string process_temp, location_temp, process_temp2, location_temp2;
		int proc, proc2, loc, loc2, temp_counter;
		bool good = false;
		while (getline(ifs, process_temp, ' ')) {
			getline(ifs, location_temp);
			good = false;
			proc = stoi(process_temp);
			loc = stoi(location_temp) - 1;
			
			if (!(get<0>(page_tables[proc][(loc) / page_size]))) {
				temp_counter = counter;
				while (getline(ifs, process_temp2, ' ')) {
					++counter;

					getline(ifs, location_temp2);
					proc2 = stoi(process_temp2);
					loc2 = stoi(location_temp2) - 1;

					if (!(get<0>(page_tables[proc2][(loc2) / page_size])) && (proc != proc2 || loc != loc2)) {
						good = true;
						break;
					}
					else {
						page_tables[proc2][(loc2) / page_size] = tuple<bool, int>(true, counter);
					}
				}
				while(ref[proc][clock[proc]]){
					ref[proc][clock[proc]] = false;
					if(clock[proc] == page_tables[proc].size()){
						clock[proc] = 0;
					}
					else{
						++clock[proc];
					}
				page_tables[proc][clock[proc]] = tuple<bool,int>(false, get<1>(page_tables[proc][clock[proc]]));
				page_tables[proc][(loc) / page_size] = tuple<bool,int>(true, counter);
				
				if (good) {
					while(ref[proc2][clock[proc2]]){
						ref[proc2][clock[proc2]] = false;
						if(clock[proc2] == page_tables[proc2].size()){
							clock[proc2] = 0;
					}
						else{
							++clock[proc2];
					}
					page_tables[proc2][clock[proc2]] = tuple<bool,int>(false, get<1>(page_tables[proc2][clock[proc2]]));
					page_tables[proc2][(loc2) / page_size] = tuple<bool,int>(true, counter);
					++faults;
				}
				++faults;
			}
			else {
				page_tables[proc][(loc) / page_size] = tuple<bool, int>(true, counter);
			}
			++counter;
		}
	}
	else {
		string process_temp, location_temp;
		int proc, loc;
		while (getline(ifs, process_temp, ' ')) {
			getline(ifs, location_temp);
			proc = stoi(process_temp);
			loc = stoi(location_temp) - 1;

			if (!(get<0>(page_tables[proc][(loc) / page_size]))) {
				while(ref[proc][clock[proc]]){
					ref[proc][clock[proc]] = false;
					if(clock[proc] == page_tables[proc].size()){
						clock[proc] = 0;
					}
					else{
						++clock[proc];
					}
				}
				page_tables[proc][clock[proc]] = tuple<bool,int>(false, get<1>(page_tables[proc][clock[proc]]));
				page_tables[proc][(loc) / page_size] = tuple<bool,int>(true, counter);
				++faults;
			}
			else {
				ref[proc][(loc) / page_size] = true;
				page_tables[proc][(loc) / page_size] = tuple<bool, int>(true, counter);
			}
			++counter;
		}
	}
	delete clock;
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

	int size, allocated;
	for (int i=0; i<list_vector.size(); ++i) {
		size = ceil((double)list_vector[i]/page_size);
		allocated = (512 / list_vector.size()) / page_size;

		vector<tuple<bool, int> > inner_vec;

		if (allocated > size) {
			for (int j=0; j<size; ++j) {
				inner_vec.push_back(tuple<bool, int>(true, 0));
			}
		}
		else {
			for (int j=0; j<allocated; ++j) {
				inner_vec.push_back(tuple<bool, int>(true, 0));
			}
			for (int j=allocated; j<size; ++j) {
				inner_vec.push_back(tuple<bool, int>(false, 0));
			}
		}

		page_tables.push_back(inner_vec);
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

	return 0;
}