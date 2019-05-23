#-------------------------------------------------
#
# Project created by QtCreator 2018-05-07T15:43:02
#
#-------------------------------------------------

QT       += core gui axcontainer xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = languageAutoCreate
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    excelhandle.cpp \
    xmlhandle.cpp

HEADERS  += widget.h \
    excelhandle.h \
    xmlhandle.h

FORMS    += widget.ui
