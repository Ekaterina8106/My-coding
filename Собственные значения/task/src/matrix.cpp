#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../inc/matrix.h"

// Функция выделения памяти для матрицы
double** allocate_matrix(int n, int m) {
    double** matrix = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*)malloc(m * sizeof(double));
    }
    return matrix;
}

// Функция освобождения памяти
void free_matrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Функции для инициализации матрицы по формулам из EigenvaluesTasks.pdf
double f_eigen(int k, int n, int i, int j) {
    i++; j++; // Переход к 1-индексации
    switch(k) {
        case 1: return n - (i > j ? i : j) + 1;
        case 2:
            if (i == j) return 2.0;
            if (abs(i - j) == 1) return -1.0;
            return 0.0;
        case 3:
            if (i == j && i < n) return 1.0;
            if (j == n) return (double)i;
            if (i == n) return (double)j;
            return 0.0;
        case 4: return 1.0 / (i + j - 1);
        default: return 0.0;
    }
}

// Функция инициализации матрицы
void init_matrix(int n, double** A, int k, char* filename) {
    if (k != 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = f_eigen(k, n, i, j);
            }
        }
    } else {
        FILE* file = fopen(filename, "r");
        if (!file) {
            printf("Ошибка открытия файла %s\n", filename);
            exit(1);
        }

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (fscanf(file, "%lf", &A[i][j]) != 1) {
                    printf("Ошибка чтения матрицы из файла\n");
                    fclose(file);
                    exit(1);
                }
            }
        }
        fclose(file);
    }
}

// Функция печати матрицы
void print_matrix(int n, int m, double** A, int r) {
    int rows = n < r ? n : r;
    int cols = m < r ? m : r;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf(" %10.3e", A[i][j]);
        }
        printf("\n");
    }
}
