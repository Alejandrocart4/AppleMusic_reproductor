QT += core gui widgets multimedia multimediawidgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    cframe.cpp

HEADERS += \
    cframe.h

FORMS += \
    cframe.ui

# Para usar archivos de audio (como mp3) correctamente
RESOURCES += \
    imagnes.qrc

# Asegúrate de tener una versión moderna de Qt
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Permite que el ejecutable encuentre las bibliotecas en tiempo de ejecución (útil si usas deploys)
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs -lQtMultimedia
unix:!macx: LIBS += -lQt5Multimedia

# Default rules for deployment.
target.path = .
INSTALLS += target
