#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../inc/matrix.h"

// функция, чтобы выделять память под матрицу (размер n*m)
double** allocate_matrix(int n, int m) {
    double** matrix = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*)malloc(m * sizeof(double));
    }
    return matrix;
}
// очищаем память. Мне просто лень писать 100 раз эти несчастные 3 строки
void free_matrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// сказали сделать, я сделала... (5 требование)
double f(int s, int n, int i, int j) {
    i++; j++;
    switch(s) {
        case 1: return n - (i > j ? i : j) + 1;
        case 2: return (i > j ? i : j);
        case 3: return (double)abs(i - j);
        case 4: return 1.0 / (i + j - 1);
        default: return 0.0;
    }
}

// инициализация матрицы, 0 при считывании из файла
void init_matrix(int n, double** A, int s, char* filename) {
    if (s != 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = f(s, n, i, j);
            }
        }
    }
    else {
        FILE* file = fopen(filename, "r");
        if (!file) {
            printf("Ошибка открытия файла %s\n", filename);
            free_matrix(A, n);
            exit(1);
            //return 1; угу, void функция, какой return
        }
        // проверка на корректность ввода матрицы
        // тут учитывается и корректный ввод и количество элементов
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (fscanf(file, "%lf", &A[i][j]) != 1) {
                    printf("Какая то фигня, а не матрица\n");
                    free_matrix(A, n);
                    fclose(file);
                    exit(1);
                }
            }
        }
        fclose(file);
    }
}
