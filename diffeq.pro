CONFIG += release warn_all
QT += widgets
QMAKE_CXXFLAGS += -Wextra -Werror

SOURCES += window.cpp math.cpp main.cpp
HEADERS += window.hpp math.hpp
