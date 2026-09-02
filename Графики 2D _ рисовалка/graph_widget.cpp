#include "graph_widget.h"
#include <QKeyEvent>
#include <QPainter>
#include <cmath>
#include <cstdio>

static QColor getColor(double z, double minZ, double maxZ) {
    double t = (z - minZ) / (maxZ - minZ);
    t = std::max(0.0, std::min(1.0, t));
    //кто то делает по своему, потому что первое пришло в голову, а значит самое понятное
    // 0 - синий, 0.25 - голубой, 0.5 - зелёный, 0.75 - жёлтый, 1 - красный
    double r, g, b;
    if (t < 0.25) { // из синего в голубой
        double s = t / 0.25;  // 0..1
        r = 0;
        g = s * 255;
        b = 255;
    }
    else if (t < 0.5) { // голубой в зеленый
        double s = (t - 0.25) / 0.25;
        r = 0;
        g = 255;
        b = 255 * (1 - s);
    }
    else if (t < 0.75) { // желтый?
        double s = (t - 0.5) / 0.25;
        r = s * 255;
        g = 255;
        b = 0;
    }
    else { // красный
        double s = (t - 0.75) / 0.25;
        r = 255;
        g = 255 * (1 - s);
        b = 0;
    }
    return QColor((int)r, (int)g, (int)b);
}

GraphWidget::GraphWidget(QWidget* parent, double a, double b, double c, double d,
                         int nx, int ny, int mx, int my, int k)
: QWidget(parent), a_(a), b_(b), c_(c), d_(d), //прорисовка чанков в майнкрафте
nx_(nx), ny_(ny), mx_(mx), my_(my), k_(k), p_(0), s_(0), drawMode_(0), angle_(0.0),
func_(k), zValues_(nullptr), needRebuild_(true), dist_(7.0), lastWidth_(0), lastHeight_(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    max_abs_f_ = 0.0;
    for (int i = 0; i <= 100; i++) {
        double x = a + (b-a)*i/100.0;
        for (int j = 0; j <= 100; j++) {
            double y = c + (d-c)*j/100.0;
            double val = std::fabs(func_.eval(x,y));
            if (val > max_abs_f_) max_abs_f_ = val;
        }
    }
    updateApproximations();
}

GraphWidget::~GraphWidget() {
    delete[] zValues_;
}

QSize GraphWidget::minimumSizeHint() const { return QSize(200, 200); }
QSize GraphWidget::sizeHint() const { return QSize(800, 600); }

void GraphWidget::updateApproximations() {
    method1_.build(nx_, ny_, a_, b_, c_, d_, func_, p_, max_abs_f_);
    method2_.build(nx_, ny_, a_, b_, c_, d_, func_, p_, max_abs_f_);
    needRebuild_ = true;
    update();
}

void GraphWidget::recomputeBounds() {
    double cx = (a_ + b_)/2.0;
    double cy = (c_ + d_)/2.0;
    double halfX = (b_ - a_)/2.0 / (1 << s_);
    double halfY = (d_ - c_)/2.0 / (1 << s_);
    minX_ = cx - halfX;
    maxX_ = cx + halfX;
    minY_ = cy - halfY;
    maxY_ = cy + halfY;
}

void GraphWidget::recomputeValues() {
    if (zValues_) delete[] zValues_;
    zValues_ = new double[mx_ * my_];
    double dx = (maxX_ - minX_) / (mx_ - 1);
    double dy = (maxY_ - minY_) / (my_ - 1);
    minZ_ = 1e100; maxZ_ = -1e100;
    for (int i = 0; i < mx_; i++) {
        double x = minX_ + i*dx;
        for (int j = 0; j < my_; j++) {
            double y = minY_ + j*dy;
            double val = 0.0;
            switch (drawMode_) {
                case 0: val = func_.eval(x,y); break;
                case 1: val = method1_.eval(x,y); break;
                case 2: val = func_.eval(x,y) - method1_.eval(x,y); break;
                case 3: val = method2_.eval(x,y); break;
                case 4: val = func_.eval(x,y) - method2_.eval(x,y); break;
            }
            // Защита от некорректных значений
            if (!std::isfinite(val)) {
                val = 0.0;
            }
            zValues_[j*mx_ + i] = val;
            if (val < minZ_) minZ_ = val;
            if (val > maxZ_) maxZ_ = val;
        }
    }
    if (!std::isfinite(minZ_) || !std::isfinite(maxZ_) || fabs(maxZ_ - minZ_) < 1e-12) {
        minZ_ = -1.0;
        maxZ_ = 1.0;
    }
    //if (fabs(maxZ_ - minZ_) < 1e-12) maxZ_ = minZ_ + 1.0;
    maxAbsCurrent_ = std::max(fabs(minZ_), fabs(maxZ_));
    printf("max|F| = %g\n", maxAbsCurrent_);
    fflush(stdout);
}

