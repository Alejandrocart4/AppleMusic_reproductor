#ifndef CANCION_H
#define CANCION_H

#include <QString>
#include <QDate>
#include <QDataStream>

struct Cancion {
    qint64 id;
    QString titulo;
    QString artista;
    QString genero;
    QString categoria;
    QString duracion;
    QString descripcion;
    QString portada;       // Ruta de imagen de portada
    QString rutaArchivo;   // Ruta del archivo de audio
    QString tipo;          // Single, EP, Álbum
    QDate fechaCarga;
    bool activo;

    // Guardar en binario
    void guardar(QDataStream &out) const {
        out << id
            << titulo
            << artista
            << genero
            << categoria
            << duracion
            << descripcion
            << portada
            << rutaArchivo
            << tipo
            << fechaCarga
            << activo;
    }

    // Cargar desde binario
    void cargar(QDataStream &in) {
        in >> id
            >> titulo
            >> artista
            >> genero
            >> categoria
            >> duracion
            >> descripcion
            >> portada
            >> rutaArchivo
            >> tipo
            >> fechaCarga
            >> activo;
    }
};

#endif // CANCION_H
