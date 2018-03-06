/*************************************************************************
	I pledge my honor that I have abided by the Stevens Honor System.
	Jacob Manzelmann, Gavin Burke
*************************************************************************/

#include <iostream>
#include <sstream>
#include <queue>
#include <pthread.h>
#include <random>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include <limits>

using namespace std;

typedef struct {
	int id;
	int life;
	double wait;
	double t1;
	double t3;
} product;

typedef struct {
	int num;
} fcfs_args;

typedef struct {
	int quantum;
	int num;
} rr_args;

int max_products;
int prods_made = 0;
int prods_used = 0;
int size;
queue<product*> Q;
vector<double> turn_around;
vector<double> wait;

pthread_mutex_t permissions;
pthread_cond_t cond;

product* create_product() {
	product* new_product = new product;
	new_product->id = random();
	timeval temp;
	gettimeofday(&temp, NULL);
	new_product->t1 = temp.tv_sec+(temp.tv_usec/1000000.0);
	new_product->life = random() % 1024;
}

inline void delete_product(product* prod) {
	delete prod;
}

int fibbo(int num) {
	if (num < 2) {
		return 1;
	}
	return fibbo(num-1) + fibbo(num-2);
}

void* first_come_first_serve_P(void* arguments) {
	fcfs_args* args = (fcfs_args*)arguments;
	pthread_mutex_lock(&permissions);
	while(1) {
		if (prods_made == max_products) {
			break;
		}
		if(Q.size() == size) {
			pthread_cond_signal(&cond);
			pthread_cond_wait(&cond, &permissions);
			continue;
		}

		product* p = create_product();
		timeval temp;
		gettimeofday(&temp, NULL);
		p->t3 = temp.tv_sec+(temp.tv_usec/1000000.0);
		Q.push(p);
		cout << "Producer " << args->num << " has produced product " << p->id << endl;
		++prods_made;

		pthread_mutex_unlock(&permissions);
		usleep(100000);
		pthread_mutex_lock(&permissions);
	}
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&permissions);
}

void* first_come_first_serve_C(void* arguments) {
	fcfs_args* args = (fcfs_args*)arguments;
	pthread_mutex_lock(&permissions);
	while(1) {
		if (prods_used == max_products) {
			break;
		}
		if(Q.empty()) {
			pthread_cond_signal(&cond);
			pthread_cond_wait(&cond, &permissions);
			continue;
		}

		product* p = Q.front();
		Q.pop();

		timeval temp;
		gettimeofday(&temp, NULL);
		double t4 = temp.tv_sec+(temp.tv_usec/1000000.0);
		double t = t4 - (p->t3);
		wait.push_back(t);

		++prods_used;
		cout << "Consumer " << args->num << " has consumed product " << p->id << endl;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&permissions);

		for (int i = 0; i<p->life; ++i) {
			fibbo(10);
		}

		gettimeofday(&temp, NULL);
		double t2 = temp.tv_sec+(temp.tv_usec/1000000.0);
		double t0 = t2 - (p->t1);

		delete_product(p);
		usleep(100000);
		pthread_mutex_lock(&permissions);
		turn_around.push_back(t0);
	}
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&permissions);
}

void* round_robin_P(void* arguments) {
	rr_args* args = (rr_args*)arguments;
	pthread_mutex_lock(&permissions);
	while(1) {
		if (prods_made == max_products) {
			break;
		}
		if(Q.size() == size) {
			pthread_cond_signal(&cond);
			pthread_cond_wait(&cond, &permissions);
			continue;
		}

		product* p = create_product();
		timeval temp;
		gettimeofday(&temp, NULL);
		p->t3 = temp.tv_sec+(temp.tv_usec/1000000.0);
		Q.push(p);
		cout << "Producer " << args->num << " has produced product " << p->id << endl;

		pthread_mutex_unlock(&permissions);
		usleep(100000);
		pthread_mutex_lock(&permissions);
	}
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&permissions);
}

