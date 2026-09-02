#include "newton_multiples_2d.h"
#include "common_defs.h"
#include <algorithm>
#include <cmath>
#include <cstring>

NewtonMultiples2D::NewtonMultiples2D()
    : nx_(0), ny_(0), coeffs_(nullptr), x_nodes_(nullptr), y_nodes_(nullptr),
      x_rep_(nullptr), y_rep_(nullptr) {}

NewtonMultiples2D::~NewtonMultiples2D() { clear(); }

void NewtonMultiples2D::clear() {
    delete[] coeffs_;   coeffs_ = nullptr;
    delete[] x_nodes_;  x_nodes_ = nullptr;
    delete[] y_nodes_;  y_nodes_ = nullptr;
    delete[] x_rep_;    x_rep_ = nullptr;
    delete[] y_rep_;    y_rep_ = nullptr;
}

void NewtonMultiples2D::buildNewton1D(int m, const double* x, const double* f,
                                      const double* df, double* coeff) {
    double** table = new double*[m];
    for (int i = 0; i < m; ++i) {
        table[i] = new double[m];
        table[i][0] = f[i];
    }
    for (int j = 1; j < m; ++j) {
        for (int i = 0; i < m - j; ++i) {
            if (std::fabs(x[i] - x[i + j]) <= EPSILON_FOR_COMPARE) {
                if (j == 1) {
                    table[i][j] = df[i];
                }
                else {
                    table[i][j] = 0.0;
                }
            }
            else {
                table[i][j] = (table[i + 1][j - 1] - table[i][j - 1]) / (x[i + j] - x[i]);
            }
        }
    }
    for (int i = 0; i < m; ++i) {
        coeff[i] = table[0][i];
    }

    for (int i = 0; i < m; ++i) {
        delete[] table[i];
    }
    delete[] table;
}

void NewtonMultiples2D::build(int nx, int ny, double a, double b,
                              double c, double d, const Function2D& func,
                              int p, double max_abs_f) {
    clear();
    nx_ = nx;
    ny_ = ny;
    a_ = a;
    b_ = b;
    c_ = c;
    d_ = d;

    x_nodes_ = new double[nx];
    y_nodes_ = new double[ny];
    for (int i = 0; i < nx; ++i)
        x_nodes_[i] = a + (b - a) * i / (nx - 1);
    for (int j = 0; j < ny; ++j)
        y_nodes_[j] = c + (d - c) * j / (ny - 1);

    int m = 2 * nx;
    int n = 2 * ny;
    x_rep_ = new double[m];
    y_rep_ = new double[n];
    for (int i = 0; i < nx; ++i) {
        x_rep_[2 * i] = x_rep_[2 * i + 1] = x_nodes_[i];
    }
    for (int j = 0; j < ny; ++j) {
        y_rep_[2 * j] = y_rep_[2 * j + 1] = y_nodes_[j];
    }

    double** coeffs_x = new double*[ny];
    double** coeffs_fy = new double*[ny];
    for (int j = 0; j < ny; ++j) {
        coeffs_x[j] = new double[m];
        coeffs_fy[j] = new double[m];
    }

    int midx = nx / 2;
    int midy = ny / 2;

    for (int j = 0; j < ny; ++j) {
        double y = y_nodes_[j];

        double* f_rep = new double[m];
        double* df_rep = new double[m];
        for (int i = 0; i < nx; ++i) {
            double x = x_nodes_[i];
            double f_val = func.eval(x, y);
            if (i == midx && j == midy)
                f_val += p * 0.1 * max_abs_f;
            f_rep[2 * i] = f_rep[2 * i + 1] = f_val;
            df_rep[2 * i] = df_rep[2 * i + 1] = func.eval_fx(x, y);
        }
        buildNewton1D(m, x_rep_, f_rep, df_rep, coeffs_x[j]);

        for (int i = 0; i < nx; ++i) {
            double x = x_nodes_[i];
            f_rep[2 * i] = f_rep[2 * i + 1] = func.eval_fy(x, y);
            df_rep[2 * i] = df_rep[2 * i + 1] = func.eval_fxy(x, y);
        }
        buildNewton1D(m, x_rep_, f_rep, df_rep, coeffs_fy[j]);

        delete[] f_rep;
        delete[] df_rep;
    }

    coeffs_ = new double[m * n];
    for (int i = 0; i < m; ++i) {
        double* f_rep = new double[n];
        double* df_rep = new double[n];
        for (int j = 0; j < ny; ++j) {
            f_rep[2 * j] = f_rep[2 * j + 1] = coeffs_x[j][i];
            df_rep[2 * j] = df_rep[2 * j + 1] = coeffs_fy[j][i];
        }
        double* coeff_row = new double[n];
        buildNewton1D(n, y_rep_, f_rep, df_rep, coeff_row);
        for (int j = 0; j < n; ++j) {
            coeffs_[i * n + j] = coeff_row[j];
        }
        delete[] f_rep;
        delete[] df_rep;
        delete[] coeff_row;
    }

    for (int j = 0; j < ny; ++j) {
        delete[] coeffs_x[j];
        delete[] coeffs_fy[j];
    }
    delete[] coeffs_x;
    delete[] coeffs_fy;
}

double NewtonMultiples2D::eval(double x, double y) const {
    if (!coeffs_) return 0.0;
    int m = 2 * nx_;
    int n = 2 * ny_;
    double res = 0.0;
    double x_term = 1.0;
    for (int i = 0; i < m; ++i) {
        double y_term = 1.0;
        for (int j = 0; j < n; ++j) {
            res += coeffs_[i * n + j] * x_term * y_term;
            y_term *= (y - y_rep_[j]);
        }
        x_term *= (x - x_rep_[i]);
    }
    return res;
}
