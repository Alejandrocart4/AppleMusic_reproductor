#include "manejadorcanciones.h"
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
