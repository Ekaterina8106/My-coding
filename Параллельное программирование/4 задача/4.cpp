#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int N; // философы
pthread_mutex_t* forks;
pthread_mutex_t mutex;

int* hunger; // массив голода для каждого философа
int gluttons_eating = 0; // сколько обжор ест сейчас

bool is_glutton(int num) {
    return (num % 2 == 1); // нечетные - обжоры
}

void* philosopher(void* arg) {
    int num = *(int*)arg;
    int left_fork = num;
    int right_fork = (num + 1) % N;

    hunger[num] = 0; // массив голода

    while (true) {
        sleep(rand() % 2 + 1);

        if (is_glutton(num)) { // обжора
            pthread_mutex_lock(&forks[right_fork]);

            if (pthread_mutex_trylock(&forks[left_fork]) == 0) { // если взял - ест
                pthread_mutex_lock(&mutex);
                gluttons_eating++;  // увеличиваем счётчик едящих обжор
                pthread_mutex_unlock(&mutex);
                printf("Обжора %d ест (кушает обжор = %d)\n", num, gluttons_eating);

                sleep(rand() % 2 + 1);
                // добавила 1 строчку:
                hunger[num] = 0;

                while (true) { // если другой обжора тоже ест
                    pthread_mutex_lock(&mutex);
                    if (gluttons_eating > 1) { // На этот фрагмент кода Туз сказал что это же как sem_post, sem_wait. Так что имеет смысл сделать через семафор, но мне лень, зачем семафор если тут переменные
                        gluttons_eating--;
                        pthread_mutex_unlock(&mutex);
                        break;
                    }
                    pthread_mutex_unlock(&mutex);

                    // Небольшая задержка перед следующей проверкой
                    sleep(1);
                }

                //printf("Обжор сейчас ест: %d \n", gluttons_eating);
                pthread_mutex_unlock(&forks[left_fork]);
                pthread_mutex_unlock(&forks[right_fork]);
            }
            else { // если не смог взять вилку вторую
                pthread_mutex_unlock(&forks[right_fork]);
                // добавила 1 строчку
                hunger[num]++;
            }
        }

        else { // философ
            pthread_mutex_lock(&forks[left_fork]);

            if (pthread_mutex_trylock(&forks[right_fork]) == 0) {
                printf("Философ %d ест (кушает обжор = %d)\n", num, gluttons_eating);

                sleep(rand() % 2 + 1);

                pthread_mutex_lock(&mutex);
                hunger[num] = 0; // обнулили голод
                pthread_mutex_unlock(&mutex);

                pthread_mutex_unlock(&forks[right_fork]);
                pthread_mutex_unlock(&forks[left_fork]); //закончили есть
                //printf("Обжор сейчас ест: %d \n", gluttons_eating);
            }
            else { // не смог поесть :(
                pthread_mutex_lock(&mutex);
                hunger[num]++;

                if (hunger[num] > 50) {
                    printf("Философ %d ушел разочарованным!\n", num);
                    pthread_mutex_unlock(&mutex);
                    pthread_mutex_unlock(&forks[left_fork]);
                    return NULL;
                }
                pthread_mutex_unlock(&mutex);
                pthread_mutex_unlock(&forks[left_fork]);
            }
        }
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    cout << "=== Встретились как то раз философы... и обжоры ===" << endl;

    cout << "Введите количество философов: ";
    cin >> N;

    if (N < 2) {
        cout << "Нужно минимум 2 философа!" << endl;
        return 1;
    }

    pthread_mutex_init(&mutex, NULL);

    forks = new pthread_mutex_t[N];
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    hunger = new int[N];
    for (int i = 0; i < N; i++) {
        hunger[i] = 0;
    }

    // Вот это изменилось (вроде)
    pthread_t* philosophers = new pthread_t [N];
    int* nums = new int [N];

    for (int i = 0; i < N; i++) {
        nums[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &nums[i]);
    }

    while (true) {
        sleep(5);

        pthread_mutex_lock(&mutex);
        cout << "Голод:";
        for (int i = 0; i < N; i++) {
            cout << " " << hunger[i];
        }
        cout << endl;
        pthread_mutex_unlock(&mutex);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    pthread_mutex_destroy(&mutex);

    delete[] forks;
    delete[] hunger;

    return 0;
}
