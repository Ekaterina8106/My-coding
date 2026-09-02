QT += widgets

CONFIG += c++11
QMAKE_CXXFLAGS += -g -W -Wall -Werror -Wfloat-equal -Wunused -Wnon-virtual-dtor -isystem $$[QT_INSTALL_HEADERS]

HEADERS = common_defs.h functions.h newton_multiplies.h hermite_piecewise.h graph_widget.h
SOURCES = functions.cpp newton_multiplies.cpp hermite_piecewise.cpp graph_widget.cpp main.cpp
