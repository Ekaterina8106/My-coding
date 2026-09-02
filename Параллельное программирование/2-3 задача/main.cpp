#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

pthread_mutex_t* forks; // вилки
pthread_mutex_t mutex;  // мьютекс для синхронизации вывода
int philosophers_count = 0; // количсетво философов
int food_per_philosopher = 0;  // количество кушанья каждого философа

void* philosopher(void* arg) {
    int num = *(int*)arg;

    int left_fork = num;
    int right_fork = (num + 1) % philosophers_count;

    int count = 0;  // сколько раз поели

    while (count < food_per_philosopher) {
        // Философ думает
        pthread_mutex_lock(&mutex);
        printf("Философ %d размышляет...\n", num);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2 + 1); // тоже чтобы сразу не поел подряд...

        // решение проблемы. последний философ берет праую, левую
        if (num == philosophers_count - 1) {
            pthread_mutex_lock(&forks[right_fork]); //правую вилку взяли

            pthread_mutex_lock(&mutex);
            printf("Философ %d (последний) взял ПРАВУЮ вилку %d\n", num, right_fork);
            pthread_mutex_unlock(&mutex);

            sleep(1); //

            pthread_mutex_lock(&forks[left_fork]); // левую если смогли взяли

            pthread_mutex_lock(&mutex);
            printf("Философ %d взял ЛЕВУЮ вилку %d\n", num, left_fork);
            pthread_mutex_unlock(&mutex);
        }
        else {
            // Остальные: сначала левую, потом правую
            //*/
            pthread_mutex_lock(&forks[left_fork]);

            pthread_mutex_lock(&mutex);
            printf("Философ %d взял ЛЕВУЮ вилку %d\n", num, left_fork);
            pthread_mutex_unlock(&mutex);

            sleep(1); // чтобы раньше времени не выхватили вилки, дедлока мб не быть

            pthread_mutex_lock(&forks[right_fork]);

            pthread_mutex_lock(&mutex);
            printf("Философ %d взял ПРАВУЮ вилку %d\n", num, right_fork);
            pthread_mutex_unlock(&mutex);
        } //

        // Философ ест
        count++;
        pthread_mutex_lock(&mutex);
        printf("Философ %d ЕСТ с вилками %d и %d (Обед %d/%d)\n",
               num, left_fork, right_fork, count, food_per_philosopher);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2 + 1); // чтобы не поел сразу второй раз/// no

        // Философ кладет вилки
        pthread_mutex_lock(&mutex);
        printf("Философ %d положил вилки %d и %d\n", num, left_fork, right_fork);
        pthread_mutex_unlock(&mutex);

        if (num == philosophers_count - 1) {
            pthread_mutex_unlock(&forks[left_fork]);
            pthread_mutex_unlock(&forks[right_fork]);
        } else {
            pthread_mutex_unlock(&forks[right_fork]);
            pthread_mutex_unlock(&forks[left_fork]);
        }
    }

    pthread_mutex_lock(&mutex);
    printf("Философ %d закончил все %d обедов!\n", num, count);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    srand(time(NULL));

    cout << "=== Встретились как то раз философы... ===" << endl;

    cout << "Введите количество философов: ";
    cin >> philosophers_count;

    if (philosophers_count < 2) {
        cout << "Нужно минимум 2 философа!" << endl;
        return 1;
    }

    cout << "Введите количество обедов для каждого философа: ";
    cin >> food_per_philosopher;

    if (food_per_philosopher <= 0) {
        cout << "Нужен минимум 1 обед!" << endl;
        return 1;
    }

    // Инициализируем мьютекс для вывода
    pthread_mutex_init(&mutex, NULL);

    // Создаем вилки (мьютексы)
    forks = new pthread_mutex_t[philosophers_count];
    for (int i = 0; i < philosophers_count; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    pthread_t philosophers[philosophers_count];
    int nums[philosophers_count];

    cout << "=== НАЧИНАЕМ ОБЕД ===" << endl;
    cout << "Каждый философ должен съесть" <<food_per_philosopher<< "обедов" << endl;

    for (int i = 0; i < philosophers_count; i++) {
        nums[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &nums[i]);
    }

    for (int i = 0; i < philosophers_count; i++) {
        pthread_join(philosophers[i], NULL);
    }

    cout << "=== ВСЕ ФИЛОСОФЫ УСПЕШНО ЗАВЕРШИЛИ ОБЕД БЕЗ БЛОКИРОВОК ===" << endl;

    for (int i = 0; i < philosophers_count; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    pthread_mutex_destroy(&mutex);

    delete[] forks;

    return 0;
}
