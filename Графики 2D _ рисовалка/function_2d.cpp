#include "function_2d.h"
#include <cmath>

Function2D::Function2D(int k) : k_(k) {}

void Function2D::setK(int k) { k_ = k; }

double Function2D::eval(double x, double y) const {
    switch (k_) {
        case 0: return 1.0;
        case 1: return x;
        case 2: return y;
        case 3: return x + y;
        case 4: return sqrt(x*x + y*y);
        case 5: return x*x + y*y;
        case 6: return exp(x*x - y*y);
        case 7: return 1.0 / (25.0*(x*x + y*y) + 1.0);
        default: return 0.0;
    }
}

double Function2D::eval_fx(double x, double y) const {
    switch (k_) {
        case 0: return 0.0;
        case 1: return 1.0;
        case 2: return 0.0;
        case 3: return 1.0;
        case 4: return x / (sqrt(x*x + y*y) + 1e-15);
        case 5: return 2.0*x;
        case 6: return 2.0*x * exp(x*x - y*y);
        case 7: {
            double den = 25.0*(x*x + y*y) + 1.0;
            return -50.0*x / (den*den);
        }
        default: return 0.0;
    }
}

double Function2D::eval_fy(double x, double y) const {
    switch (k_) {
        case 0: return 0.0;
        case 1: return 0.0;
        case 2: return 1.0;
        case 3: return 1.0;
        case 4: return y / (sqrt(x*x + y*y) + 1e-15);
        case 5: return 2.0*y;
        case 6: return -2.0*y * exp(x*x - y*y);
        case 7: {
            double den = 25.0*(x*x + y*y) + 1.0;
            return -50.0*y / (den*den);
        }
        default: return 0.0;
    }
}

double Function2D::eval_fxy(double x, double y) const {
    switch (k_) {
        case 0: return 0.0;
        case 1: return 0.0;
        case 2: return 0.0;
        case 3: return 0.0;
        case 4: return -x*y / (x*x + y*y + 1e-15) / (sqrt(x*x + y*y) + 1e-15);
        case 5: return 0.0;
        case 6: return -4.0*x*y * exp(x*x - y*y);
        case 7: {
            double den = 25.0*(x*x + y*y) + 1.0;
            return 5000.0*x*y / (den*den*den);
        }
        default: return 0.0;
    }
}

const char* Function2D::name() const {
    switch (k_) {
        case 0: return "1";
        case 1: return "x";
        case 2: return "y";
        case 3: return "x+y";
        case 4: return "sqrt(x^2+y^2)";
        case 5: return "x^2+y^2";
        case 6: return "exp(x^2-y^2)";
        case 7: return "1/(25(x^2+y^2)+1)";
        default: return "";
    }
}
