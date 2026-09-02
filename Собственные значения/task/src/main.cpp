#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../inc/matrix.h"
#include "../inc/eigenvalues.h"
//#include <fenv.h>

int main(int argc, char* argv[]) {
    //feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

    if (argc < 5) {
        printf("Использование: %s n m epsilon k [filename]\n", argv[0]);
        return 1;
    }

    // парсинг аргументов, не сложно заметить что это копия предыдущего маина
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    double epsilon = atof(argv[3]); // преобразует строку в число типа double !!!
    int k = atoi(argv[4]);
    char* filename = NULL;

    if (k == 0 && argc < 6) {
        printf("И где файл?\n");
        return 1;
    }
    if (k == 0) {
        filename = argv[5];
    }

    // Выделение памяти, как и в прошлый раз
    double** A = allocate_matrix(n, n);
    double* eigenvalues = (double*)malloc(n * sizeof(double));
    //double* work = (double*)malloc(n * sizeof(double)); // Вспомогательный вектор

    // Инициализация матрицы
    init_matrix(n, A, k, filename);

    printf("Исходная матрица (первые %dx%d элементов):\n", m, m);
    print_matrix(n, n, A, m);
    printf("\n");

    // теперь оно тут
    double trace_A = 0.0;
    double norm_A_sq = 0.0;

    for (int i = 0; i < n; i++) {
        trace_A += A[i][i];
        for (int j = 0; j < n; j++) {
            norm_A_sq += A[i][j] * A[i][j];
        }
    }
    double norm_A = sqrt(norm_A_sq);

    // Измерение времени приведения к почти треугольному виду
    clock_t start = clock();
    pochti_treyg(n, A);
    clock_t end = clock();
    double t1 = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Матрица после приведения к почти треугольному виду (первые %dx%d элементов):\n", m, m);
    print_matrix(n, n, A, m);
    printf("\n");

    int max_iterations;
    if (n < 100) {
        max_iterations = 5000;
    } else if (n <= 500) {
        max_iterations = 10000;
    } else {
        max_iterations = 500;  // Меньше итераций для очень больших матриц
    }
    
    // Измерение времени LR-алгоритма
    start = clock();
    int iterations = lr_algorithm(n, A, eigenvalues, epsilon, max_iterations);
    end = clock();
    double t2 = (double)(end - start) / CLOCKS_PER_SEC;

    // Вычисление невязок
    double residual1, residual2;
    compute_residuals(n, trace_A, norm_A, eigenvalues, &residual1, &residual2);

    // Вывод результатов
    printf("Найденные собственные значения:\n");
    for (int i = 0; i < n && i < m; i++) {
        printf(" %10.3e", eigenvalues[i]);
    }
    printf("\n\n");

    printf("%s : Res1 = %e Res2 = %e Iterations = %d Iterations1 = %d T1 = %.2f T2 = %.2f\n",
           argv[0], residual1, residual2, iterations, iterations / n, t1, t2);

    // Освобождение памяти
    free_matrix(A, n);
    free(eigenvalues);

    return 0;
}
