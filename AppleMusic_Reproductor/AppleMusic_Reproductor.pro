QT += core gui widgets multimedia multimediawidgets
QT += charts

CONFIG += c++17


SOURCES += \
    artista.cpp \
    cancion.cpp \
    interfaz_usuario.cpp \
    interfazadministrador.cpp \
    main.cpp \
    cframe.cpp \
    manejadorcanciones.cpp \
    usuarios.cpp

HEADERS += \
    artista.h \
    cancion.h \
    cframe.h \
    interfaz_usuario.h \
    interfazadministrador.h \
    manejadorcanciones.h \
    usuarios.h

FORMS += \
    cframe.ui

# Para usar archivos de audio (como mp3) correctamente
RESOURCES += \
    imagnes.qrc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Permite que el ejecutable encuentre las bibliotecas en tiempo de ejecución
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs -lQtMultimedia
unix:!macx: LIBS += -lQt5Multimedia

# Default rules for deployment.
target.path = .
INSTALLS += target
