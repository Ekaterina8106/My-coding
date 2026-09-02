#ifndef CUBIC_HERMITE_PIECEWISE_2D_H
#define CUBIC_HERMITE_PIECEWISE_2D_H

#include "function_2d.h"

class CubicHermitePiecewise2D {
public:
    CubicHermitePiecewise2D();
    ~CubicHermitePiecewise2D();
    void build(int nx, int ny, double a, double b, double c, double d,
               const Function2D& func, int p, double max_abs_f);
    double eval(double x, double y) const;
private:
    int nx_, ny_;
    double a_, b_, c_, d_;
    double* coeffs_; // 16*(nx-1)*(ny-1)
    double* x_nodes_, *y_nodes_;
    void clear();
};

#endif
