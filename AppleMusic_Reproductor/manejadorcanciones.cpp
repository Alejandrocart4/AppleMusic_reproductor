#include "manejadorcanciones.h"
#include <QFile>
#include <QDataStream>

ManejadorCanciones::ManejadorCanciones() {}

void ManejadorCanciones::agregarCancion(const Cancion &cancion) {
    QFile archivo(rutaArchivo);
    archivo.open(QIODevice::Append);
    QDataStream out(&archivo);
    cancion.guardar(out);
    archivo.close();
}

QList<Cancion> ManejadorCanciones::obtenerTodas() {
    QList<Cancion> lista;
    QFile archivo(rutaArchivo);
    if (!archivo.exists()) return lista;

    if (archivo.open(QIODevice::ReadOnly)) {
        QDataStream in(&archivo);
        while (!in.atEnd()) {
            Cancion c;
            c.cargar(in);
            lista.append(c);
        }
        archivo.close();
    }
    return lista;
}

QList<Cancion> ManejadorCanciones::obtenerPorArtista(const QString &artista) {
    QList<Cancion> resultado;
    QList<Cancion> todas = obtenerTodas();
    for (const Cancion &c : todas) {
        if (c.artista == artista && c.activo)
            resultado.append(c);
    }
    return resultado;
}

QList<Cancion> ManejadorCanciones::obtenerPorTipo(const QString &tipo) {
    QList<Cancion> resultado;
    QList<Cancion> todas = obtenerTodas();
    for (const Cancion &c : todas) {
        if (c.tipo == tipo && c.activo)
            resultado.append(c);
    }
    return resultado;
}

bool ManejadorCanciones::renombrarArtista(const QString &oldName, const QString &newName) {
    QList<Cancion> canciones = obtenerTodas();
    bool cambiado = false;

    for (Cancion &c : canciones) {
        if (c.artista == oldName) {
            c.artista = newName;
            cambiado = true;
        }
    }

    if (cambiado) {
        QFile archivo(rutaArchivo);
        if (!archivo.open(QIODevice::WriteOnly))
            return false;
        QDataStream out(&archivo);
        for (const Cancion &c : canciones)
            c.guardar(out);
        archivo.close();
    }

    return cambiado;
}

//***************************************************************************
//***************************************************************************
//***************************************************************************
// ARCHIVOS INDEXADOS — utilidades de actualización — BEGIN

bool ManejadorCanciones::actualizarDuracion(qint64 songId, const QString &duracion) {
    QList<Cancion> canciones = obtenerTodas();
    bool cambiado = false;
    for (Cancion &c : canciones) {
        if (c.id == songId) {
            if (c.duracion != duracion && !duracion.isEmpty()) {
                c.duracion = duracion;
                cambiado = true;
            }
            break;
        }
    }
    if (!cambiado) return false;

    QFile archivo("canciones.dat");
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    QDataStream out(&archivo);
    for (const Cancion &c : canciones) c.guardar(out);
    archivo.close();
    return true;
}

// ARCHIVOS INDEXADOS — utilidades de actualización — END
//***************************************************************************
//***************************************************************************
//***************************************************************************


