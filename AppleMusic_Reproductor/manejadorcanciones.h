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
    bool renombrarArtista(const QString &oldName, const QString &newName);

    //***************************************************************************
    //***************************************************************************
    //***************************************************************************
    // ARCHIVOS INDEXADOS — utilidades de actualización — BEGIN

    bool actualizarDuracion(qint64 songId, const QString &duracion);

    // ARCHIVOS INDEXADOS — utilidades de actualización — END
    //***************************************************************************
    //***************************************************************************
    //***************************************************************************


private:
    QString rutaArchivo = "canciones.dat";
};

#endif // MANEJADORCANCIONES_H
