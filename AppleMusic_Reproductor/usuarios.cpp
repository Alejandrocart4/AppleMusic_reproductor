#include "usuarios.h"
#include <QCryptographicHash>

// Función para cifrar contraseñas con SHA-256
static QString cifrarContrasenia(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

static bool esHex(const QString &s) {
    for (QChar c : s) { if (!c.isDigit() && (c.toLower() < 'a' || c.toLower() > 'f')) return false; }
    return true;
}

void Usuario::guardar(QDataStream &out) const {
    QString toStore = contrasenia;
    // si parece un SHA-256 (64 hex), no volver a hashear
    if (!(toStore.size() == 64 && esHex(toStore))) {
        toStore = cifrarContrasenia(contrasenia);
    }

    out << id
        << aliasre
        << toStore
        << correoElectronico
        << activo
        << static_cast<int>(tipo)
        << fechaRegistro
        << rutaImagen
        << nombreReal;

    if (tipo == Administrador) {
        out << nombreArtistico << paisOrigen << generoMusical << biografia;
    } else if (tipo == UsuarioComun) {
        out << fechaNacimiento << generoPreferido;
    }
}

// Cargar usuario desde binario
    void Usuario::cargar(QDataStream &in) {
        int tipoInt;
        in >> id
            >> aliasre
            >> contrasenia
            >> correoElectronico
            >> activo
            >> tipoInt
            >> fechaRegistro
            >> rutaImagen
            >> nombreReal;

        tipo = static_cast<TipoUsuario>(tipoInt);

        if (tipo == Administrador) {
            in >> nombreArtistico >> paisOrigen >> generoMusical >> biografia;
        } else if (tipo == UsuarioComun) {
            in >> fechaNacimiento >> generoPreferido;
        }
    }

// Constructor del manejador
ManejadorUsuarios::ManejadorUsuarios(const QString &archivo)
    : rutaArchivo(archivo) {}

// Agrega un nuevo usuario al archivo
void ManejadorUsuarios::agregarUsuario(const Usuario &usuario) {
    QFile archivo(rutaArchivo);
    bool existe = QFile::exists(rutaArchivo);

    if (!archivo.open(existe ? QIODevice::Append : (QIODevice::WriteOnly | QIODevice::Truncate))) {
        qWarning() << "No se pudo abrir el archivo para escritura.";
        return;
    }
    QDataStream out(&archivo);
    usuario.guardar(out);
    archivo.close();
}

// Devuelve todos los usuarios
QList<Usuario> ManejadorUsuarios::obtenerUsuarios() const {
    QList<Usuario> lista;
    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::ReadOnly)) {
        qWarning() << "No se pudo abrir el archivo para lectura.";
        return lista;
    }

    QDataStream in(&archivo);
    while (!in.atEnd()) {
        Usuario u;
        u.cargar(in);
        lista.append(u);
    }

    archivo.close();
    return lista;
}
