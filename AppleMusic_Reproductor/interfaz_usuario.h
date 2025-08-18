#ifndef INTERFAZ_USUARIO_H
#define INTERFAZ_USUARIO_H

#include <QWidget>
#include <QPointer>
#include <QScrollArea>
#include <QTimer>
#include <QList>
#include <QSet>
#include <QDate>
#include <QScrollBar>
#include <QKeyEvent>


class QLabel;
class QPushButton;
class QSlider;
class QMediaPlayer;
class QAudioOutput;
class QMouseEvent;

class cframe;
struct Cancion;

// ================== Scroll overlay auto-oculto (horizontal) ==================
class SmartScrollAreaUser : public QScrollArea {
    Q_OBJECT
public:
    explicit SmartScrollAreaUser(QWidget *parent=nullptr)
        : QScrollArea(parent), m_autoH(true), m_autoV(false), m_delay(1200) {
        setWidgetResizable(true);
        QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &SmartScrollAreaUser::showBarsTemporarily);
        connect(verticalScrollBar(),   &QScrollBar::valueChanged, this, &SmartScrollAreaUser::showBarsTemporarily);
        timer.setSingleShot(true);
        connect(&timer, &QTimer::timeout, this, &SmartScrollAreaUser::hideBars);
    }
    void setAutoHide(bool h, bool v){ m_autoH=h; m_autoV=v; hideBars(); }
    void setAutoHideDelay(int ms){ m_delay=ms; }

protected:
    void wheelEvent(QWheelEvent *e) override { showBarsTemporarily(); QScrollArea::wheelEvent(e); }
    void mousePressEvent(QMouseEvent *e) override { showBarsTemporarily(); QScrollArea::mousePressEvent(e); }
    void mouseMoveEvent(QMouseEvent *e) override { if (e->buttons()!=Qt::NoButton) showBarsTemporarily(); QScrollArea::mouseMoveEvent(e); }
    void keyPressEvent(QKeyEvent *e) override {
        switch (e->key()) {
        case Qt::Key_Left: case Qt::Key_Right: case Qt::Key_Up: case Qt::Key_Down:
        case Qt::Key_PageUp: case Qt::Key_PageDown: case Qt::Key_Home: case Qt::Key_End:
            showBarsTemporarily(); break;
        default: break;
        }
        QScrollArea::keyPressEvent(e);
    }

private slots:
    void showBarsTemporarily() {
        if (m_autoH) QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        if (m_autoV) QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        timer.start(m_delay);
    }
    void hideBars() {
        if (m_autoH) QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        if (m_autoV) QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

private:
    bool m_autoH, m_autoV; int m_delay; QTimer timer;
};

// ============== Filtro sencillo para doble clic en cualquier widget ==========
class DblClickFilter : public QObject {
    Q_OBJECT
public:
    explicit DblClickFilter(std::function<void()> onDbl, QObject *parent=nullptr)
        : QObject(parent), cb(std::move(onDbl)) {}
protected:
    bool eventFilter(QObject* obj, QEvent* ev) override {
        if (ev->type()==QEvent::MouseButtonDblClick) { if (cb) cb(); return true; }
        return QObject::eventFilter(obj, ev);
    }
private:
    std::function<void()> cb;
};


// ================================ InterfazUsuario ============================
class InterfazUsuario : public QObject {
    Q_OBJECT
public:
    explicit InterfazUsuario(cframe *ventana);
    void cargarEnPagina(QWidget *pagina);

    // Reproductor (top bar) expuesto por cframe
    QWidget* crearReproductor();

private:
    // Layouts / zonas
    QWidget* crearMenuIzquierdo();
    QWidget* crearZonaCentral();
    QWidget* crearZonaDerecha();

    // HOME (lo que pides)
    void construirHome();

    // Secciones horizontales
    QWidget* crearSeccionHorizontal(const QString &titulo, const QList<QWidget*> &tarjetas);

    // Tarjetas
    QWidget* crearTarjetaCancion(const Cancion &c);                 // doble clic → reproducir single
    QWidget* crearTarjetaArtista(const QString &artista, const QString &portadaHint);
    QWidget* crearTarjetaColeccion(const QString &nombreColeccion, const QString &portada, const QString &tipo);

    // Navegación/acciones
    void abrirArtistaUsuario(const QString &artista);
    void abrirColeccionUsuario(const QString &nombreColeccion, const QString &tipo, const QString &artista);

    // Reproducción
    void reproducirCancion(const QString &ruta);
    void reproducirColeccionCompleta(const QString &coleccion, const QString &tipo, const QString &artista);

    // Imágenes utilitarias
    static QPixmap fitOrCover(const QString &ruta, const QSize &dest, bool cover);
    static QLabel* coverLabel(const QString &ruta, const QSize &sz, bool cover);

    // Datos auxiliares: “Recién escuchados”
    struct Reproduccion { qint64 cancionId; int userId; qint64 ms; QDate fecha; };
    QList<Reproduccion> leerReproducciones() const;

    // Vistas de menú
    void mostrarVistaBiblioteca();
    void mostrarVistaFavoritos();
    void mostrarVistaPlaylists();
    void mostrarVistaDescargas();

private:
    cframe *ventanaPrincipal = nullptr;
    QWidget *zonaCentral = nullptr;

    // Botones laterales
    QPushButton *btnHome = nullptr;
    QPushButton *btnBiblioteca = nullptr;
    QPushButton *btnFavoritos = nullptr;
    QPushButton *btnPlaylists = nullptr;
    QPushButton *btnDescargas = nullptr;

    // Cola de reproducción para álbum/EP
    QList<QString> colaReproduccion;
    int idxCola = -1;
    bool conexionSiguienteHecha = false;
    void conectarAvanceAutomatico();

    // Helpers
    void limpiarZonaCentral();
    QList<Cancion> obtenerCancionesTodas() const;
    QList<Cancion> obtenerCancionesPorArtista(const QString& artista) const;
    QList<Cancion> obtenerCancionesPorColeccion(const QString& col, const QString& artista, const QString& tipo) const;
};

#endif // INTERFAZ_USUARIO_H
