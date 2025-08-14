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
    QString coleccion;     // Nombre del Álbum o EP (vacío si Single)  << NUEVO
    QDate fechaCarga;
    bool activo;

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
            << coleccion        // << NUEVO en binario
            << fechaCarga
            << activo;
    }

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
            >> coleccion        // << NUEVO en binario
            >> fechaCarga
            >> activo;
    }
};

#endif // CANCION_H