// когда вышел из далеких земель в обычный майн
// void GraphWidget::initializeGL() {
//     initializeOpenGLFunctions();
//     glClearColor(0.2f, 0.2f, 0.3f, 0.1f); //
//     glEnable(GL_DEPTH_TEST);
//     glLineWidth(1.5f);
// }
/*
void GraphWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GraphWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (needRebuild_) {
        recomputeBounds();
        recomputeValues();
        needRebuild_ = false;
    }*/
/*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int w = width(), h = height();
    double aspect = (double)w / h;
    double fov = 45.0 * M_PI / 180.0;
    double nearPlane = 0.1, farPlane = 100.0;
    double top = nearPlane * tan(fov/2.0);
    double bottom = -top;
    double right = top * aspect;
    double left = -right;
    glFrustum(left, right, bottom, top, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    QMatrix4x4 view;
    view.lookAt(QVector3D(3.0f, 3.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    glMultMatrixf(view.constData());

    glRotatef(angle_, 0.0f, 0.0f, 1.0f);

    double cx = (minX_ + maxX_)/2.0;
    double cy = (minY_ + maxY_)/2.0;
    //double cz = (minZ_ + maxZ_)/2.0;
    double rangeX = maxX_ - minX_;
    double rangeY = maxY_ - minY_;
    double rangeZ = maxZ_ - minZ_;
    double scaleXY = (rangeX > rangeY) ? 2.0 / rangeX : 2.0 / rangeY;
    double scaleZ = 2.0 / rangeZ;
    glTranslatef(-cx, -cy, 0.0f); // в нач коорд
    //glTranslatef(-cx, -cy, -cz);
    glScalef(scaleXY, scaleXY, scaleZ);

    drawGrid();
    drawInfo();
}*/

// void GraphWidget::drawGrid() {
//     if (!zValues_) return;
//     double dx = (maxX_ - minX_) / (mx_ - 1);
//     double dy = (maxY_ - minY_) / (my_ - 1);
//     glColor3f(0.8f, 0.8f, 1.0f);
//     for (int j = 0; j < my_; ++j) {
//         double y = minY_ + j*dy;
//         glBegin(GL_LINE_STRIP);
//         for (int i = 0; i < mx_; ++i) {
//             double x = minX_ + i*dx;
//             double z = zValues_[j*mx_ + i];
//             glVertex3f(x, y, z);
//         }
//         glEnd();
//     }
//     Линии по Y (постоянный X)
//     for (int i = 0; i < mx_; ++i) {
//         double x = minX_ + i*dx;
//         glBegin(GL_LINE_STRIP);
//         for (int j = 0; j < my_; ++j) {
//             double y = minY_ + j*dy;
//             double z = zValues_[j*mx_ + i];
//             glVertex3f(x, y, z);
//         }
//         glEnd();
//     }
// }

QPointF GraphWidget::project(double x, double y, double z) const{
    double ugol = angle_ * M_PI / 180.0;
    double cosA = cos(ugol);
    double sinA = sin(ugol);
    double xr = x*cosA - y*sinA;
    double yr = x*sinA + y*cosA;
    double zr = z;

    double rangeX = maxX_ - minX_;
    double rangeY = maxY_ - minY_;
    double rangeZ = maxZ_ - minZ_;

    double cx = (minX_ + maxX_)/2.0;
    double cy = (minY_ + maxY_)/2.0;
    double cz = (minZ_ + maxZ_)/2.0;

    //double maxRange = std::max(rangeX, rangeY);
    double scaleXY = (rangeX > rangeY) ? 2.0 / rangeX : 2.0 / rangeY;
    double scaleZ = 2.0 / rangeZ;

    double xs = (xr - cx) * scaleXY;
    double ys = (yr - cy) * scaleXY;
    double zs = (zr - cz) * scaleZ;

    double dist = dist_;
    double tmp = dist - zs;
    if (fabs(tmp) < 1e-12) {
        tmp = 1e-12;
    }
    double musor = dist / (tmp);
    double xp = xs * musor;
    double yp = ys * musor;

    int w = width();
    int h = height();
    double scale = std::min(w,h)/2.0;
    double x_Win = w/2 + xp * scale;
    double y_Win = h/2 - yp * scale;
    return QPointF(x_Win, y_Win);
}

