#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <ctime>

using namespace std;

sem_t* forks;
int num_philosophers = 0;
int meals_per_philosopher = 0;

void* philosopher(void* arg) {
    int id = *(int*) arg;
    int left_fork = id;
    int right_fork = (id + 1) % num_philosophers;

    int meals_eaten = 0;

    while (meals_eaten < meals_per_philosopher) { 
        printf("Philosopher %d is thinking...\n", id);
        sleep(rand() % 3 + 1);

        if (id == num_philosophers - 1) {
            sem_wait(&forks[right_fork]);
            printf("Philosopher %d took RIGHT fork %d\n", id, right_fork);

            sem_wait(&forks[left_fork]);
            printf("Philosopher %d took LEFT fork %d\n", id, left_fork);
        } else {
            sem_wait(&forks[left_fork]);
            printf("Philosopher %d took LEFT fork %d\n", id, left_fork);

            sem_wait(&forks[right_fork]);
            printf("Philosopher %d took RIGHT fork %d\n", id, right_fork);
        }

        meals_eaten++;
        printf("Philosopher %d is eating using forks %d and %d (Meal %d / %d)\n", 
                id, left_fork, right_fork, meals_eaten, meals_per_philosopher);
        sleep(rand() % 2 + 1);

        printf("Philosopher %d put down forks %d and %d\n", id, left_fork, right_fork);
        sem_post(&forks[left_fork]);
        sem_post(&forks[right_fork]);
    }

    printf("Philosopher %d finished all %d meals!\n", id, meals_eaten);

    return nullptr;
}

int main() {
    srand(time(nullptr));

    cout << "=== DINING PHILOSOPHERS ===" << endl;

    cout << "Enter number of philosophers: ";
    cin >> num_philosophers;

    if (num_philosophers < 2) {
        cout << "Need at least 2 philosophers!" << endl;
        return 1;
    }

    cout << "Enter numbers of meals per philosopher: ";
    cin >> meals_per_philosopher;

    if (meals_per_philosopher <= 0) {
        cout << "Need at least 1 meal per philosopher!" << endl;
        return 1;
    }

    forks = new sem_t[num_philosophers];

    for (int i = 0; i < num_philosophers; i++) {
        sem_init(&forks[i], 0, 1);
    }

    pthread_t philosophers[num_philosophers];
    int ids[num_philosophers];

    cout << "=== STARTING DINNER... ===" << endl;

    for (int i = 0; i < num_philosophers; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], nullptr, philosopher, &ids[i]);
    }

    for (int i = 0; i < num_philosophers; i++) {
        pthread_join(philosophers[i], nullptr);
    }

    cout << "=== DINNER FINISHED ===" << endl;

    for (int i = 0; i < num_philosophers; i++) {
        sem_destroy(&forks[i]);
    }

    delete[] forks;

    return 0;
}