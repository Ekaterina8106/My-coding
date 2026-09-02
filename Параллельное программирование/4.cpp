#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <mutex>
#include <unistd.h>
#include <ctime>
#define n 5

void *thread_func(void *arg);
void print_st(int *sticks);

void print_st(int *sticks){
	for(int i = 0; i < n; i++){
		std::cout << sticks[i] << " ";
	}
	std::cout << "\n";
}

sem_t semc;
std::mutex mpr;
std::mutex mg;
sem_t semg;
std::mutex *mutex = new std::mutex[n];

void *thread_func(void *arg){
	int local_id = *((int *)arg);
	int *sticks = ((int *)arg) + 1;
	int *hungry = ((int *)arg) + n + 1;
	int *gluttons = ((int *)arg) + n + n + 1;
	int first = (local_id + (local_id % 2)) % n;
	int second = (local_id + ((local_id + 1) % 2)) % n;
	sem_post(&semc);

	if ((local_id % 2) == 0){
		while(true){
			mutex[first].lock();
                        mpr.lock();
                        sticks[first] = 0;
                        mpr.unlock();
			while ((hungry[local_id] % 4) < 3){
				if(mutex[second].try_lock()){

					mpr.lock();
					sticks[second] = 0;
					std::cout << local_id << " ate " << "hungry: " << hungry[local_id] << ", gluttons = "<< *gluttons << "\n";
					print_st(sticks);
					sticks[first] = 1;
					sticks[second] = 1;
					mpr.unlock();

					mutex[first].unlock();
					mutex[second].unlock();
					mpr.lock();
					hungry[local_id] = 0;
					mpr.unlock();
					sleep(rand() % 3);
					break;
				}
				else{
					mpr.lock();
					hungry[local_id]++;
					mpr.unlock();
					sleep(rand() % 3);
				}
			}
			if ((hungry[local_id] % 4) == 3){
				mpr.lock();
				hungry[local_id]++;
				sticks[first] = 1;
				mpr.unlock();
				mutex[first].unlock();
				sleep(rand() % 3);
			}
			if(hungry[local_id] > 100){
				mpr.lock();
				sticks[first] = 1;
				std::cout << local_id << "leaves";
				mpr.unlock();
				mutex[first].unlock();
				return NULL;
			}
		}
	}
	else{
		while(true){

			mutex[first].lock();
			mpr.lock();
                        sticks[first] = 0;
                        mpr.unlock();

			while(true){
                                if(mutex[second].try_lock()){

                                        mpr.lock();
                                        sticks[second] = 0;
                                        std::cout << local_id << " ate " << "hungry: " << hungry[local_id] << ", gluttons = "<< *gluttons << "\n";
                                        print_st(sticks);
                                        hungry[local_id] = 0;
					mpr.unlock();

                                        break;
                                }
                                else{
					mpr.lock();
					hungry[local_id]++;
					std::cout << "why does he not eating?\n";
					mpr.unlock();
                                }
				if(hungry[local_id] > 100){
                                	mpr.lock();
                                	std::cout << local_id << "leaves";
                                	mpr.unlock();
					return NULL;
                        	}
                        }

			if((mg.try_lock()) == false){
				sem_post(&semg);
			}
			mpr.lock();
			(*gluttons)++;
			mpr.unlock();
			sleep(rand() % 2);

			sem_wait(&semg);

			mpr.lock();
			(*gluttons)--;
			std::cout << local_id << " ate, " << "gluttons = " << *gluttons << "\n";
			print_st(sticks);
                        sticks[first] = 1;
                        sticks[second] = 1;
			mpr.unlock();

			mutex[first].unlock();
                        mutex[second].unlock();

			sleep(rand() % 3);
		}
	}
	return NULL;
}


int main(){
	int result = 0;
	pthread_t threadm[n];
	int *id_sticks = new int [n + n + 2];
	sem_init(&semc, 0, 0);
	sem_init(&semg, 0, 0);
	for(int i = 0; i < n; i++){
		id_sticks[i + 1] = 1;
		id_sticks[n + i + 1] = 0;
	}
	id_sticks[n + n + 1] = 0;
	std::srand(std::time(nullptr));

	for(id_sticks[0] = 0; id_sticks[0] < n; id_sticks[0]++){
		result = pthread_create(
			&threadm[id_sticks[0]],
			nullptr,
			thread_func,
			id_sticks
		);
		if(result != 0){
			std::cout << "Creating the " << id_sticks[0] << "thread failed\n";
			return 1;
		}
		sem_wait(&semc);
	}
	while(1){
		sleep(5);
		mpr.lock();
		std::cout << "hungries";
		for(int i = 0; i < n; i++){
			std::cout << " " << id_sticks[n + i + 1];
		}
		std::cout << "\n";
		mpr.unlock();
	}
	for(int i = 0; i < n; i++){
		result = pthread_join(threadm[i], nullptr);
		if(result != 0){
			std::cout << "Joining the " << i << "thread failed\n";
			return 1;
		}
	}
	delete[] id_sticks;
	return 0;
}
