#include <stdio.h>      // printf, scanf
#include <pthread.h>    // pthread_*
#include <unistd.h>     // sleep
#include <stdlib.h>     // rand, srand, malloc, free
#include <time.h>       // time

int N;                              // количество философов
pthread_mutex_t *forks;             // массив мьютексов-вилок
pthread_mutex_t mutex;              // общий мьютекс

int *hunger;                        // массив голода философов
int gluttons_eating = 0;            // сколько обжор ест одновременно

// Нечётные номера — обжоры
int is_glutton(int num) {
    return num % 2 == 1;
}

// Функция потока философа/обжоры
void* philosopher(void* arg) {
    int num = *(int*)arg;            // номер философа
    int left_fork = num;             // левая вилка
    int right_fork = (num + 1) % N;  // правая вилка (по кругу)

    hunger[num] = 0;                 // начальный голод = 0

    while (1) {
        sleep(rand() % 2 + 1);       // думает/ждёт

        if (is_glutton(num)) {
            pthread_mutex_lock(&forks[right_fork]);

            if (pthread_mutex_trylock(&forks[left_fork]) == 0) {
                pthread_mutex_lock(&mutex);
                gluttons_eating++;
                pthread_mutex_unlock(&mutex);

                printf("Обжора %d ест (обжор сейчас = %d)\n", num, gluttons_eating);

                sleep(rand() % 2 + 1);
                hunger[num] = 0;

                while (1) {
                    pthread_mutex_lock(&mutex);
                    if (gluttons_eating > 3) {
                        gluttons_eating--;
                        pthread_mutex_unlock(&mutex);
                        break;
                    }
                    pthread_mutex_unlock(&mutex);
                    sleep(1);
                }

                pthread_mutex_unlock(&forks[left_fork]);
                pthread_mutex_unlock(&forks[right_fork]);
            }
            else {
                pthread_mutex_unlock(&forks[right_fork]);
                hunger[num]++;
            }

        }
        else {
            pthread_mutex_lock(&forks[left_fork]);

            if (pthread_mutex_trylock(&forks[right_fork]) == 0) {
                printf("Философ %d ест (обжор сейчас = %d)\n", num, gluttons_eating);

                sleep(rand() % 2 + 1);

                pthread_mutex_lock(&mutex);
                hunger[num] = 0;
                pthread_mutex_unlock(&mutex);

                pthread_mutex_unlock(&forks[right_fork]);
                pthread_mutex_unlock(&forks[left_fork]);
            }
            else {
                pthread_mutex_lock(&mutex);
                hunger[num]++;
                pthread_mutex_unlock(&mutex);

                pthread_mutex_unlock(&forks[left_fork]);
            }
        }
    }
    return NULL;
}

int main() {
    srand(time(NULL));   // инициализация ГПСЧ

    printf("Введите количество философов: ");
    scanf("%d", &N);
    if (N < 2) {
        printf("Нужно минимум 2 философа!\n");
        return 1;
    }

    pthread_mutex_init(&mutex, NULL);   // общий мьютекс

    forks = new pthread_mutex_t[N];
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    hunger = new int[N];
    for (int i = 0; i < N; i++) {
        hunger[i] = 0;
    }

    pthread_t* philosophers = new pthread_t [N];
    int* nums = new int [N];

    for (int i = 0; i < N; i++) {
        nums[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &nums[i]);
    }

    while (1) {
        sleep(5);

        pthread_mutex_lock(&mutex);
        printf("Голод:");
        for (int i = 0; i < N; i++) {
            printf(" %d", hunger[i]);
        }
        printf("\n");
        pthread_mutex_unlock(&mutex);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    pthread_mutex_destroy(&mutex);

    free(forks);
    free(hunger);
    free(philosophers);
    free(nums);

    return 0;
}

