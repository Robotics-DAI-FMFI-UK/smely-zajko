# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Debug/GNU-Linux-x86
TARGET = gen_train_data
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += debug 
QT = core gui network opengl phonon qt3support sql svg xml webkit
SOURCES += main.cpp
HEADERS +=
FORMS +=
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Debug/GNU-Linux-x86
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += /usr/include /usr/include/opencv /usr/include/qt4/Qt /usr/include/qt4 ../nn_lib 
LIBS += -lcv -lcvaux -lcxcore -lhighgui -lml ../nn_lib/dist/Debug/GNU-Linux-x86/libnn_lib.a -lfann -lqt-mt -lQtCore -lQtGui  
