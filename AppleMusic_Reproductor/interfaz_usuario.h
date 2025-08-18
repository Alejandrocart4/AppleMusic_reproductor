#ifndef INTERFAZ_USUARIO_H
#define INTERFAZ_USUARIO_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QScrollBar>
#include <QLabel>
#include <QListWidget>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QRadioButton>
#include <QVector>
#include <functional>

#include "cancion.h"
#include "usuarios.h"

class cframe;

// ---------- ScrollArea sin Q_OBJECT (auto-oculta barras) ----------
class SmartScrollAreaUser : public QScrollArea {
public:
    explicit SmartScrollAreaUser(QWidget *parent=nullptr)
        : QScrollArea(parent), m_autoH(true), m_autoV(true), m_delayMs(1200) {
        setWidgetResizable(true);
        setFrameShape(QFrame::NoFrame);
        QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        auto showBarsTemporarily = [this]() {
            if (m_autoH) QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            if (m_autoV) QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            m_timer.start(m_delayMs);
        };
        connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, [showBarsTemporarily](int){ showBarsTemporarily(); });
        connect(verticalScrollBar(),   &QScrollBar::valueChanged, this, [showBarsTemporarily](int){ showBarsTemporarily(); });

        connect(&m_timer, &QTimer::timeout, this, [this]() {
            if (m_autoH) QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            if (m_autoV) QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        });
        m_timer.setSingleShot(true);
    }
    void setAutoHide(bool horizontal, bool vertical) { m_autoH=horizontal; m_autoV=vertical; hideBars(); }
    void setAutoHideDelay(int ms) { m_delayMs = ms; }
    void hideBars() {
        if (m_autoH) setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        if (m_autoV) setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
protected:
    void wheelEvent(QWheelEvent *e) override { if (m_autoV) setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); m_timer.start(m_delayMs); QScrollArea::wheelEvent(e); }
    void keyPressEvent(QKeyEvent *e) override {
        switch (e->key()) {
        case Qt::Key_Left: case Qt::Key_Right: case Qt::Key_Up: case Qt::Key_Down:
        case Qt::Key_PageUp: case Qt::Key_PageDown: case Qt::Key_Home: case Qt::Key_End:
            if (m_autoV) setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded), m_timer.start(m_delayMs);
            break;
        default: break;
        }
        QScrollArea::keyPressEvent(e);
    }
private:
    bool  m_autoH, m_autoV; int m_delayMs; QTimer m_timer;
};

// ---------- Filtro doble clic ----------
class DblClickFilter : public QObject {
public:
    explicit DblClickFilter(std::function<void()> onDbl, QObject *parent=nullptr)
        : QObject(parent), cb(std::move(onDbl)) {}
protected:
    bool eventFilter(QObject *obj, QEvent *ev) override {
        if (ev->type() == QEvent::MouseButtonDblClick) { if (cb) cb(); return true; }
        return QObject::eventFilter(obj, ev);
    }
private:
    std::function<void()> cb;
};

// ---------- InterfazUsuario ----------
class InterfazUsuario {
public:
    InterfazUsuario(cframe *ventana, const Usuario &usuario);
    void setUsuario(const Usuario &usuario);
    void cargarEnPagina(QWidget *pagina);

private:
    // estado
    cframe *ventanaPrincipal = nullptr;
    Usuario usuarioActual;

    // contenedores centro
    SmartScrollAreaUser *zonaCentralScroll = nullptr; // scroll vertical de toda la zona central
    QWidget *zonaCentralWidget = nullptr;             // contenido interno del scroll

    // reproductor/menus
    QWidget* crearReproductor();
    QWidget* crearMenuIzquierdo();
    QWidget* crearZonaCentral();  // Home por defecto
    QWidget* crearZonaDerecha();

    // secciones home
    void construirHome();
    QWidget* crearSeccionHorizontal(const QString &titulo, const QList<QWidget*> &tarjetas);
    QWidget* crearTarjetaCancion(const Cancion &c);
    QWidget* crearTarjetaColeccion(const QString &nombreColeccion, const QString &portada, const QString &tipo);
    void abrirColeccionUsuario(const QString &nombreColeccion, const QString &tipo, const QString &artista);
    void abrirArtistaUsuario(const QString &artista);

