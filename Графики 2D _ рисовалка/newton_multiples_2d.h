#ifndef NEWTON_MULTIPLES_2D_H
#define NEWTON_MULTIPLES_2D_H

#include "function_2d.h"

class NewtonMultiples2D {
public:
    NewtonMultiples2D();
    ~NewtonMultiples2D();
    void build(int nx, int ny, double a, double b, double c, double d,
               const Function2D& func, int p, double max_abs_f);
    double eval(double x, double y) const;
private:
    int nx_, ny_;
    double a_, b_, c_, d_;
    double* coeffs_;
    double* x_nodes_;
    double* y_nodes_;
    double* x_rep_;
    double* y_rep_;
    void clear();
    static void buildNewton1D(int m, const double* x, const double* f,
                              const double* df, double* coeff);
};

#endif
