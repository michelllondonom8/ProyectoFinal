QT       += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    catapulta.cpp \
    enemigo.cpp \
    fisica.cpp \
    gladiador.cpp \
    main.cpp \
    mainwindow.cpp \
    nivel.cpp \
    nivel1.cpp \
    nivel2.cpp \
    nivel3.cpp \
    portada.cpp \
    proyectil.cpp

HEADERS += \
    catapulta.h \
    enemigo.h \
    fisica.h \
    gladiador.h \
    mainwindow.h \
    nivel.h \
    nivel1.h \
    nivel2.h \
    nivel3.h \
    portada.h \
    proyectil.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

