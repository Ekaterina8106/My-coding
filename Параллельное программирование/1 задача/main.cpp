#include <iostream>
#include <pthread.h> // для потоков
#include <semaphore.h> // для семафоров
#include <unistd.h> // для sleep
#include <ctime>

using namespace std;

sem_t bread; // сколько булок есть (всего)
sem_t space; // свободное место на полке
sem_t mutex; // будет помогать с корректной работой программы

int mesto; // сколько места на полке всего
int bread_now = 0; // хлеба сейчас на полке
int already_baked = 0; // уже сделано хлеба
int stop; // количество хлеба которые должны испечь, после стоп

void* baker (void* arg){
    int num = *(int*)arg; // номера потока

    while (already_baked < stop) {
        sleep(rand() % 2 + 1);

        sem_wait(&space); // ждем пока овободилось место

        sem_wait(&mutex); // кладем булку

        if (already_baked >= stop) { // чтобы случайно 2 покупателя не увидели булку и не схватили ее
            sem_post(&mutex);
            sem_post(&space); // освобождаем место обратно
            break;
        }
        bread_now ++;
        already_baked ++;

        printf (" Булочник %d положил булку на полку: %d / %d. Всего испекли: %d / %d \n", num, bread_now, mesto, already_baked, stop);

        sem_post (&mutex); // отработали с потоком
        sem_post (&bread); // увеличиваем счетчик булок на 1.
    }

    sem_wait (&mutex);
    printf (" Булочник %d завершил выпекать булочки \n", num);
    sem_post (&mutex);

    return NULL;
}

void* customer (void* arg){
    int num = *(int*)arg;
    int buy = 0; // сколько купит

    while (true){
        sem_wait(&mutex);
        if (already_baked >= stop && bread_now == 0) {
            sem_post(&mutex);
            break; // все испечено, все куплено
        }
        sem_post(&mutex);

        sleep(rand() % 2 + 1);

        sem_wait(&bread); // ждем булочку
        sem_wait(&mutex);
        if (bread_now > 0) {
            bread_now --;
            buy ++;
            printf ("Покупатель %d купил булку. Осталось: %d / %d. Купил уже: %d / %d \n", num, bread_now, mesto, buy, stop);
        }
        sem_post(&mutex);
        sem_post(&space);
    }

    sem_wait(&mutex);
    printf("Покупатель %d ушел, купил %d булок.\n", num, buy);
    sem_post(&mutex);

    return NULL;
}

int main (){
    srand(time(NULL)); //счетчик случайных чисел

    cout << "Размер полки: ";
    cin >> mesto;

    int col_bakers;
    int col_customers;
    cout << "Количество пекарей: ";
    cin >> col_bakers;
    cout << "Количество покупателей: ";
    cin >> col_customers;
    cout << "Сколько всего испечь булок: ";
    cin >> stop;

    sem_init(&bread, 0, 0);
    sem_init (&space, 0, mesto);
    sem_init (&mutex, 0, 1);

    pthread_t* bakers = new pthread_t [col_bakers];
    pthread_t* customers = new pthread_t [col_customers]; // создаем потоки

    int* baker_num = new int[col_bakers];
    int* customer_num = new int[col_customers];

    cout << "\n=== ПЕКАРНЯ ОТКРЫЛАСЬ ===\n" << endl;
    cout << "Всего нужно испечь: " << stop << " булок" << endl;

    for (int i = 0; i < col_bakers; i++) { // поток пекарей
        baker_num[i] = i + 1;
        pthread_create(&bakers[i], NULL, baker, &(baker_num[i]));
    }

    for (int i = 0; i < col_customers; i++) { // поток покупателей
        customer_num[i] = i + 1;
        pthread_create(&customers[i], NULL, customer, &customer_num[i]);
    }

    // Ожидание завершения булочников
    for (int i = 0; i < col_bakers; i++) {
        pthread_join(bakers[i], NULL); // чтобы main ждал пока поток завершит данную работу
    }

    for (int i = 0; i < col_customers; i++) {
        sem_post(&bread); // разбудить покупателей
    }

    // Даем время покупателям забрать оставшиеся булки
    sleep(rand() % 5 + 1);

    // Ожидание завершения покупателей
    for (int i = 0; i < col_customers; i++) {
        pthread_join(customers[i], NULL);
    }

    // Уничтожение семафоров
    sem_destroy(&bread);
    sem_destroy(&space);
    sem_destroy(&mutex);

    cout << "\n=== ПЕКАРНЯ ЗАКРЫЛАСЬ ===" << endl;
    cout << "Всего испекли: " << already_baked << " булок" << endl;

    delete[] bakers;
    delete[] customers;
    delete[] baker_num;
    delete[] customer_num;

    return 0;
}




