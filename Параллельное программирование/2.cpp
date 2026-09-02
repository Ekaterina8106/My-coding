#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

sem_t* forks;
int num_philosophers = 0;

void* philosopher_deadlock(void* arg) {
    int id = *(int*) arg;
    int left_fork = id;
    int right_fork = (id + 1) % num_philosophers;

    while (true) {
        printf("Philosopher %d is thinking...\n", id);
        sleep(2);
        
        sem_wait(&forks[left_fork]);
        printf("Philosopher %d took left fork %d\n", id, left_fork);

        sleep(2);

        sem_wait(&forks[right_fork]);
        printf("Philosopher %d took right fork %d\n", id, right_fork);

        printf("Philosopher %d is EATING with forks %d and %d\n", id, left_fork, right_fork);
        sleep(2);

        sem_post(&forks[left_fork]);
        sem_post(&forks[right_fork]);
        printf("Philosopher %d put down forks %d and %d\n", id, left_fork, right_fork);
    }

    return nullptr;
}

int main() {

    cout << "=== DINING PHILOSOPHERS DEADLOCK ===" << endl;
    cout << "Enter number of philosophers: ";
    cin >> num_philosophers;
    cout << endl;

    if (num_philosophers < 2) {
        cout << "Need at least 2 philosophers!" << endl;
        return 1;
    }

    forks = new sem_t[num_philosophers];

    for (int i = 0; i < num_philosophers; i++) {
        sem_init(&forks[i], 0, 1);
    }

    pthread_t philosophers[num_philosophers];
    int ids[num_philosophers];

    for (int i = 0; i < num_philosophers; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], nullptr, philosopher_deadlock, &ids[i]);
    }

    /*for (int i = 0; i < num_philosophers; i++) {
        pthread_join(philosophers[i], nullptr);
    } */

    sleep(15);

    cout << "=== DEADLOCK LIKELY OCCURRED ===" << endl;

    for (int i = 0; i < num_philosophers; i++) {
        sem_destroy(&forks[i]);
    }

    delete[] forks;

    return 0;
}