    // navegación / limpieza / búsqueda
    void limpiarCentro();
    void mostrarPerfil();
    void mostrarResultadosBusqueda(const QString &q);
    void mostrarDescargas();

    // utilidades
    static QPixmap fitOrCover(const QString &ruta, const QSize &dest, bool cover);
    static QLabel* coverLabel(const QString &ruta, const QSize &sz, bool cover);
    void reproducirCancion(const QString &ruta);
    void reproducirCancionMeta(const Cancion &c);
    void actualizarPanelDerecha(const Cancion &c);
    Cancion buscarPorRuta(const QString &ruta) const;
    Cancion buscarPorId(qint64 id) const;
    static QString mmssFromMs(qint64 ms);
    void asegurarDuracion(Cancion c, std::function<void(const QString&)> aplicarUI);


    // recientes / rating (binarios simples)
    struct Reproduccion { qint64 cancionId; int userId; qint64 ms; QDate fecha; };
    struct Calificacion { qint64 cancionId; int userId; int estrellas; QDate fecha; };

    QList<Reproduccion> leerReproducciones() const;
    void guardarReproduccion(qint64 songId, qint64 ms = 0);
    int  leerCalificacion(qint64 songId) const; // 0 si no existe
    void guardarCalificacion(qint64 songId, int estrellas);

    // cache
    QList<Cancion> cacheCanciones;

    //***************************************************************************
    //***************************************************************************
    //***************************************************************************
    // ARCHIVOS INDEXADOS (DECLARACIONES) — BEGIN

    struct Playlist {
        qint64 id = 0;
        QString nombre;
        QString portada;   // ruta de imagen
        bool activa = true;
        void guardar(QDataStream &out) const { out << id << nombre << portada << activa; }
        void cargar(QDataStream &in)       { in  >> id >> nombre >> portada >> activa; }
    };

    struct PlaylistItem {
        qint64 playlistId = 0;
        qint64 cancionId  = 0;
        void guardar(QDataStream &out) const { out << playlistId << cancionId; }
        void cargar(QDataStream &in)       { in  >> playlistId >> cancionId; }
    };

    // Rutas por usuario (un archivo por tipo = “indexado” por ID)
    QString pathFav() const { return QString("favoritos_%1.dat").arg(usuarioActual.id); }
    QString pathPL()  const { return QString("playlists_%1.dat").arg(usuarioActual.id); }
    QString pathPLI() const { return QString("playlist_items_%1.dat").arg(usuarioActual.id); }

    // ARCHIVOS INDEXADOS (DECLARACIONES) — BEGIN (Descargas)

    QString pathDOWN() const { return QString("descargas_%1.dat").arg(usuarioActual.id); }
    QSet<qint64> leerDescargas() const;
    void guardarDescargas(const QSet<qint64>&) const;

    // ARCHIVOS INDEXADOS (DECLARACIONES) — END (Descargas)

    // IO binario
    QSet<qint64>        leerFavoritos() const;
    void                guardarFavoritos(const QSet<qint64>&) const;
    QList<Playlist>     leerPlaylists() const;
    void                guardarPlaylists(const QList<Playlist>&) const;
    QList<PlaylistItem> leerPlaylistItems() const;
    void                guardarPlaylistItems(const QList<PlaylistItem>&) const;

    // UI helpers
    void mostrarFavoritos();
    void mostrarPlaylists();
    void abrirPlaylist(const Playlist &p);
    void dialogCrearPlaylist(std::function<void(const Playlist&)> cb = {});
    void agregarCancionAPlaylist(qint64 playlistId, qint64 cancionId);
    void toggleFavorito(qint64 cancionId, QPushButton *btnHeart = nullptr);
    QMenu* menuAddToPlaylist(qint64 cancionId);

    // ARCHIVOS INDEXADOS (DECLARACIONES) — END
    //***************************************************************************
    //***************************************************************************
    //***************************************************************************

};

#endif // INTERFAZ_USUARIO_H

