#include "cubic_hermite_piecewise_2d.h"
#include <algorithm>
#include <cstring>

CubicHermitePiecewise2D::CubicHermitePiecewise2D() : nx_(0), ny_(0), coeffs_(nullptr), x_nodes_(nullptr), y_nodes_(nullptr) {}
CubicHermitePiecewise2D::~CubicHermitePiecewise2D() { clear(); }

void CubicHermitePiecewise2D::clear() {
    delete[] coeffs_; coeffs_ = nullptr;
    delete[] x_nodes_; x_nodes_ = nullptr;
    delete[] y_nodes_; y_nodes_ = nullptr;
}

void CubicHermitePiecewise2D::build(int nx, int ny, double a, double b, double c, double d,
                                    const Function2D& func, int p, double max_abs_f) {
    clear();
    nx_ = nx; ny_ = ny;
    a_ = a; b_ = b; c_ = c; d_ = d;
    x_nodes_ = new double[nx];
    y_nodes_ = new double[ny];
    for (int i = 0; i < nx; ++i) x_nodes_[i] = a + (b-a)*i/(nx-1);
    for (int j = 0; j < ny; ++j) y_nodes_[j] = c + (d-c)*j/(ny-1);

    int cx = nx-1, cy = ny-1;
    coeffs_ = new double[cx * cy * 16];
    double* ptr = coeffs_;
    int midx = nx/2, midy = ny/2;
    for (int i = 0; i < cx; ++i) {
        double xl = x_nodes_[i], xr = x_nodes_[i+1];
        for (int j = 0; j < cy; ++j) {
            double yb = y_nodes_[j], yt = y_nodes_[j+1];
            double f00 = func.eval(xl, yb);
            double f10 = func.eval(xr, yb);
            double f01 = func.eval(xl, yt);
            double f11 = func.eval(xr, yt);
            // возмущение в центральном узле
            if (i == midx && j == midy) f00 += p * 0.1 * max_abs_f;
            if (i+1 == midx && j == midy) f10 += p * 0.1 * max_abs_f;
            if (i == midx && j+1 == midy) f01 += p * 0.1 * max_abs_f;
            if (i+1 == midx && j+1 == midy) f11 += p * 0.1 * max_abs_f;
            double fx00 = func.eval_fx(xl, yb);
            double fx10 = func.eval_fx(xr, yb);
            double fx01 = func.eval_fx(xl, yt);
            double fx11 = func.eval_fx(xr, yt);
            double fy00 = func.eval_fy(xl, yb);
            double fy10 = func.eval_fy(xr, yb);
            double fy01 = func.eval_fy(xl, yt);
            double fy11 = func.eval_fy(xr, yt);
            double fxy00 = func.eval_fxy(xl, yb);
            double fxy10 = func.eval_fxy(xr, yb);
            double fxy01 = func.eval_fxy(xl, yt);
            double fxy11 = func.eval_fxy(xr, yt);
            double data[16] = {
                f00, fx00, fy00, fxy00,
                f10, fx10, fy10, fxy10,
                f01, fx01, fy01, fxy01,
                f11, fx11, fy11, fxy11
            };
            memcpy(ptr, data, sizeof(double)*16);
            ptr += 16;
        }
    }
}

double CubicHermitePiecewise2D::eval(double x, double y) const {
    if (!coeffs_) return 0.0;
    if (x <= x_nodes_[0]) x = x_nodes_[0] + 1e-12;
    if (x >= x_nodes_[nx_-1]) x = x_nodes_[nx_-1] - 1e-12;
    if (y <= y_nodes_[0]) y = y_nodes_[0] + 1e-12;
    if (y >= y_nodes_[ny_-1]) y = y_nodes_[ny_-1] - 1e-12;
    int i = (int)((x - a_) / (b_ - a_) * (nx_-1));
    i = std::max(0, std::min(nx_-2, i));
    int j = (int)((y - c_) / (d_ - c_) * (ny_-1));
    j = std::max(0, std::min(ny_-2, j));
    double xl = x_nodes_[i], xr = x_nodes_[i+1];
    double yb = y_nodes_[j], yt = y_nodes_[j+1];
    double hx = xr - xl, hy = yt - yb;
    double u = std::max(0.0, std::min(1.0, (x - xl)/hx));
    double v = std::max(0.0, std::min(1.0, (y - yb)/hy));
    const double* base = coeffs_ + (i * (ny_-1) + j) * 16;
    auto H0 = [](double t) { double t2=t*t, t3=t2*t; return 1 - 3*t2 + 2*t3; };
    auto H1 = [](double t) { double t2=t*t, t3=t2*t; return t - 2*t2 + t3; };
    auto H2 = [](double t) { double t2=t*t, t3=t2*t; return 3*t2 - 2*t3; };
    auto H3 = [](double t) { double t2=t*t, t3=t2*t; return -t2 + t3; };
    double val = 0.0;
    val += base[0]  * H0(u)*H0(v);
    val += base[1]  * hx * H1(u)*H0(v);
    val += base[2]  * hy * H0(u)*H1(v);
    val += base[3]  * hx*hy * H1(u)*H1(v);
    val += base[4]  * H2(u)*H0(v);
    val += base[5]  * hx * H3(u)*H0(v);
    val += base[6]  * hy * H2(u)*H1(v);
    val += base[7]  * hx*hy * H3(u)*H1(v);
    val += base[8]  * H0(u)*H2(v);
    val += base[9]  * hx * H1(u)*H2(v);
    val += base[10] * hy * H0(u)*H3(v);
    val += base[11] * hx*hy * H1(u)*H3(v);
    val += base[12] * H2(u)*H2(v);
    val += base[13] * hx * H3(u)*H2(v);
    val += base[14] * hy * H2(u)*H3(v);
    val += base[15] * hx*hy * H3(u)*H3(v);
    return val;
}