void* round_robin_C(void* arguments) {
	rr_args* args = (rr_args*)arguments;
	pthread_mutex_lock(&permissions);
	while(1) {
		if (prods_used == max_products) {
			break;
		}
		if(Q.empty()) {
			pthread_cond_signal(&cond);
			pthread_cond_wait(&cond, &permissions);
			continue;
		}

		product* p = Q.front();
		Q.pop();

		timeval temp;
		gettimeofday(&temp, NULL);
		double t4 = temp.tv_sec+(temp.tv_usec/1000000.0);
		double t = t4 - (p->t3);
		p->wait += t;

		pthread_cond_signal(&cond);
		if (p->life <= args->quantum) {
			wait.push_back(p->wait);
			cout << "Consumer " << args->num << " has consumed product " << p->id << endl;
			pthread_mutex_unlock(&permissions);
			for (int i = 0; i<p->life; ++i) {
				fibbo(10);
			}
			++prods_used;

			gettimeofday(&temp, NULL);
			double t2 = temp.tv_sec+(temp.tv_usec/1000000.0);
			double t0 = t2 - (p->t1);

			delete_product(p);
			usleep(100000);
			pthread_mutex_lock(&permissions);
			turn_around.push_back(t0);
		}
		else {
			pthread_mutex_unlock(&permissions);
			for (int i = 0; i<args->quantum; ++i) {
				fibbo(10);
			}
			p->life -= args->quantum;
			usleep(100000);
			pthread_mutex_lock(&permissions);
			Q.push(p);
		}
	}
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&permissions);
}

