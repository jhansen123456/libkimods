include(Libki.pro)
TARGET = welcome-tests
CONFIG += testcase console
QT += testlib
SOURCES -= main.cpp
SOURCES += tests/welcome_tests.cpp
