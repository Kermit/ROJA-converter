#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T08:00:56
#
#-------------------------------------------------

QT       += core network sql
QT       -= gui

TARGET = Roja-Converter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += src/main.cpp \
    src/chmfile.cpp \
    src/config.cpp \
    src/model/stops.cpp \
    src/model/lines.cpp \
    src/model/days.cpp \
    src/model/communes.cpp \
    src/model/routes.cpp \
    src/model/times.cpp \
    src/model/routesdetails.cpp \
    src/model/settings.cpp \
    src/database.cpp

HEADERS += \
    src/chmfile.h \
    src/config.h \
    src/model/stops.h \
    src/model/lines.h \
    src/model/days.h \
    src/model/communes.h \
    src/model/routes.h \
    src/model/times.h \
    src/model/routesdetails.h \
    src/model/settings.h \
    src/database.h
































