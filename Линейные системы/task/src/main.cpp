#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../inc/matrix.h"
#include "../inc/inverse_gauss.h"

// функция для вывода матрицы, именно первые r строк
void print_matrix(int n, int m, double** A, int r) {
    int rows = n < r ? n : r; //стр
    int cols = m < r ? m : r; //стб

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf(" %10.3e", A[i][j]);
        }
        printf("\n");
    }
}

// вычисление нормы матрицы
double matrix_norm(int n, double** A) {
    double max = 0.0;
    for (int j = 0; j < n; j++) {
        double sum = 0.0;
        for (int i = 0; i < n; i++) {
            sum += fabs(A[i][j]);
        }
        if (sum > max) max = sum;
    }
    return max;
}

// умножение матриц, слава богу они одного размера
void matrix_multiply(int n, double** A, double** B, double** C) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// считаем погрешность, temp - AA-1 - E или наоборот
void pogreshnost(int n, double** A, double** invA, double* r1, double* r2, double** temp) {
    if (n > 11000) {
        *r1 = 0.0;
        *r2 = 0.0;
        return;
    }

    matrix_multiply(n, A, invA, temp);
    for (int i = 0; i < n; i++) {
        temp[i][i] -= 1.0;
    }
    *r1 = matrix_norm(n, temp);

    matrix_multiply(n, invA, A, temp);
    for (int i = 0; i < n; i++) {
        temp[i][i] -= 1.0; // единичную вычитаем
    }
    *r2 = matrix_norm(n, temp);

    //free_matrix(temp, n);
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Маловато аргументов\n");
        return 1;
    }

    // парсинг аргументов
    int n = atoi(argv[1]);
    int r = atoi(argv[2]);
    int s = atoi(argv[3]);
    char* filename = NULL;

    if (s == 0 && argc < 5) {
        printf("И где файл?\n");
        return 1;
    }
    if (s == 0) {
        filename = argv[4];
    }

    // создание нашей матрицы и ее вывод
    double** A = allocate_matrix(n, n);
    init_matrix(n, A, s, filename);
    printf("Исходная матрица (первые %dx%d элементов):\n", r, r);
    print_matrix(n, n, A, r);
    printf("\n");

    // копия матрицы A для вычислений
    // double** A_copy = allocate_matrix(n, n);
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         A_copy[i][j] = A[i][j];
    //     }
    // }

    double** invA = allocate_matrix(n, n);
    double** temp_matrix = allocate_matrix(n, n); // временная матричка
    int* perm = (int*)malloc(n * sizeof(int)); // массив перестановок
    for (int i = 0; i < n; i++) {
        perm[i] = i;
    }

    // считаем время вычисления обратной
    clock_t start = clock();
    int result = inverse_gauss(n, A, invA, perm, temp_matrix);
    clock_t end = clock();
    double t1 = (double)(end - start) / CLOCKS_PER_SEC; // в секундах время

    init_matrix(n, A, s, filename); // еще раз матрицу А, тк выше я ее испортила

    double r1, r2;
    double t2 = 0.0;

    if (result == 0) {
        // Выводим обратную матрицу
        printf("Обратная матрица (первые %dx%d элементов):\n", r, r);
        print_matrix(n, n, invA, r);

        // время вычисления погрешностей
        start = clock();
        pogreshnost(n, A, invA, &r1, &r2, temp_matrix);
        end = clock();
        t2 = (double)(end - start) / CLOCKS_PER_SEC;
    }
    else {
        r1 = -1.0;
        r2 = -1.0;
        printf("Вырожденная матрица, обратной нет\n");
    }

    printf("%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = %d\n",
           argv[0], 2, r1, r2, t1, t2, s, n);

    free_matrix(A, n);
    free_matrix(invA, n);
    free_matrix(temp_matrix, n);
    free(perm);

    return 0;
}
