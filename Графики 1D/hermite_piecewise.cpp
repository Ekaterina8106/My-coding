#include "hermite_piecewise.h"
#include <algorithm>

void build_hermite(int n, const double * /*x*/, const double *f, const double *df,
                   double *coeff, double * /*temp*/) {
    for (int i = 0; i < n; ++i) {
        coeff[i] = f[i];
        coeff[n + i] = df[i];
    }
                   }

                   double hermite_eval(double x, double a, double b, int n,
                                       const double *x_nodes, const double *coeff) {
                       const double *f = coeff;
                       const double *d = coeff + n;

                       if (x <= a) return f[0];
                       if (x >= b) return f[n-1];

                       int i = 0;
                       while (i < n-1 && x > x_nodes[i+1]) ++i;

                       double h = x_nodes[i+1] - x_nodes[i];
                       if (h < 1e-15) return f[i];

                       double dx = x - x_nodes[i];
                       double slope = (f[i+1] - f[i]) / h;

                       // Коэффициенты кубического многочлена Эрмита в базисе (x-x_i)
                       double c3 = (3.0 * slope - 2.0 * d[i] - d[i+1]) / h;
                       double c4 = (d[i] + d[i+1] - 2.0 * slope) / (h * h);

                       return f[i] + d[i] * dx + c3 * dx * dx + c4 * dx * dx * dx;
                                       }