void GraphWidget::paintEvent(QPaintEvent* /*event*/) {
    if (needRebuild_){
        recomputeBounds();
        recomputeValues();
        needRebuild_ = false;
    }
    if (!zValues_) return;

    QPainter painter(this);
    painter.setPen(QPen(Qt::blue, 1.0));
    double dx = (maxX_ - minX_) / (mx_ - 1);
    double dy = (maxY_ - minY_) / (my_ - 1);

    for (int j = 0; j < my_; j++) {
        double y = minY_ + j*dy;
        QPointF pred = project(minX_, y, zValues_[j*mx_ + 0]);
        for (int i = 1; i < mx_; i++) {
            double x = minX_ + i*dx;
            double z_pred = zValues_[j*mx_ + i - 1];
            double z_nast = zValues_[j*mx_ + i];
            double z_mid = (z_pred + z_nast)/2.0;
            //if (std::abs(maxZ_ - minZ_) < 1e-12){
            QColor col = getColor(z_mid, minZ_, maxZ_);
            painter.setPen(QPen(col, 1.5));
            QPointF nast = project(x, y, zValues_[j*mx_ + i]);
            painter.drawLine(pred, nast);
            pred = nast;
            //}
        }
    }

    for (int i = 0; i < mx_; i++) {
        double x = minX_ + i*dx;
        QPointF pred = project(x, minY_, zValues_[i]);
        for (int j = 1; j < my_; j++) {
            double y = minY_ + j*dy;
            double z_pred = zValues_[(j-1)*mx_ + i];
            double z_nast = zValues_[j*mx_ + i];
            double z_mid = (z_pred + z_nast) / 2.0;
            //if (std::abs(maxZ_ - minZ_) < 1e-12){
            QColor col = getColor(z_mid, minZ_, maxZ_);
            painter.setPen(QPen(col, 1.5));
            QPointF nast = project(x, y, z_nast);
            painter.drawLine(pred, nast);
            pred = nast;
            //}
        }
    }

    painter.setPen(Qt::darkGreen);
    char buf[256];
    sprintf(buf, "k=%d %s   nx=%d ny=%d  s=%d  p=%d  угол=%.1f°",
            k_, func_.name(), nx_, ny_, s_, p_, angle_);
    painter.drawText(10, 20, buf);
    sprintf(buf, "max|F|=%g  minZ=%g maxZ=%g", maxAbsCurrent_, minZ_, maxZ_);
    painter.drawText(10, 40, buf);
    const char* modeNames[] = {"Функция", "Ньютон", "Ошибки Ньютона", "Эрмит", "Неточности Эрмита"};
    painter.drawText(10, 60, modeNames[drawMode_ % 5]);
    painter.end();
}

void GraphWidget::keyPressEvent(QKeyEvent* e) {
    bool rebuild = false;
    switch (e->key()) {
        case Qt::Key_0:
            k_ = (k_ + 1) % 8;
            func_.setK(k_);
            rebuild = true;
            break;
        case Qt::Key_1:
            drawMode_ = (drawMode_ + 1) % 5;
            rebuild = true;
            break;
        case Qt::Key_2:
            s_++;
            rebuild = true;
            break;
        case Qt::Key_3:
            if (s_ > 0) s_--;
            rebuild = true;
        break;
        case Qt::Key_4:
            if (nx_ * ny_ * 4 <= 10000000) { nx_ *= 2; ny_ *= 2; rebuild = true; }
            break;
        case Qt::Key_5:
            if (nx_ % 2 == 0 && ny_ % 2 == 0 && nx_ >= 10 && ny_ >= 10) { nx_ /= 2; ny_ /= 2; rebuild = true; }
            break;
        case Qt::Key_6:
            p_++;
            rebuild = true;
            break;
        case Qt::Key_7:
            p_--;
            rebuild = true;
            break;
        case Qt::Key_8:
            angle_ += 15.0;  // пи/12 = 15° так было вчера
            rebuild = true;
            break;
        case Qt::Key_9:
            angle_ -= 15.0;
            rebuild = true;
            break;
        default:
            QWidget::keyPressEvent(e);
            return;
    }
    if (rebuild) {
        updateApproximations();
    }
    update();
}