int main(int argc, char* argv[]) {
	int num_producers, num_consumers, num_products, Qsize, Atype, quantum;
	unsigned int seed;
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

	size = Qsize;
	max_products = num_products;
	srandom(seed);

	pthread_t producer_thread[num_producers];
	pthread_t consumer_thread[num_consumers];
	int np[num_producers];
	int nc[num_consumers];

	if (Atype) {
		rr_args* p_args[num_producers];
		rr_args* c_args[num_consumers];
		pthread_mutex_init(&permissions, NULL);
		pthread_cond_init(&cond, NULL);

		timeval total_tv;
		gettimeofday(&total_tv, NULL);
		double total_start = total_tv.tv_sec+(total_tv.tv_usec/1000000.0);

		for (int i=0;i<num_producers;i++){
			np[i] = i;
			rr_args* temp = new rr_args;
			temp->quantum = quantum;
			temp->num = i;
			p_args[i] = temp;
			pthread_create(&producer_thread[i],NULL,round_robin_P,(void*)temp);
  		}

		gettimeofday(&total_tv, NULL);
		double consumer_start = total_tv.tv_sec+(total_tv.tv_usec/1000000.0);

		for (int i=0;i<num_consumers;i++){
			nc[i] = i;
			rr_args* temp = new rr_args;
			temp->quantum = quantum;
			temp->num = i;
			c_args[i] = temp;
			pthread_create(&consumer_thread[i],NULL,round_robin_C,(void*)temp);
  		}

  		for (int i=0;i<num_producers;i++) 
    		pthread_join(producer_thread[i],NULL);

		gettimeofday(&total_tv, NULL);
		double producer_end = total_tv.tv_sec+(total_tv.tv_usec/1000000.0);

  		for (int i=0;i<num_consumers;i++)
    		pthread_join(consumer_thread[i],NULL);

		gettimeofday(&total_tv, NULL);
		double total_end = total_tv.tv_sec+(total_tv.tv_usec/1000000.0);

		pthread_mutex_destroy(&permissions);
		pthread_cond_destroy(&cond);
		for (int i=0; i<num_producers; ++i) {
			delete p_args[i];
		}
		for (int i=0; i<num_consumers; ++i) {
			delete c_args[i];
		}

		double t_min = numeric_limits<double>::infinity(), t_max = 0, t_average = 0;
		for (int i=0; i<turn_around.size(); ++i) {
			if (turn_around.at(i) < t_min) {
				t_min = turn_around.at(i);
			}

			if (turn_around.at(i) > t_max) {
				t_max = turn_around.at(i);
			}

			t_average += turn_around.at(i);
		}
		t_average /= num_products;

		double w_min = numeric_limits<double>::infinity(), w_max = 0, w_average = 0;
		for (int i=0; i<wait.size(); ++i) {
			if (wait.at(i) < w_min) {
				w_min = wait.at(i);
			}

			if (wait.at(i) > w_max) {
				w_max = wait.at(i);
			}

			w_average += wait.at(i);
		}
		w_average /= num_products;

		cout << "Total processing time: " << total_end-total_start << "s" << endl;
		cout << "Producer throughput: " << (num_products/(producer_end-total_start))* 60 << "prods/m" << endl;
		cout << "Consumer throughput: " << (num_products/(total_end-consumer_start))* 60 << "prods/m" << endl;
		cout << "Turn-around Min: " << t_min << ", Max: " << t_max << ", Average: " << t_average << endl;
		cout << "Wait Min: " << w_min << ", Max: " << w_max << ", Average: " << w_average << endl;

		return 0;
	}

	fcfs_args* p_args[num_producers];
	fcfs_args* c_args[num_consumers];
	pthread_mutex_init(&permissions, NULL);
	pthread_cond_init(&cond, NULL);

	timeval total_tv;
	gettimeofday(&total_tv, NULL);
	double total_start = total_tv.tv_sec+(total_tv.tv_usec/1000000.0);

	for (int i=0;i<num_producers;i++){
		np[i] = i;
		fcfs_args* temp = new fcfs_args;
		temp->num = i;
		p_args[i] = temp;
		pthread_create(&producer_thread[i],NULL,first_come_first_serve_P,(void*)temp);
	}

	gettimeofday(&total_tv, NULL);
	double consumer_start = total_tv.tv_sec+(total_tv.tv_usec/1000000.0);

	for (int i=0;i<num_consumers;i++){
		nc[i] = i;
		fcfs_args* temp = new fcfs_args;
		temp->num = i;
		c_args[i] = temp;
		pthread_create(&consumer_thread[i],NULL,first_come_first_serve_C,(void*)temp);
	}

	for (int i=0;i<num_producers;i++) 
		pthread_join(producer_thread[i],NULL);

	gettimeofday(&total_tv, NULL);
	double producer_end = total_tv.tv_sec+(total_tv.tv_usec/1000000.0);

	for (int i=0;i<num_consumers;i++)
		pthread_join(consumer_thread[i],NULL);

	gettimeofday(&total_tv, NULL);
	double total_end = total_tv.tv_sec+(total_tv.tv_usec/1000000.0);

	pthread_mutex_destroy(&permissions);
	pthread_cond_destroy(&cond);
	for (int i=0; i<num_producers; ++i) {
		delete p_args[i];
	}
	for (int i=0; i<num_consumers; ++i) {
		delete c_args[i];
	}

	double t_min = numeric_limits<double>::infinity(), t_max = 0, t_average = 0;
	for (int i=0; i<turn_around.size(); ++i) {
		if (turn_around.at(i) < t_min) {
			t_min = turn_around.at(i);
		}

		if (turn_around.at(i) > t_max) {
			t_max = turn_around.at(i);
		}

		t_average += turn_around.at(i);
	}
	t_average /= num_products;

	double w_min = numeric_limits<double>::infinity(), w_max = 0, w_average = 0;
	for (int i=0; i<wait.size(); ++i) {
		if (wait.at(i) < w_min) {
			w_min = wait.at(i);
		}

		if (wait.at(i) > w_max) {
			w_max = wait.at(i);
		}

		w_average += wait.at(i);
	}
	w_average /= num_products;

	cout << "Total processing time: " << total_end-total_start << "s" << endl;
	cout << "Producer throughput: " << (num_producers/(producer_end-total_start))* 60 << "prods/m" << endl;
	cout << "Consumer throughput: " << (num_products/(total_end-consumer_start))* 60 << "prods/m" << endl;
	cout << "Turn-around Min: " << t_min << ", Max: " << t_max << ", Average: " << t_average << endl;
	cout << "Wait Min: " << w_min << ", Max: " << w_max << ", Average: " << w_average << endl;

	return 0;
}

