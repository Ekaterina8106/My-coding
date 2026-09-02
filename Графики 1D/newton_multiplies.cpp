#include "newton_multiplies.h"
#include <cmath>

void build_newton_multiples(int n, const double *x, const double *f, const double *df,
                            double *coeff, double *temp) {
    int m = 2 * n;

    // Инициализация нулевого порядка
    for (int i = 0; i < n; ++i) {
        temp[2 * i]     = f[i];
        temp[2 * i + 1] = f[i];
    }
    coeff[0] = temp[0];

    for (int k = 1; k < m; ++k) {
        for (int i = 0; i < m - k; ++i) {
            int idx_i   = i / 2;
            int idx_ik  = (i + k) / 2;

            if (idx_i == idx_ik) {
                // Узлы совпадают – используем производную
                temp[i] = df[idx_i];
            } else {
                double yi   = x[idx_i];
                double yik  = x[idx_ik];
                temp[i] = (temp[i+1] - temp[i]) / (yik - yi);
            }
        }
        coeff[k] = temp[0];
    }
                            }

                            double newton_multiples_eval(double x, double /*a*/, double /*b*/, int n,
                                                         const double *x_nodes, const double *coeff) {
                                int m = 2 * n;
                                double res = coeff[m-1];
                                for (int i = m-2; i >= 0; --i) {
                                    int idx = i / 2;
                                    double yi = x_nodes[idx];
                                    res = coeff[i] + (x - yi) * res;
                                }
                                return res;
                                                         }
