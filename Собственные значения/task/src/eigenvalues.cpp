#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "../inc/eigenvalues.h"

// Вспомогательная функция для вычисления нормы вектора
double vector_norm(int n, double* v) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}

// Приведение к почти треугольному виду методом отражений
void pochti_treyg(int n, double** A) {
    double* v = (double*)malloc(n*sizeof(double));
    for (int k = 0; k < n - 2; k++) { // обнуляем именно под главной и 1ой под
        //главной диагональю элементы, те в k стб зануляем k+2, ... n-1
        // Вычисление нормы текущего столбца под диагональю
        double norm = 0.0;
        for (int i = k + 1; i < n; i++) {
            norm += A[i][k] * A[i][k];
        }
        norm = sqrt(norm);

        if (norm < 1e-15){
            for (int i = k + 2; i < n; i++) {
                A[i][k] = 0.0;
            }
            continue;
        }

        double sign = (A[k+1][k] >= 0) ? 1.0 : -1.0;
        double beta = 1.0 / (norm * (norm + fabs(A[k+1][k])));

        // Создание вектора отражения. Сначала он нулевой
        for (int i = 0; i < n; i++) {
            v[i] = 0.0;
        }
        // v = x + sign*норма*e_1 именно поэтому меняется "первый" элемет, остальные нет
        // Первый элемент вектора отражения
        v[k+1] = A[k+1][k] + sign * norm;

        // Остальные элементы., те по факту это зануляем столбец... ниже диагонали
        for (int i = k + 2; i < n; i++) {
            v[i] = A[i][k];
        }
        // Применение отражения слева: A = (E - beta * v * v^T) * A
        for (int j = k; j < n; j++) { // тут происходит v^T * A, те идем по стб матрицы А
            double musor = 0.0;
            for (int i = k + 1; i < n; i++) {
                musor += v[i] * A[i][j];
            }
            for (int i = k + 1; i < n; i++) {
                A[i][j] -= beta * v[i] * musor;
            }
        } //после него элементы в стб становятся 0

        // Применение отражения справа: A = A * (Е - 2*v*v^T)
        for (int i = 0; i < n; i++) { // A * v, соответственно по строке бегу
            double musor = 0.0;
            for (int j = k + 1; j < n; j++) {
                musor += A[i][j] * v[j];
            }
            for (int j = k + 1; j < n; j++) {
                A[i][j] -= beta * v[j] * musor;
            }
        }

        // Обнуление элементов под второй диагональю (мало ли)
        for (int i = k + 2; i < n; i++) {
            A[i][k] = 0.0;
        }
    }
    free(v);
}

// LR-АЛГОРИТМ
int lr_algorithm(int n, double** A, double* eigenvalues, double epsilon, int max_iterations) {
    // Счетчик итераций - сколько раз мы выполнили LR-разложение + перемножение
    int iterations = 0;

    // L - нижняя треугольная матрица с единицами на диагонали
    // R - верхняя треугольная матрица
    double** L = (double**)malloc(n * sizeof(double*));
    double** R = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        L[i] = (double*)malloc(n * sizeof(double));
        R[i] = (double*)malloc(n * sizeof(double));
    }

    // итерационный процесс A_{k+1} = R_k × L_k
    for (int iter = 0; iter < max_iterations; iter++) {
        // if (iter % 100 == 0) {
        //     printf("LR: итерация %d\n", iter);
        // } // ДА ОНО РАБОТАЕТ УРА
        // L единичная матрица, R - копия текущей матрицы A
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                L[i][j] = (i == j) ? 1.0 : 0.0;
                R[i][j] = A[i][j];
            }
        }

        //int singular = 0;
        // Проходим по всем столбцам кроме последнего
        for (int k = 0; k < n - 1; k++) {
            if (fabs(R[k][k]) < 1e-15) { // проверка на почти 0 эл
                for (int i = k + 1; i < n; i++) {
                    L[i][k] = 0.0;
                }
                //singular = 1;
                continue;
            }
            // Для каждой строки под текущим диагональным элементом
            for (int i = k + 1; i < n; i++) {
                // Это коэффициент, на который нужно умножить строку k, чтобы обнулить элемент R[i][k]
                L[i][k] = R[i][k] / R[k][k];

                // ВЫЧИТАЕМ ИЗ СТРОКИ i СТРОКУ k, УМНОЖЕННУЮ НА МНОЖИТЕЛЬ
                // Это обнуляет элемент R[i][k] и преобразует остальные элементы строки i
                for (int j = k; j < n; j++) {
                    if (fabs(L[i][k]) < 1e-15){
                        L[i][k]=0;
                    }
                    double product = L[i][k] * R[k][j]; // тут ошибка ля. тип 0?
                    R[i][j] = R[i][j] - product;
                    
                }
            }
        }

        // перемножаем матрицы A = R*L
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = 0.0;
                for (int k = 0; k < n; k++) {
                    A[i][j] += R[i][k] * L[k][j];
                }
            }
        }

        // Каждая итерация = одно LR-разложение + одно перемножение
        iterations++;

        // Ищем МАКСИМАЛЬНЫЙ ВНЕДИАГОНАЛЬНЫЙ ЭЛЕМЕНТ под побочной диагональю
        double max_subdiagonal = 0.0; // потом буду сравнивать с нормой матрицы

        // Проходим по всем строкам начиная со 2-й (i=1)
        for (int i = 1; i < n; i++) {
            double current_element = fabs(A[i][i-1]);
            if (current_element > max_subdiagonal) {
                max_subdiagonal = current_element;
                }
            }


        // Если все внедиагональные эелементы меньше точности заданной, то матрица
        // стала оч близкой к треугольной
        double norm_A_sq = 0.0;
        
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if(A[i][j] < 1e-15){
                    continue;
                }
                norm_A_sq += A[i][j] * A[i][j];
            }
        }
        double norm_A = sqrt(norm_A_sq);
    
        if (max_subdiagonal < epsilon * norm_A) {
            break;
        }
        if (iter >= max_iterations - 1) {
            break;
        }
    }

    // После сходимости диагональные элементы A содержат собственные значения
    for (int i = 0; i < n; i++) {
        eigenvalues[i] = A[i][i];
    }

    for (int i = 0; i < n; i++) {
        free(L[i]);
        free(R[i]);
    }
    free(L);
    free(R);

    return iterations;
}

// Вычисление невязок
void compute_residuals(int n, double trace_A, double norm_A, double* eigenvalues, double* res1, double* res2) {
    
    //double trace_A = 0.0; // след
    double sum_eigenvalues = 0.0; // сумма сз
    //double norm_A_sq = 0.0; // A2
    double sum_eigenvalues_sq = 0.0; // сумма квадратов сз

    for (int i = 0; i < n; i++) {
        //trace_A += A[i][i];
        sum_eigenvalues += eigenvalues[i];
        sum_eigenvalues_sq += eigenvalues[i] * eigenvalues[i];
    }
    // Защита от деления на ноль
    if (norm_A < 1e-15) { // здравствуйте машинный эпсилон
        *res1 = 0.0;
        *res2 = 0.0;
    } else {
        *res1 = fabs(trace_A - sum_eigenvalues) / norm_A;
        *res2 = fabs(norm_A - sqrt(sum_eigenvalues_sq)) / norm_A;
    }
}
