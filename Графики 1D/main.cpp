#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <cstdio>
#include "graph_widget.h"
#include "common_defs.h"

#define DEFAULT_A (-5.0)
#define DEFAULT_B (5.0)
#define DEFAULT_N (10)
#define DEFAULT_K (0)

static int parse_command_line(int argc, char **argv,
                              double *pa, double *pb, int *pn, int *pk) {
    if (argc == 1) return 0; // использовать значения по умолчанию

    if (argc != 5) {
        fprintf(stderr, "Usage: %s a b n k\n", argv[0]);
        return -1;
    }

    if (sscanf(argv[1], "%lf", pa) != 1) {
        fprintf(stderr, "Invalid a\n");
        return -1;
    }
    if (sscanf(argv[2], "%lf", pb) != 1) {
        fprintf(stderr, "Invalid b\n");
        return -1;
    }
    if (sscanf(argv[3], "%d", pn) != 1 || *pn < 2) {
        fprintf(stderr, "Invalid n (must be >=2)\n");
        return -1;
    }
    if (sscanf(argv[4], "%d", pk) != 1 || *pk < 0 || *pk > 6) {
        fprintf(stderr, "Invalid k (must be 0..6)\n");
        return -1;
    }

    double len = *pb - *pa;
    if (len < EPSILON_FOR_COMPARE) {
        fprintf(stderr, "Segment too short\n");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    double a = DEFAULT_A;
    double b = DEFAULT_B;
    int n = DEFAULT_N;
    int k = DEFAULT_K;

    if (parse_command_line(argc, argv, &a, &b, &n, &k) < 0)
        return -1;

    QApplication app(argc, argv);

    QMainWindow mainWindow;
    GraphWidget *graphWidget = new GraphWidget(&mainWindow, a, b, n, k);

    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *fileMenu = menuBar->addMenu("&File");
    QMenu *funcMenu = menuBar->addMenu("F&unction");

    QAction *changeFuncAction = funcMenu->addAction("&Change function");
    changeFuncAction->setShortcut(QString("Ctrl+C"));
    // ИСПРАВЛЕНО: правильное имя метода
    QObject::connect(changeFuncAction, &QAction::triggered, graphWidget, &GraphWidget::changeFunction);

    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QString("Ctrl+X"));
    QObject::connect(exitAction, &QAction::triggered, &mainWindow, &QMainWindow::close);

    mainWindow.setCentralWidget(graphWidget);
    mainWindow.setWindowTitle("Interpolation Demo");
    mainWindow.show();

    return app.exec();
}
