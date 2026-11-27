# Proyecto de pruebas de la logica. Va en una carpeta "pruebas" junto a "codigo".
QT       += core
QT       -= gui
CONFIG   += console c++17
CONFIG   -= app_bundle
TEMPLATE  = app
TARGET    = pruebas
INCLUDEPATH += ../codigo

HEADERS += pruebas.h

SOURCES += main_pruebas.cpp pruebas.cpp \
    prueba_objetoCircular.cpp prueba_zona.cpp prueba_obstaculo.cpp prueba_proyectil.cpp \
    prueba_capitan.cpp prueba_capitanN1.cpp prueba_capitanN2.cpp \
    prueba_efecto.cpp prueba_esbirro.cpp prueba_muroPerseguidor.cpp prueba_jefe.cpp \
    prueba_nivel1.cpp prueba_nivel2.cpp \
    ../codigo/objetoCircular.cpp ../codigo/zona.cpp ../codigo/obstaculo.cpp \
    ../codigo/proyectil.cpp ../codigo/capitan.cpp ../codigo/capitanN1.cpp \
    ../codigo/capitanN2.cpp ../codigo/efecto.cpp ../codigo/esbirro.cpp \
    ../codigo/muroPerseguidor.cpp ../codigo/jefe.cpp \
    ../codigo/nivel1.cpp ../codigo/nivel2.cpp
