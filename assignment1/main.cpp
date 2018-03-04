/*************************************************************************
	I pledge my honor that I have abided by the Stevens Honor System.
	Jacob Manzelmann, Gavin Burke
*************************************************************************/

#include <iostream>
#include <sstream>
#include <queue>
#include <pthread.h>

using namespace std;

void first_come_first_serve(int num_producers, int num_consumers, int num_products, int Qsize, int seed) {

}

void round_robin(int num_producers, int num_consumers, int num_products, int Qsize, int quantum, int seed) {

}

int main(int argc, char* argv[]) {
	int num_producers, num_consumers, num_products, Qsize, Atype, quantum, seed;
	istringstream iss;

	if (argc != 8) {
		cerr << "Usage: ./assign1 <producers> <consumers> <products> <Q size> <algo type> <quantum> <seed>" << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[1]);
	if (!(iss >> num_producers) || num_producers < 1) {
		cerr << "Error: Invalid number of producers '" << argv[1] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[2]);
	if (!(iss >> num_consumers) || num_consumers < 1) {
		cerr << "Error: Invalid number of consumers '" << argv[2] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[3]);
	if (!(iss >> num_products) || num_products < 1) {
		cerr << "Error: Invalid number of products '" << argv[3] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[4]);
	if (!(iss >> Qsize) || Qsize < 1) {
		cerr << "Error: Invalid queue size '" << argv[4] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[5]);
	if (!(iss >> Atype) || Atype < 0 || Atype > 1) {
		cerr << "Error: Invalid algorithm type '" << argv[5] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[6]);
	if (!(iss >> quantum) || quantum < 1) {
		cerr << "Error: Invalid number of producers '" << argv[6] << "'." << endl;
		return 1;
	}
	iss.clear();

	iss.str(argv[7]);
	if (!(iss >> seed) || seed < 1) {
		cerr << "Error: Invalid number of producers '" << argv[7] << "'." << endl;
		return 1;
	}
	iss.clear();

	if (Atype) {
		round_robin(num_producers, num_consumers, num_products, Qsize, quantum, seed);
		return 0;
	}

	first_come_first_serve(num_producers, num_consumers, num_products, Qsize, seed);
	return 0;
}