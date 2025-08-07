#ifndef MANEJADORCANCIONES_H
#define MANEJADORCANCIONES_H

#include "cancion.h"
#include <QList>
#include <QString>

class ManejadorCanciones {
public:
    ManejadorCanciones();

    void agregarCancion(const Cancion &cancion);
    QList<Cancion> obtenerTodas();
    QList<Cancion> obtenerPorArtista(const QString &artista);
    QList<Cancion> obtenerPorTipo(const QString &tipo); // Álbum, EP, Single

private:
    QString rutaArchivo = "canciones.dat";
};

#endif // MANEJADORCANCIONES_H
