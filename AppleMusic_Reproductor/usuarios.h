#ifndef USUARIOS_H
#define USUARIOS_H

#include <QString>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QDebug>

// Enumerador para tipo de usuario
enum TipoUsuario {
    UsuarioComun = 0,
    Administrador = 1
};

struct Usuario {
    QString aliasre;
    QString contrasenia;
    bool activo;
    TipoUsuario tipo;

    // Campos comunes a ambos
    QDate fechaRegistro;

    // Solo si es Administrador
    QString nombreArtistico;
    QString nombreReal;
    QString paisOrigen;
    QString generoMusical;
    QString biografia;
    QString rutaImagen;

    // Solo si es Usuario común
    QString NombreUsuario;
    QDate fechaNacimiento;
    QString generoPreferido;

    void guardar(QDataStream &out) const;
    void cargar(QDataStream &in);
};

class ManejadorUsuarios {
public:
    ManejadorUsuarios(const QString &archivo = "usuarios.dat");

    void agregarUsuario(const Usuario &usuario);
    QList<Usuario> obtenerUsuarios();

private:
    QString rutaArchivo;
};

#endif // USUARIOS_H
