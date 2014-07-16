QT += core gui opengl xml
QMAKE_CXXFLAGS += -std=gnu++11
TARGET = cloudshop
TEMPLATE = app

HEADERS += \
    surface/include/*.h \
    tools/include/*.h

SOURCES += \
    surface/src/*.cpp \
    tools/src/*.cpp

FORMS += \
    tools/gui/*.ui

INCLUDEPATH += tools/include surface/include

LIBS += -lQGLViewer -lGL -lGLU -lGLEW -lpng

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
DESTDIR = .
