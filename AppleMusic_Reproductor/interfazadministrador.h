#ifndef INTERFAZADMINISTRADOR_H
#define INTERFAZADMINISTRADOR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include <QDate>
#include <QMap>
#include <QDialog>
#include <QTimer>
#include <QScrollBar>
#include <QWheelEvent>
#include <QKeyEvent>

#include "manejadorcanciones.h"
#include "usuarios.h"

class cframe; // fwd

// =============================================================================
// QScrollArea con auto-ocultado de barras (overlay) tras inactividad
// =============================================================================
class SmartScrollArea : public QScrollArea {
    Q_OBJECT
public:
    explicit SmartScrollArea(QWidget *parent=nullptr)
        : QScrollArea(parent)
        , m_autoH(true), m_autoV(true), m_delayMs(1200)
    {
        setWidgetResizable(true);
        QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &SmartScrollArea::showBarsTemporarily);
        connect(verticalScrollBar(),   &QScrollBar::valueChanged, this, &SmartScrollArea::showBarsTemporarily);
        connect(&m_timer, &QTimer::timeout, this, &SmartScrollArea::hideBars);
        m_timer.setSingleShot(true);
    }

    void setAutoHide(bool horizontal, bool vertical) {
        m_autoH = horizontal; m_autoV = vertical;
        if (m_autoH) QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        if (m_autoV) QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    void setAutoHideDelay(int ms) { m_delayMs = ms; }

public slots:
    void showBarsTemporarily() {
        if (m_autoH) QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        if (m_autoV) QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_timer.start(m_delayMs);
    }

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
    void hideBars() {
        if (m_autoH) QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        if (m_autoV) QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

private:
    bool  m_autoH, m_autoV;
    int   m_delayMs;
    QTimer m_timer;
};

// =============================================================================

class InterfazAdministrador : public QWidget
{
    Q_OBJECT
public:
    explicit InterfazAdministrador(QWidget *parent = nullptr);

    void cargarEnPagina(QWidget *pagina);
    void setNombreArtista(const QString &nombre);
    void setPaginaPrincipal(cframe *pagina);

private slots:
    // Menú izquierdo
    void slotHome();
    void slotMisCanciones();
    void slotMostrarPerfil();           // header clicable → perfil
    void slotAgregarCancion();
    void slotEditarEliminarCancion();
    void slotCrearAlbum();
    void slotCrearEP();
    void slotVerEstadisticas();
    void slotCerrarSesion();

    // Agregar/Editar canción
    void slotTipoCancionCambio(const QString &texto);
    void slotGuardarCancionNueva();
    void slotGuardarCancionEditada();

    // Editar/Eliminar
    void slotAccionEditarCancion();
    void slotAccionEliminarCancion();

private:
    // ===== Estado =====
    QString nombreArtistaLogueado;
    QWidget *zonaCentral = nullptr;   // widget interno que cuelga del scroll vertical
    QWidget *zonaDerecha = nullptr;
    cframe  *paginaPrincipal = nullptr;

    // ---- Header del menú (para refrescar nombre/foto) ----
    QPushButton *hdrBtn = nullptr;
    QLabel *hdrAvatar = nullptr;
    QLabel *hdrNombre = nullptr;

    // ---- Agregar/Editar canción (widgets) ----
    QLineEdit *leTitulo = nullptr;
    QComboBox *cbGenero = nullptr;
    QComboBox *cbCategoria = nullptr;
    QLineEdit *leDuracion = nullptr;
    QComboBox *cbTipo = nullptr;
    QTextEdit *teDescripcion = nullptr;
    QLabel *lblPortada = nullptr;
    QLabel *lblAudio = nullptr;
    QPushButton *btnCargarPortada = nullptr;
    QPushButton *btnCargarAudio = nullptr;
    QPushButton *btnGuardar = nullptr;
    QWidget *seccionColeccion = nullptr;
    QComboBox *cbSeleccionColeccion = nullptr;
    QString rutaPortadaSeleccionada;
    QString rutaAudioSeleccionada;

    // ---- Crear Álbum/EP ----
    QString rutaImagenColeccionTmp;

    // ---- Editar/Eliminar ----
    QListWidget *listaCanciones = nullptr;
    qint64 idCancionEnEdicion = 0;

    // ---- Perfil ----
    QLabel *lblImagenPerfil = nullptr;
    QString rutaImagenPerfilTmp;

    // ===== Construcción UI =====
    QWidget* crearMenuIzquierdo();
    QWidget* crearZonaCentral();
    QWidget* crearZonaDerecha();

    // ===== Formularios / vistas =====
    void limpiarZonaCentral();
    void limpiarResiduosEdicionGlobal();

    // Home
    void construirHome();
    void construirSeccionSingles(QVBoxLayout *layout);                                           // Singles: scroll horizontal, 3 por columna
    void construirSeccionColecciones(QVBoxLayout *layout, const QString &titulo, const QString &archivoColecciones); // Álbum/EP: 1 por columna
    void abrirDialogoColeccion(const QString &archivoColecciones, const QString &nombreColeccion);

    // Mis canciones
    void construirMisCancionesLista();


    // Formularios existentes
    void mostrarFormularioAgregar(bool limpiar = true);
    void mostrarFormularioCrearAlbumEP(const QString &tipo);
    void mostrarListaCancionesEditable();
    void mostrarFormularioEditarPerfil();        // edición
    void mostrarPerfilArtista();                  // solo lectura
    void mostrarFormularioEditarCancion(const Cancion &c);  // ⬅️ **DECLARADA**

    // ===== Álbum/EP =====
    struct Coleccion {
        QString nombre, descripcion, rutaImagen, genero;
        QDate   fecha;
        bool    activo = true;
    };
    QList<Coleccion> leerColecciones(const QString &ruta) const;
    QStringList cargarNombresAlbumes() const;
    QStringList cargarNombresEPs() const;
    void guardarColeccionEnArchivo(const QString &tipo, const QString &nombre, const QString &descripcion, const QString &rutaImagen);

    // ===== Estadísticas =====
    void refrescarEstadisticasDerecha();
    void construirVistaEstadisticasCentro();

    // Binarios de stats (supuesto simple)
    struct Reproduccion { qint64 cancionId; int userId; qint64 ms; QDate fecha; };
    struct Calificacion { qint64 cancionId; int userId; int estrellas; QDate fecha; };
    QList<Reproduccion> leerReproducciones() const;
    QList<Calificacion> leerCalificaciones() const;

    // Util
    static bool esImagenValida(const QString &ruta);
    QWidget* crearItemListaCancion(const Cancion &c, int alto = 64) const;
    QWidget* crearTarjetaColeccionPequena(const Coleccion &col) const;
    QWidget* crearHeaderPerfilEnMenu();

    // Refrescar header tras cambios en Perfil
    void refreshHeaderPerfil();                    // recarga desde usuarios.dat
    void setHeaderAvatarPreview(const QString&);   // solo vista (sin guardar)

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;
};

#endif // INTERFAZADMINISTRADOR_H

