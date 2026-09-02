#include "graph_widget.h"
#include "common_defs.h"
#include "functions.h"
#include "newton_multiplies.h"
#include "hermite_piecewise.h"

#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <QTextStream>  // для QString::asprintf или QTextStream
#include <cmath>
#include <algorithm>

GraphWidget::GraphWidget(QWidget *parent, double a, double b, int n, int k)
: QWidget(parent)
, m_a_original(a)
, m_b_original(b)
, m_center((a + b) / 2.0)
, m_n(n)
, m_k(k)
, m_s(0)
, m_p(0)
, m_displayMode(0)
, m_a_disp(a)
, m_b_disp(b)
, m_min_y(0.0)
, m_max_y(0.0)
, m_max_abs_f(0.0)
, m_valid1(false)
, m_valid2(false)
, m_func_name(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
    setFunctionK(m_k);
    updateNodesAndApprox();
}

QSize GraphWidget::minimumSizeHint() const {
    return QSize(100, 100);
}

QSize GraphWidget::sizeHint() const {
    return QSize(1000, 1000);
}

void GraphWidget::setFunctionK(int k) {
    m_k = k;
    // Используем лямбды с захватом k; они будут преобразованы в std::function
    m_func = [k](double x) { return func(k, x); };
    m_deriv = [k](double x) { return func_deriv(k, x); };
    m_func_name = func_name(k);
}

void GraphWidget::updateNodesAndApprox() {
    // Генерация равномерных узлов
    m_x_nodes.resize(m_n);
    double h = (m_b_original - m_a_original) / (m_n - 1);
    for (int i = 0; i < m_n; ++i) {
        m_x_nodes[i] = m_a_original + i * h;
    }

    // Максимум |f| на исходном отрезке
    m_max_abs_f = computeMaxAbsF();

    // Значения функции с учётом возмущения
    int mid = m_n / 2; // индекс центрального узла (с нуля)
    m_f_nodes.resize(m_n);
    for (int i = 0; i < m_n; ++i) {
        double val = m_func(m_x_nodes[i]);
        if (i == mid) {
            val += m_p * 0.1 * m_max_abs_f;
        }
        m_f_nodes[i] = val;
    }

    // Производные (без возмущения)
    m_df_nodes.resize(m_n);
    for (int i = 0; i < m_n; ++i) {
        m_df_nodes[i] = m_deriv(m_x_nodes[i]);
    }

    // Метод 1 (Ньютон) – только при n <= 50
    m_valid1 = (m_n <= 50);
    if (m_valid1) {
        m_coeff1.resize(2 * m_n);
        std::vector<double> temp(2 * m_n);
        build_newton_multiples(m_n, m_x_nodes.data(), m_f_nodes.data(), m_df_nodes.data(),
                               m_coeff1.data(), temp.data());
    }

    // Метод 2 (Эрмит) – всегда
    m_coeff2.resize(2 * m_n);
    build_hermite(m_n, m_x_nodes.data(), m_f_nodes.data(), m_df_nodes.data(),
                  m_coeff2.data(), nullptr);
    m_valid2 = true;

    recomputeBounds();
    update();
}

void GraphWidget::recomputeBounds() {
    double step = (m_b_disp - m_a_disp) / PLOT_RESOLUTION;
    m_min_y = 1e100;
    m_max_y = -1e100;

    auto update = [&](double y) {
        if (y < m_min_y) m_min_y = y;
        if (y > m_max_y) m_max_y = y;
    };

        for (double x = m_a_disp; x <= m_b_disp + EPSILON_FOR_COMPARE; x += step) {
            double fx = m_func(x);
            if (m_displayMode == 0) {
                update(fx);
            } else if (m_displayMode == 1) {
                update(fx);
                if (m_valid1) update(newton_multiples_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff1.data()));
            } else if (m_displayMode == 2) {
                update(fx);
                if (m_valid2) update(hermite_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff2.data()));
            } else if (m_displayMode == 3) {
                update(fx);
                if (m_valid1) update(newton_multiples_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff1.data()));
                if (m_valid2) update(hermite_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff2.data()));
            } else if (m_displayMode == 4) {
                if (m_valid1) {
                    double err1 = std::abs(fx - newton_multiples_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff1.data()));
                    update(err1);
                }
                if (m_valid2) {
                    double err2 = std::abs(fx - hermite_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff2.data()));
                    update(err2);
                }
            }
        }

        // Небольшой отступ по вертикали
        double dy = 0.01 * (m_max_y - m_min_y);
        if (dy < 1e-12) dy = 1.0;
        m_min_y -= dy;
    m_max_y += dy;

    double max_abs = std::max(std::abs(m_min_y), std::abs(m_max_y));
    qDebug() << "Max |y| =" << max_abs;
}

QPointF GraphWidget::graphToWindow(double x_graph, double y_graph) {
    double x_win = (x_graph - m_a_disp) / (m_b_disp - m_a_disp) * width();
    double y_win = (m_max_y - y_graph) / (m_max_y - m_min_y) * height();
    return QPointF(x_win, y_win);
}

