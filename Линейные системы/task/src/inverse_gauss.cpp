#include <stdlib.h>
#include <math.h>
#include "../inc/matrix.h"
#include "../inc/inverse_gauss.h"

// обратная матрица методом Гаусса
int inverse_gauss(int n, double** A, double** invA, int* perm, double** temp_matrix) {
    // Инициализируем обратную матрицу как единичную
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            invA[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }

    // основной цикл метода Гаусса, у меня с выбором главного элемента по СТРОКЕ
    for (int k = 0; k < n; k++) {
        int main_col = k; // столбец главный
        double max_val = fabs(A[k][k]); //наибольшее ПО МОДУЛЮ значение

        // ищу максимальный в строке и заменяю потом индекс
        for (int j = k + 1; j < n; j++) {
            if (fabs(A[k][j]) > max_val) {
                max_val = fabs(A[k][j]);
                main_col = j;
            }
        }

        // Если вдруг максимальный по строке 0, очевидно что матрица вырожденная )
        if (max_val < 1e-15) {
            free(perm);
            return 1; // Матрица вырождена
        }

        // на данном этапе у меня наибольший в строке элемент на месте main_col? а я хочу на место k его, те диагональ

        // двигаем максимальный элемент на свое место
        if (main_col != k) {
            for (int i = 0; i < n; i++) {
                double musor = A[i][k];
                A[i][k] = A[i][main_col];
                A[i][main_col] = musor;
            }
            // запоминаем какие стб поменялись местами
            int xlam = perm[k];
            perm[k] = perm[main_col];
            perm[main_col] = xlam;
        }

        double main_el = A[k][k];
        for (int j = 0; j < n; j++) {
            A[k][j] /= main_el;
            invA[k][j] /= main_el;
        }

        for (int i = 0; i < n; i++) {
            if (i == k) continue; //
            double usless = A[i][k];
            for (int j = 0; j < n; j++) {
                A[i][j] -= usless * A[k][j]; // зануляем все остальные элементы
                invA[i][j] -= usless * invA[k][j];
            }
        }
    }

    // досчитываем обратную
    // чтобы вернуть нормальный порядок стб
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            temp_matrix[i][j] = invA[i][j];
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            invA[perm[i]][j] = temp_matrix[i][j]; // меняем местами строки
        }
    }


    return 0;
}
