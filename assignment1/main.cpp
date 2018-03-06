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

using namespace std;

typedef struct {
	int id;
	timeval time;
	timeval wait;
	int life;
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

pthread_mutex_t permissions;
pthread_cond_t cond;

product* create_product() {
	product* new_product = new product;
	new_product->id = random();
	gettimeofday(&(new_product->time), NULL);
	new_product->t1 = (new_product->time).tv_sec+((new_product->time).tv_usec/1000000.0);
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
		Q.push(p);
		gettimeofday(&(p->wait), NULL);
		p->t3 = (p->wait).tv_sec+((p->wait).tv_usec/1000000.0);	
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
		gettimeofday(&(p->wait), NULL);
		double t4=(p->wait).tv_sec+((p->time).tv_usec/1000000.0);
		double t;
		t = t4 - (p->t3);
		cout << "Product " << p->id << " was waiting for " << t << " seconds." << endl;
		++prods_used;
		cout << "Consumer " << args->num << " has consumed product " << p->id << endl;
		gettimeofday(&(p->time), NULL);
		double t2=(p->time).tv_sec+((p->time).tv_usec/1000000.0);
		double t0;
		t0 = t2 - (p->t1);
		cout << "Product " << p->id << " was processed for " << t0 << " seconds." << endl;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&permissions);

		for (int i = 0; i<p->life; ++i) {
			fibbo(10);
		}

		delete_product(p);
		usleep(100000);
		pthread_mutex_lock(&permissions);
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
		Q.push(p);
		gettimeofday(&(p->wait), NULL);
		p->t3 = (p->wait).tv_sec+((p->wait).tv_usec/1000000.0);	
		cout << "Producer " << args->num << " has produced product " << p->id << endl;
		++prods_made;

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
		gettimeofday(&(p->wait), NULL);
		double t4=(p->wait).tv_sec+((p->time).tv_usec/1000000.0);
		double t;
		t = t4 - (p->t3);
		cout << "Product " << p->id << " was waiting for " << t << " seconds." << endl;
		pthread_cond_signal(&cond);
		if (p->life <= args->quantum) {
			cout << "Consumer " << args->num << " has consumed product " << p->id << endl;
			pthread_mutex_unlock(&permissions);
			for (int i = 0; i<p->life; ++i) {
				fibbo(10);
			}
			++prods_used;
			cout << "Consumer " << args->num << " has consumed product " << p->id << endl;
			gettimeofday(&(p->time), NULL);
			double t2=(p->time).tv_sec+((p->time).tv_usec/1000000.0);
			double t0;
			t0 = t2 - (p->t1);
			cout << "Product " << p->id << " was processed for " << t0 << " seconds." << endl;
			delete_product(p);
			usleep(100000);
			pthread_mutex_lock(&permissions);
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
		for (int i=0;i<num_producers;i++){
			np[i] = i;
			rr_args* temp = new rr_args;
			temp->quantum = quantum;
			temp->num = i;
			p_args[i] = temp;
			pthread_create(&producer_thread[i],NULL,round_robin_P,(void*)temp);
  		}

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

  		for (int i=0;i<num_consumers;i++)
    		pthread_join(consumer_thread[i],NULL);

		pthread_mutex_destroy(&permissions);
		pthread_cond_destroy(&cond);
		for (int i=0; i<num_producers; ++i) {
			delete p_args[i];
		}
		for (int i=0; i<num_consumers; ++i) {
			delete c_args[i];
		}
		return 0;
	}

	fcfs_args* p_args[num_producers];
	fcfs_args* c_args[num_consumers];
	pthread_mutex_init(&permissions, NULL);
	pthread_cond_init(&cond, NULL);
	for (int i=0;i<num_producers;i++){
		np[i] = i;
		fcfs_args* temp = new fcfs_args;
		temp->num = i;
		p_args[i] = temp;
		pthread_create(&producer_thread[i],NULL,first_come_first_serve_P,(void*)temp);
	}

	for (int i=0;i<num_consumers;i++){
		nc[i] = i;
		fcfs_args* temp = new fcfs_args;
		temp->num = i;
		c_args[i] = temp;
		pthread_create(&consumer_thread[i],NULL,first_come_first_serve_C,(void*)temp);
	}

	for (int i=0;i<num_producers;i++) 
		pthread_join(producer_thread[i],NULL);

	for (int i=0;i<num_consumers;i++)
		pthread_join(consumer_thread[i],NULL);

	pthread_mutex_destroy(&permissions);
	pthread_cond_destroy(&cond);
	for (int i=0; i<num_producers; ++i) {
		delete p_args[i];
	}
	for (int i=0; i<num_consumers; ++i) {
		delete c_args[i];
	}
	return 0;
}

