#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T16:42:40
#
#-------------------------------------------------

QT       += core gui
CONFIG += communi c++11
COMMUNI += core model util

QT += widgets

TARGET = color
TEMPLATE = app


SOURCES += main.cpp\
        main_window.cpp \
    draw_area.cpp \
    bot.cpp

HEADERS  += main_window.h \
    draw_area.h \
    settings.h \
    bot.h
