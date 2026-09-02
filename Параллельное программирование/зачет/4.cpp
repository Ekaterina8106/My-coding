#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int N; // количество философов
pthread_mutex_t* forks; // вилки (мьютексы)
pthread_mutex_t mutex; // мьютекс для общих данных
sem_t min_gluttons_sem; // семафор для минимального количества обжор

int* hunger; // массив голода для каждого философа
int gluttons_eating = 0; // сколько обжор ест сейчас
int total_gluttons = 0; // всего обжор

// Проверка, является ли философ обжорой
bool is_glutton(int num) {
    // Обжоры - нечётные, начиная с 1 (1, 3, 5, ...)
    return (num % 2 == 1);
}

void* philosopher(void* arg) {
    int num = *(int*)arg;
    int left_fork = num;
    int right_fork = (num + 1) % N;

    while (true) {
        sleep(rand() % 3 + 1); // думаем

        if (is_glutton(num)) { // если обжора
            // Ждём, пока не наберётся минимум 3 обжоры для еды
            sem_wait(&min_gluttons_sem);

            // Берем вилки в определенном порядке для избежания deadlock
            if (num % 2 == 0) {
                pthread_mutex_lock(&forks[left_fork]);
                pthread_mutex_lock(&forks[right_fork]);
            } else {
                pthread_mutex_lock(&forks[right_fork]);
                pthread_mutex_lock(&forks[left_fork]);
            }

            // Начинаем есть
            pthread_mutex_lock(&mutex);
            gluttons_eating++;
            printf("Обжора %d ест (кушает обжор = %d)\n", num, gluttons_eating);
            pthread_mutex_unlock(&mutex);

            // Едим дольше, чем философы
            sleep(rand() % 3 + 2);

            // Заканчиваем есть
            pthread_mutex_lock(&mutex);
            gluttons_eating--;
            pthread_mutex_unlock(&mutex);

            // Освобождаем вилки
            pthread_mutex_unlock(&forks[left_fork]);
            pthread_mutex_unlock(&forks[right_fork]);

            // Освобождаем место для следующего обжоры
            sem_post(&min_gluttons_sem);
        }
        else { // если философ
            // Пытаемся взять вилки
            pthread_mutex_lock(&forks[left_fork]);

            if (pthread_mutex_trylock(&forks[right_fork]) == 0) {
                printf("Философ %d ест (кушает обжор = %d)\n", num, gluttons_eating);
                sleep(rand() % 2 + 1); // едим

                pthread_mutex_lock(&mutex);
                hunger[num] = 0; // сбрасываем голод
                pthread_mutex_unlock(&mutex);

                pthread_mutex_unlock(&forks[right_fork]);
                pthread_mutex_unlock(&forks[left_fork]);
            }
            else { // не смогли взять вторую вилку
                pthread_mutex_lock(&mutex);
                hunger[num]++; // увеличиваем голод

                if (hunger[num] > 30) {
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

    cout << "=== Встретились как-то раз философы... и обжоры ===" << endl;

    cout << "Введите количество философов (не менее 6): ";
    cin >> N;

    if (N < 6) {
        cout << "Нужно минимум 6 философов!" << endl;
        return 1;
    }

    // Подсчитываем количество обжор
    total_gluttons = 0;
    for (int i = 0; i < N; i++) {
        if (is_glutton(i)) total_gluttons++;
    }

    if (total_gluttons < 3) {
        cout << "При таком количестве философов обжор меньше 3!" << endl;
        cout << "Обжоры располагаются на нечётных позициях (1, 3, 5, ...)" << endl;
        return 1;
    }

    cout << "Всего философов: " << N << endl;
    cout << "Из них обжор: " << total_gluttons << endl;
    cout << "Обжоры сидят через одного на позициях: ";
    for (int i = 0; i < N; i++) {
        if (is_glutton(i)) cout << i << " ";
    }
    cout << endl << "==============================================" << endl;

    // Инициализация мьютекса
    pthread_mutex_init(&mutex, NULL);

    // Инициализация семафора: значение = total_gluttons - 2
    // Это гарантирует, что всегда будет есть минимум 3 обжоры
    // (если total_gluttons >= 3)
    sem_init(&min_gluttons_sem, 0, total_gluttons - 2);

    // Выделение памяти и инициализация вилок
    forks = new pthread_mutex_t[N];
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Выделение памяти и инициализация массива голода
    hunger = new int[N];
    for (int i = 0; i < N; i++) {
        hunger[i] = 0;
    }

    // Создание потоков
    pthread_t* philosophers = new pthread_t[N];
    int* nums = new int[N];

    for (int i = 0; i < N; i++) {
        nums[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &nums[i]);
    }

    // Основной цикл вывода информации
    int iterations = 0;
    while (iterations < 20) { // Ограничим количество итераций для демонстрации
        sleep(3);

        pthread_mutex_lock(&mutex);
        cout << "\n=== Состояние на " << (iterations + 1) * 3 << " секунде ===" << endl;
        cout << "Голод философов:";
        for (int i = 0; i < N; i++) {
            cout << " Ф" << i << ":" << hunger[i];
        }
        cout << endl;
        cout << "Едящих обжор сейчас: " << gluttons_eating << endl;

        // Проверяем условие минимального количества обжор
        if (gluttons_eating < 3) {
            cout << "ВНИМАНИЕ: Ест меньше 3 обжор!" << endl;
        } else {
            cout << "Условие выполняется: ест минимум 3 обжоры" << endl;
        }
        pthread_mutex_unlock(&mutex);

        iterations++;
    }

    cout << "\n=== Завершение работы ===" << endl;

    // Даем потокам время завершиться
    sleep(2);

    // Уничтожение мьютексов и семафора
    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&min_gluttons_sem);

    // Освобождение памяти
    delete[] forks;
    delete[] hunger;
    delete[] philosophers;
    delete[] nums;

    return 0;
}
