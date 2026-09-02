QT += widgets opengl
CONFIG += c++11
QMAKE_CXXFLAGS += -W -Wall -Werror -Wfloat-equal -Wunused -Wnon-virtual-dtor

HEADERS = common_defs.h \
          function_2d.h \
          newton_multiples_2d.h \
          cubic_hermite_piecewise_2d.h \
          graph_widget.h
SOURCES = function_2d.cpp \
          newton_multiples_2d.cpp \
          cubic_hermite_piecewise_2d.cpp \
          graph_widget.cpp \
          main.cpp
