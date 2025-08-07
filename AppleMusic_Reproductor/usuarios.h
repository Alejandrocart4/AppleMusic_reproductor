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
    int id;                    // ID único del usuario
    QString aliasre;           // Alias o nombre de usuario
    QString contrasenia;       // Contraseña (se guardará cifrada)
    QString correoElectronico; // Nuevo campo
    bool activo;
    TipoUsuario tipo;
    QDate fechaRegistro;
    QString rutaImagen;        // Imagen/avatar para cualquier tipo de usuario

    // Datos comunes
    QString nombreReal;        // Para administradores y también usuarios comunes

    // Solo si es Administrador
    QString nombreArtistico;
    QString paisOrigen;
    QString generoMusical;
    QString biografia;

    // Solo si es Usuario común
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
