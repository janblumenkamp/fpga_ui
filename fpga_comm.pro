#-------------------------------------------------
#
# Project created by QtCreator 2015-10-09T12:39:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fpga_comm
TEMPLATE = app

CONFIG += serialport

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