double GraphWidget::computeMaxAbsF() {
    double max_abs = 0.0;
    double step = (m_b_original - m_a_original) / 1000.0;
    for (double x = m_a_original; x <= m_b_original + EPSILON_FOR_COMPARE; x += step) {
        double y = m_func(x);
        double abs_y = std::abs(y);
        if (abs_y > max_abs) max_abs = abs_y;
    }
    return max_abs;
}

void GraphWidget::paintEvent(QPaintEvent * /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    double range = (m_b_original - m_a_original) / 2.0;
    m_a_disp = m_center - range / std::pow(2.0, m_s);
    m_b_disp = m_center + range / std::pow(2.0, m_s);

    recomputeBounds();

    QPen pen_black(Qt::black, 0, Qt::SolidLine);
    QPen pen_blue(Qt::blue, 0, Qt::SolidLine);
    QPen pen_green(Qt::green, 0, Qt::SolidLine);

    painter.setPen(pen_black);
    painter.drawLine(graphToWindow(m_a_disp, 0), graphToWindow(m_b_disp, 0));
    painter.drawLine(graphToWindow(0, m_min_y), graphToWindow(0, m_max_y));

    auto drawGraph = [&](const QPen &pen, std::function<double(double)> f) {
        painter.setPen(pen);
        double step = (m_b_disp - m_a_disp) / PLOT_RESOLUTION;
        double x1 = m_a_disp;
        double y1 = f(x1);
        for (double x2 = x1 + step; x2 <= m_b_disp + EPSILON_FOR_COMPARE; x2 += step) {
            double y2 = f(x2);
            painter.drawLine(graphToWindow(x1, y1), graphToWindow(x2, y2));
            x1 = x2;
            y1 = y2;
        }
    };

    if (m_displayMode == 0) {
        drawGraph(pen_black, m_func);
    } else if (m_displayMode == 1) {
        drawGraph(pen_black, m_func);
        if (m_valid1)
            drawGraph(pen_blue, [this](double x) {
                return newton_multiples_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff1.data());
            });
    } else if (m_displayMode == 2) {
        drawGraph(pen_black, m_func);
        if (m_valid2)
            drawGraph(pen_green, [this](double x) {
                return hermite_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff2.data());
            });
    } else if (m_displayMode == 3) {
        drawGraph(pen_black, m_func);
        if (m_valid1)
            drawGraph(pen_blue, [this](double x) {
                return newton_multiples_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff1.data());
            });
        if (m_valid2)
            drawGraph(pen_green, [this](double x) {
                return hermite_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff2.data());
            });
    } else if (m_displayMode == 4) {
        if (m_valid1)
            drawGraph(pen_blue, [this](double x) {
                return std::abs(m_func(x) - newton_multiples_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff1.data()));
            });
        if (m_valid2)
            drawGraph(pen_green, [this](double x) {
                return std::abs(m_func(x) - hermite_eval(x, m_a_original, m_b_original, m_n, m_x_nodes.data(), m_coeff2.data()));
            });
    }

    painter.setPen(Qt::black);
    // Используем QString::asprintf (современная замена sprintf)
    QString info = QString::asprintf("k=%d %s   n=%d   s=%d   p=%d   max|y|=%g",
                                     m_k, m_func_name, m_n, m_s, m_p,
                                     std::max(std::abs(m_min_y), std::abs(m_max_y)));
    painter.drawText(10, 20, info);
}

void GraphWidget::keyPressEvent(QKeyEvent *event) {
    //qDebug() << "Key pressed:" << event->key() << "text:" << event->text();
    bool need_update = false;
    bool need_rebuild = false;

    switch (event->key()) {
        case Qt::Key_0:
            m_k = (m_k + 1) % 7;
            setFunctionK(m_k);
            need_rebuild = true;
            break;
        case Qt::Key_1:
            m_displayMode = (m_displayMode + 1) % 5;
            need_update = true;
            break;
        case Qt::Key_2:
            m_s++;
            need_update = true;
            break;
        case Qt::Key_3:
            m_s--;
            need_update = true;
            break;
        case Qt::Key_4:
            m_n *= 2;
            if (m_n < 2) m_n = 2;
            need_rebuild = true;
        break;
        case Qt::Key_5:
            m_n /= 2;
            if (m_n < 2) m_n = 2;
            need_rebuild = true;
        break;
        case Qt::Key_6:
            m_p++;
            need_rebuild = true;
            break;
        case Qt::Key_7:
            m_p--;
            need_rebuild = true;
            break;
        default:
            QWidget::keyPressEvent(event);
            return;
    }

    if (need_rebuild) {
        updateNodesAndApprox();
    } else if (need_update) {
        recomputeBounds();
        update();
    }
}

void GraphWidget::changeFunction() {
    // вызывается из меню – просто имитируем нажатие '0'
    keyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier));
}
