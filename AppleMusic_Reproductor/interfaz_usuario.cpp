#include "interfaz_usuario.h"
#include "cframe.h"
#include "ui_cframe.h"
#include "manejadorcanciones.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QScrollArea>
#include <QRadioButton>
#include <QPixmap>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QDateTime>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QRandomGenerator>
#include <QtAlgorithms>
#include <QSpacerItem>
#include <QFile>
#include <QDataStream>
#include <QSet>
#include <QStackedWidget>
#include <QDialog>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QSet>
#include <QDateTime>
#include <QObject>
#include <QCursor>
#include <QIcon>

// ===== Persistencia simple por IDs =====

struct PlaylistInfo {
    qint64  id;
    QString nombre;
    QString portada; // ruta de imagen (opcional)
};

static QString pathPlaylists()     { return "playlists.dat"; }        // define tu propia ruta si ya la tienes
static QString pathPlaylistItems() { return "playlist_items.dat"; }   // pares (playlistId, songId)

// Lee las playlists guardadas (id, nombre, portada)
static QVector<PlaylistInfo> obtenerPlaylists() {
    QVector<PlaylistInfo> v;
    QFile f(pathPlaylists());
    if (!f.open(QIODevice::ReadOnly)) return v;

    QDataStream ds(&f);
    ds.setVersion(QDataStream::Qt_6_5);

    while (!ds.atEnd()) {
        PlaylistInfo p;
        ds >> p.id >> p.nombre >> p.portada;
        v.push_back(p);
    }
    return v;
}

// ¿Ya existe la canción en la playlist?
static bool existeEnPlaylist(qint64 playlistId, qint64 songId) {
    QFile f(pathPlaylistItems());
    if (!f.open(QIODevice::ReadOnly)) return false;

    QDataStream ds(&f);
    ds.setVersion(QDataStream::Qt_6_5);

    qint64 pid, sid;
    while (!ds.atEnd()) {
        ds >> pid >> sid;
        if (pid == playlistId && sid == songId) return true;
    }
    return false;
}

// Agrega UNA sola canción (songId) a UNA playlist (playlistId)
static bool agregarCancionAPlaylist(qint64 playlistId, qint64 songId) {
    if (existeEnPlaylist(playlistId, songId)) return false;

    QFile f(pathPlaylistItems());
    if (!f.open(QIODevice::Append)) return false;

    QDataStream ds(&f);
    ds.setVersion(QDataStream::Qt_6_5);
    ds << playlistId << songId;
    return true;
}

static QString pathFavoritos()  { return "favoritos.dat"; }
static QString pathDescargas()  { return "descargas.dat"; }

static QSet<qint64> leerSetIds(const QString &path) {
    QSet<qint64> s;
    QFile f(path);
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream ds(&f);
        ds.setVersion(QDataStream::Qt_6_5);
        qint64 id;
        while (!ds.atEnd()) { ds >> id; s.insert(id); }
    }
    return s;
}
static void guardarSetIds(const QString &path, const QSet<qint64> &s) {
    QFile f(path);
    if (f.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QDataStream ds(&f);
        ds.setVersion(QDataStream::Qt_6_5);
        for (qint64 id : s) ds << id;
    }
}

static QSet<qint64> leerFavoritos()  { return leerSetIds(pathFavoritos()); }
static void guardarFavoritos(const QSet<qint64> &s) { guardarSetIds(pathFavoritos(), s); }

static QSet<qint64> leerDescargas()  { return leerSetIds(pathDescargas()); }
static void guardarDescargas(const QSet<qint64> &s) { guardarSetIds(pathDescargas(), s); }

// ===== Acciones por id =====
static void toggleFavorito(qint64 songId, QPushButton *btn = nullptr) {
    auto favs = leerFavoritos();
    const bool on = !favs.contains(songId);
    if (on) favs.insert(songId); else favs.remove(songId);
    guardarFavoritos(favs);
    if (btn) {
        btn->setStyleSheet(QString(
                               "QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}"
                               ).arg(on ? "#E74C3C" : "#bbb"));
    }
}

// Construye el menú “Agregar a playlist” para UNA pista
static QMenu* menuAddToPlaylist(qint64 songId) {
    QMenu *m = new QMenu;
    const auto listas = obtenerPlaylists(); // <-- ya existe

    for (const auto &p : listas) {
        QAction *a = m->addAction(p.nombre);
        QObject::connect(a, &QAction::triggered, m, [=](){
            agregarCancionAPlaylist(p.id, songId); // SOLO esa canción
            QMessageBox::information(nullptr, "Playlist",
                                     "Añadida a \"" + p.nombre + "\"");
        });
    }
    return m;
}


// ================== Helpers gráficos ==================
QPixmap InterfazUsuario::fitOrCover(const QString &ruta, const QSize &dest, bool cover) {
    QPixmap px;
    if (!ruta.isEmpty()) px = QPixmap(ruta);
    if (px.isNull()) { QPixmap dummy(dest); dummy.fill(Qt::white); return dummy; }
    return px.scaled(dest, cover? Qt::KeepAspectRatioByExpanding : Qt::KeepAspectRatio,
                     Qt::SmoothTransformation);
}
QLabel* InterfazUsuario::coverLabel(const QString &ruta, const QSize &sz, bool cover) {
    QLabel *img = new QLabel; img->setFixedSize(sz); img->setAlignment(Qt::AlignCenter);
    img->setStyleSheet("background:#fff; border-radius:10px;");
    img->setPixmap(fitOrCover(ruta, sz, cover));
    return img;
}

// ================== ctor / estado ==================
InterfazUsuario::InterfazUsuario(cframe *ventana, const Usuario &usuario)
    : ventanaPrincipal(ventana), usuarioActual(usuario) {}

void InterfazUsuario::setUsuario(const Usuario &usuario) { usuarioActual = usuario; }

// ================== Page builder ==================
void InterfazUsuario::cargarEnPagina(QWidget *pagina) {
    // LIMPIAR por completo la página
    if (auto *oldLay = pagina->layout()) { delete oldLay; }
    const QString style = "background-color: #121212; color: white;";
    pagina->setStyleSheet(style);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(pagina);
    // Laterales iguales; solo bajo el reproductor con un espacio extra arriba
    layoutPrincipal->setContentsMargins(20, 12, 15, 12);
    layoutPrincipal->setSpacing(10);

    // ↓↓↓ Espacio superior extra para que el reproductor no quede pegado arriba
    const int GAP_SUPERIOR_REPRODUCTOR = 28; // ajústalo si lo quieres más/menos abajo
    layoutPrincipal->addSpacing(GAP_SUPERIOR_REPRODUCTOR);

    // Reproductor (arriba, pero ya no pegado al borde superior)
    layoutPrincipal->addWidget(crearReproductor());

    // Cuerpo 3 columnas
    QHBoxLayout *cuerpo = new QHBoxLayout;
    cuerpo->setContentsMargins(0,0,0,0);
    cuerpo->setSpacing(12);

    cuerpo->addWidget(crearMenuIzquierdo(), 1);
    cuerpo->addWidget(crearZonaCentral(), 4);  // scroll vertical ocupa el centro
    cuerpo->addWidget(crearZonaDerecha(), 1);

    layoutPrincipal->addLayout(cuerpo);
    pagina->setLayout(layoutPrincipal);
}

// ================== Reproductor (top) ==================
QWidget* InterfazUsuario::crearReproductor() {
    QWidget *w = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->setContentsMargins(0,0,0,0);
    lay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    lay->addWidget(ventanaPrincipal->getBtnAbrir());
    lay->addWidget(ventanaPrincipal->getBtnPause());
    lay->addWidget(ventanaPrincipal->getBtnPlay());
    lay->addWidget(ventanaPrincipal->getBtnStop());
    lay->addWidget(ventanaPrincipal->getBtnMute());

    QSlider *volumen = ventanaPrincipal->getSliderVolumen();
    volumen->setFixedSize(120, 12);
    lay->addWidget(volumen);

    lay->addWidget(ventanaPrincipal->getSliderProgreso());
    lay->addWidget(ventanaPrincipal->getLabelTiempo());

    lay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    return w;
}

// ================== Menú izquierdo ==================
QWidget* InterfazUsuario::crearMenuIzquierdo() {
    QWidget *menu = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(menu);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(8);

    QPushButton *btnPerfil = new QPushButton("👤 Mi perfil");
    btnPerfil->setStyleSheet("text-align:left; padding:6px; border-radius:6px; font-weight:600;");
    lay->addWidget(btnPerfil);

    QLineEdit *buscador = new QLineEdit;
    buscador->setPlaceholderText("🔎 Buscar...");
    buscador->setStyleSheet("background-color: white; color: black;");
    lay->addWidget(buscador);

    static QTimer *buscadorTimer = nullptr;
    if (!buscadorTimer) {
        buscadorTimer = new QTimer(buscador);
        buscadorTimer->setSingleShot(true);
        buscadorTimer->setInterval(160); // ajusta si quieres más/menos sensible
    }

    // Cuando cambia el texto, reiniciamos el temporizador
    QObject::connect(buscador, &QLineEdit::textChanged, buscador, [=](const QString&){
        buscadorTimer->stop();
        buscadorTimer->start();
    });

    // Al vencer el temporizador, mostramos resultados o Home si está vacío
    QObject::connect(buscadorTimer, &QTimer::timeout, buscador, [=](){
        const QString t = buscador->text().trimmed();
        if (t.isEmpty()) {
            construirHome();              // vuelve a Home si no hay texto
        } else {
            mostrarResultadosBusqueda(t); // resultados en vivo
        }
    });

    QObject::connect(btnPerfil, &QPushButton::clicked, [=](){
        mostrarPerfil();
    });

    QPushButton *btnHome       = new QPushButton("🏠 Home");
    QPushButton *btnBiblioteca = new QPushButton("📚 Biblioteca");
    QPushButton *btnFavoritos  = new QPushButton("❤️ Favoritos");
    QPushButton *btnPlaylists  = new QPushButton("🎶 Playlists");
    QPushButton *btnDescargas  = new QPushButton("📁 Descargas");

    QList<QPushButton*> botones = {btnHome, btnBiblioteca, btnFavoritos, btnPlaylists, btnDescargas};
    for (QPushButton *btn : botones) {
        btn->setStyleSheet("text-align: left; padding: 6px; border-radius:6px;");
        lay->addWidget(btn);
    }

    QObject::connect(btnFavoritos, &QPushButton::clicked, [=]() { mostrarFavoritos(); });
    QObject::connect(btnPlaylists, &QPushButton::clicked, [=]() { mostrarPlaylists(); });
    QObject::connect(btnDescargas, &QPushButton::clicked, [=](){ mostrarDescargas(); });

    // Botón Cerrar sesión MÁS ARRIBA
    QPushButton *btnCerrarSesion = ventanaPrincipal->getBtnCerrarSesion();
    btnCerrarSesion->setText("🚪 Cerrar sesión");
    btnCerrarSesion->setStyleSheet("background-color: #E74C3C; color: white; font-weight: bold; padding: 6px; border-radius: 4px;");
    lay->addSpacing(6);
    lay->addWidget(btnCerrarSesion); // visible sin ir hasta abajo
    lay->addStretch();

    // Conexiones (cada vista limpia TODO el centro y ocupa el área completa)
    QObject::connect(btnHome, &QPushButton::clicked, [=]() {
        construirHome();
    });

    QObject::connect(btnBiblioteca, &QPushButton::clicked, [=]() {
        if (!zonaCentralWidget) return;
        limpiarCentro();
        QVBoxLayout *l = new QVBoxLayout(zonaCentralWidget);
        l->setContentsMargins(8,8,8,8);
        QLabel *t = new QLabel("📚 Tu Biblioteca");
        t->setStyleSheet("font-size:20px; font-weight:bold;");
        l->addWidget(t);

        QListWidget *lista = new QListWidget; lista->setStyleSheet("color:white;");
        ManejadorCanciones mc;
        for (const auto &c : mc.obtenerTodas())
            if (c.activo) {
                auto *it = new QListWidgetItem(c.titulo + " — " + c.artista, lista);
                it->setData(Qt::UserRole, static_cast<qlonglong>(c.id));
            }
        l->addWidget(lista, 1);

        QObject::connect(lista, &QListWidget::itemDoubleClicked, [=](QListWidgetItem *it){
            const qint64 id = it->data(Qt::UserRole).toLongLong();
            reproducirCancionMeta(buscarPorId(id));
        });
    });

    QObject::connect(btnDescargas, &QPushButton::clicked, [=]() {
        if (!zonaCentralWidget) return;
        limpiarCentro();
        QVBoxLayout *l = new QVBoxLayout(zonaCentralWidget);
        l->setContentsMargins(8,8,8,8);
        QLabel *t = new QLabel("📁 Tus Descargas");
        t->setStyleSheet("font-size:20px; font-weight:bold;");
        l->addWidget(t);
        l->addStretch(1);
    });


    QObject::connect(btnCerrarSesion, &QPushButton::clicked, [=]() {
        if (auto *stack = menu->parentWidget()->parentWidget()->findChild<QStackedWidget *>()) stack->setCurrentIndex(0);
        // Parar reproducción lo maneja tu cframe si lo deseas.
    });

    return menu;
}

// ================== Zona central (HOME) ==================
QWidget* InterfazUsuario::crearZonaCentral() {
    zonaCentralScroll = new SmartScrollAreaUser;
    zonaCentralScroll->setAutoHide(false, true); // scroll vertical visible según necesidad
    zonaCentralWidget = new QWidget;
    zonaCentralScroll->setWidget(zonaCentralWidget);
    construirHome();
    return zonaCentralScroll;
}

static void mezclar(QList<Cancion> &v) {
    if (!v.isEmpty()) std::shuffle(v.begin(), v.end(), *QRandomGenerator::global());
}

void InterfazUsuario::limpiarCentro() {
    if (!zonaCentralWidget) return;

    // 1) Si había layout, saca y destruye sus items y widgets
    if (QLayout *old = zonaCentralWidget->layout()) {
        QLayoutItem *it;
        while ((it = old->takeAt(0)) != nullptr) {
            if (QWidget *w = it->widget()) w->deleteLater();
            delete it;
        }
        delete old;
    }
    // 2) Por si quedaron widgets sueltos sin layout (residuos visuales)
    const auto hijos = zonaCentralWidget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget *w : hijos) w->deleteLater();
}

void InterfazUsuario::construirHome() {
    if (!zonaCentralWidget) return;

    // Limpia contenido anterior
    limpiarCentro();
    QVBoxLayout *root = new QVBoxLayout(zonaCentralWidget);
    root->setSpacing(18);
    root->setContentsMargins(8, 0, 8, 0);

    // ------------- Datos base -------------
    ManejadorCanciones mc;
    cacheCanciones = mc.obtenerTodas();

    // Género preferido
    const QString generoPreferido = usuarioActual.generoPreferido;

    // Separación por género
    QList<Cancion> porGenero, otros;
    for (const Cancion &c : cacheCanciones) {
        if (!c.activo) continue;
        if (!generoPreferido.isEmpty() && c.genero == generoPreferido) porGenero << c;
        else if (!generoPreferido.isEmpty() && c.genero != generoPreferido) otros << c;
        else otros << c; // sin preferido → todo a explorar
    }
    mezclar(porGenero);
    mezclar(otros);

    // ============= Recomendados (8 del preferido) =============
    QList<QWidget*> tarjetasRec;
    for (int i=0; i<qMin(8, porGenero.size()); ++i)
        tarjetasRec << crearTarjetaCancion(porGenero[i]);
    root->addWidget(crearSeccionHorizontal(
        QString("⭐ Recomendados %1").arg(generoPreferido.isEmpty()? "" : "· " + generoPreferido), tarjetasRec));

    // ============= Recién escuchados (carátula por colección o Single) =============
    QList<QWidget*> tarjetasRecientes;
    {
        // leer reproducciones
        struct Rep { qint64 id; int user; qint64 ms; QDate fecha; };
        QList<Rep> reps;
        QFile f("reproducciones.dat");
        if (f.open(QIODevice::ReadOnly)) {
            QDataStream in(&f);
            while (!in.atEnd()) { Rep r; in >> r.id >> r.user >> r.ms >> r.fecha; reps.append(r); }
            f.close();
        }
        std::sort(reps.begin(), reps.end(), [](const Rep&a,const Rep&b){ return a.fecha>b.fecha; });

        QSet<QString> vistos;
        int maxItems = 8;

        for (const Rep &r : reps) {
            if (tarjetasRecientes.size() >= maxItems) break;
            Cancion found; bool ok=false;
            for (const Cancion &c : cacheCanciones) if (c.id == r.id) { found=c; ok=true; break; }
            if (!ok || !found.activo) continue;

            const QString clave = (found.tipo=="Single") ? QString("S:%1").arg(found.id)
                                                           : QString("%1:%2").arg(found.tipo, found.coleccion);
            if (vistos.contains(clave)) continue;
            vistos.insert(clave);

            QWidget *card = new QWidget; card->setFixedWidth(180);
            QVBoxLayout *v = new QVBoxLayout(card); v->setSpacing(8); v->setContentsMargins(8,8,8,8);
            v->addWidget(coverLabel(found.portada, QSize(180,180), true));
            QLabel *t = new QLabel(found.tipo=="Single"? found.titulo : found.coleccion); t->setWordWrap(true);
            t->setStyleSheet("font-weight:600; font-size:14px;"); v->addWidget(t);

            auto onDbl = [=]() {
                if (found.tipo=="Single") reproducirCancion(found.rutaArchivo);
                else abrirColeccionUsuario(found.coleccion, found.tipo, found.artista);
            };
            auto *filt = new DblClickFilter(onDbl, card);
            card->installEventFilter(filt); t->installEventFilter(filt);

            card->setStyleSheet("QWidget{background:#1e1e1e;border-radius:12px;} QWidget:hover{background:#262626;}");
            tarjetasRecientes << card;
        }
    }
    root->addWidget(crearSeccionHorizontal("🕒 Recién escuchados", tarjetasRecientes));

    // ============= Explorar (8 de otros géneros) =============
    QList<QWidget*> tarjetasExplorar;
    for (int i=0; i<qMin(8, otros.size()); ++i)
        tarjetasExplorar << crearTarjetaCancion(otros[i]);
    root->addWidget(crearSeccionHorizontal("🔎 Explorar", tarjetasExplorar));

    root->addStretch();
}

void InterfazUsuario::mostrarPerfil() {
    if (!zonaCentralWidget) return;
    limpiarCentro();
    QVBoxLayout *l = new QVBoxLayout(zonaCentralWidget);
    l->setContentsMargins(8,8,8,8);

    QPushButton *btnPerfil = new QPushButton("👤 Mi perfil");
    btnPerfil->setStyleSheet("text-align:left; padding:6px; border-radius:6px; font-weight:600;");
    l->addWidget(btnPerfil);

    QObject::connect(btnPerfil, &QPushButton::clicked, [=](){
        mostrarPerfil();
    });

    QWidget *card = new QWidget;
    card->setStyleSheet("background:#1e1e1e; border-radius:10px;");
    QHBoxLayout *hl = new QHBoxLayout(card);
    hl->setContentsMargins(12,12,12,12);
    hl->setSpacing(12);

    QLabel *avatar = coverLabel(usuarioActual.rutaImagen, QSize(120,120), true);
    hl->addWidget(avatar);

    QVBoxLayout *info = new QVBoxLayout;
    auto mk = [](const QString &s){ auto *L=new QLabel(s); L->setStyleSheet("font-size:14px;"); return L; };
    info->addWidget(mk("Alias: " + usuarioActual.aliasre));
    info->addWidget(mk("Nombre: " + usuarioActual.nombreReal));
    info->addWidget(mk("Correo: " + usuarioActual.correoElectronico));
    info->addWidget(mk("Género preferido: " + usuarioActual.generoPreferido));
    hl->addLayout(info, 1);

    l->addWidget(card);
    l->addStretch(1);
}

void InterfazUsuario::mostrarResultadosBusqueda(const QString &qRaw) {
    if (!zonaCentralWidget) return;

    const QString q = qRaw.trimmed();
    limpiarCentro();

    QVBoxLayout *l = new QVBoxLayout(zonaCentralWidget);
    l->setContentsMargins(8,8,8,8);

    QLabel *t = new QLabel(QString("🔎 Resultados para “%1”").arg(q.isEmpty()? "…" : q));
    t->setStyleSheet("font-size:20px; font-weight:bold;");
    l->addWidget(t);

    // Asegura catálogo en memoria
    if (cacheCanciones.isEmpty()) {
        ManejadorCanciones mc; cacheCanciones = mc.obtenerTodas();
    }

    // Normalizador mínimo (minúsculas). Si quieres quitar acentos, avísame.
    auto norm = [](QString s){ return s.toLower(); };
    const QString nq = norm(q);

    QList<Cancion> matches;
    if (!nq.isEmpty()) {
        for (const Cancion &c : cacheCanciones) {
            if (!c.activo) continue;

            const QString nt = norm(c.titulo);
            const QString na = norm(c.artista);
            const QString ng = norm(c.genero);
            const QString nc = norm(c.coleccion);

            bool ok = false;
            if (nq.size() == 1) {
                ok = nt.startsWith(nq) || na.startsWith(nq) || ng.startsWith(nq) || nc.startsWith(nq);
            } else {
                ok = nt.contains(nq) || na.contains(nq) || ng.contains(nq) || nc.contains(nq);
            }
            if (ok) matches << c;
        }
    }

    if (matches.isEmpty()) {
        QLabel *v = new QLabel(q.isEmpty()? "Escribe para buscar." : "Sin resultados.");
        v->setStyleSheet("color:#bbb;");
        l->addWidget(v);
        l->addStretch();
        return;
    }

    // Grilla de tarjetas (similar a Home)
    QWidget *gridW = new QWidget;
    QGridLayout *grid = new QGridLayout(gridW);
    grid->setContentsMargins(0,0,0,0);
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(12);

    const int COLS = 4; // ajusta columnas si quieres
    int row = 0, col = 0;

    for (const Cancion &c : matches) {
        QWidget *card = crearTarjetaCancion(c);
        grid->addWidget(card, row, col);
        card->setStyleSheet("QWidget{background:#1e1e1e;border-radius:12px;} QWidget:hover{background:#262626;}");
        if (++col >= COLS) { col = 0; ++row; }
    }

    l->addWidget(gridW);
    l->addStretch();
}



// Crea una sección horizontal con título y scroll auto-oculto
QWidget* InterfazUsuario::crearSeccionHorizontal(const QString &titulo, const QList<QWidget*> &tarjetas) {
    QWidget *seccion = new QWidget;
    QVBoxLayout *v = new QVBoxLayout(seccion);
    v->setSpacing(8);
    v->setContentsMargins(0, 0, 0, 0);

    QLabel *t = new QLabel(titulo);
    t->setStyleSheet("font-size: 20px; font-weight: bold;");
    v->addWidget(t);

    auto *scroll = new SmartScrollAreaUser;
    scroll->setAutoHide(true, false); // oculta horizontal
    QWidget *viewport = new QWidget;
    QHBoxLayout *h = new QHBoxLayout(viewport);
    h->setSpacing(12);
    h->setContentsMargins(0, 0, 0, 0);

    for (QWidget *w : tarjetas) h->addWidget(w);
    h->addStretch();

    viewport->setLayout(h);
    scroll->setWidget(viewport);
    v->addWidget(scroll);
    return seccion;
}

// ---------- Tarjetas ----------
QWidget* InterfazUsuario::crearTarjetaCancion(const Cancion &c) {
    QWidget *card = new QWidget;
    card->setFixedWidth(180);
    QVBoxLayout *v = new QVBoxLayout(card);
    v->setSpacing(8);
    v->setContentsMargins(8, 8, 8, 8);

    QLabel *img = coverLabel(c.portada, QSize(180, 180), true);
    v->addWidget(img);

    QLabel *lblTitulo = new QLabel(c.titulo);
    lblTitulo->setStyleSheet("font-weight:600; font-size:14px;");
    lblTitulo->setWordWrap(true);
    v->addWidget(lblTitulo);

    QLabel *lblSub = new QLabel(QString("%1 · %2").arg(c.artista, c.tipo));
    lblSub->setStyleSheet("color:#bbb; font-size:12px;");
    lblSub->setWordWrap(true);
    v->addWidget(lblSub);

    //***************************************************************************
    //***************************************************************************
    //***************************************************************************
    // HASHTABLE (Favoritos) — USO EN UI (lectura/estado/toggle) — BEGIN

    QHBoxLayout *actions = new QHBoxLayout;
    actions->setContentsMargins(0,0,0,0);
    actions->addStretch();

    auto mkTool = [](const QString& txt)->QPushButton*{
        auto *b = new QPushButton(txt);
        b->setCursor(Qt::PointingHandCursor);
        b->setFlat(true);
        b->setStyleSheet("QPushButton{font-size:16px;color:#bbb;} QPushButton:hover{color:white;}");
        return b;
    };

    QPushButton *btnFav  = mkTool("❤");
    QPushButton *btnAdd  = mkTool("+");
    QPushButton *btnDown = mkTool("⤓");
    actions->addWidget(btnFav);
    actions->addWidget(btnAdd);
    actions->addWidget(btnDown);
    v->addLayout(actions);

    const qint64 sid = c.id;
    const Cancion song = c;


    // Estado inicial ❤️ y ⤓
    QSet<qint64> favs = leerFavoritos();
    btnFav->setStyleSheet(QString("QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}")
                              .arg(favs.contains(c.id) ? "#E74C3C" : "#bbb"));

    QSet<qint64> downs = leerDescargas();
    btnDown->setStyleSheet(QString("QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}")
                               .arg(downs.contains(c.id) ? "#1DB954" : "#bbb"));


    // Toggle favorito (solo esta pista)
    QObject::connect(btnFav, &QPushButton::clicked, card, [=](bool){
        toggleFavorito(sid, btnFav);
    });


    // Menú “Agregar a playlist”
    QObject::connect(btnAdd, &QPushButton::clicked, card, [=](bool){
        QMenu *m = menuAddToPlaylist(sid);
        m->exec(QCursor::pos());
        m->deleteLater();
    });

    // Marcar/Desmarcar descarga
    QObject::connect(btnDown, &QPushButton::clicked, card, [=](){
        QSet<qint64> s = leerDescargas();
        const bool on = !s.contains(sid);
        if (on) s.insert(sid); else s.remove(sid);
        guardarDescargas(s);
        // feedback visual
        btnDown->setStyleSheet(QString(
                                   "QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}"
                                   ).arg(on ? "#1DB954" : "#bbb"));
    });

    // HASHTABLE (Favoritos) — USO EN UI (lectura/estado/toggle) — END
    //***************************************************************************
    //***************************************************************************
    //***************************************************************************

    auto onDbl = [=]() {
        if (!c.coleccion.isEmpty() && (c.tipo == "Álbum" || c.tipo == "EP"))
            abrirColeccionUsuario(c.coleccion, c.tipo, c.artista);
        else
            reproducirCancion(c.rutaArchivo);
    };
    auto *f = new DblClickFilter(onDbl, card);
    card->installEventFilter(f); img->installEventFilter(f); lblTitulo->installEventFilter(f);

    card->setStyleSheet(
        "QWidget{background:#1e1e1e; border-radius:12px;}"
        "QWidget:hover{background:#262626;}"
        );
    return card;
}

//***************************************************************************
//***************************************************************************
//***************************************************************************
// HASHTABLE — Toggle de favorito y menú de agregar — BEGIN

void InterfazUsuario::toggleFavorito(qint64 cancionId, QPushButton *btnHeart) {
    QSet<qint64> s = leerFavoritos();
    const bool on = !s.contains(cancionId);
    if (on) s.insert(cancionId); else s.remove(cancionId);
    guardarFavoritos(s);
    if (btnHeart)
        btnHeart->setStyleSheet(QString("QPushButton{font-size:18px;color:%1;} QPushButton:hover{color:white;}")
                                    .arg(on ? "#E74C3C" : "#bbb"));
}

QMenu* InterfazUsuario::menuAddToPlaylist(qint64 cancionId) {
    //***************************************************************************
    // Nota: el menú se llena leyendo las cabeceras de playlists (archivo “indexado”)
    //***************************************************************************
    QMenu *m = new QMenu;
    auto pls = leerPlaylists();
    for (const auto &p : pls) {
        QAction *a = m->addAction("➕ " + p.nombre);
        QObject::connect(a, &QAction::triggered, m, [=](bool){
            agregarCancionAPlaylist(p.id, cancionId);
        });
    }
    if (!pls.isEmpty()) m->addSeparator();
    QAction *nuevo = m->addAction("🆕 Nueva playlist…");
    QObject::connect(nuevo, &QAction::triggered, m, [=](bool){
        dialogCrearPlaylist([=](const Playlist &p){ agregarCancionAPlaylist(p.id, cancionId); });
    });
    return m;
}

// HASHTABLE — Toggle de favorito y menú de agregar — END
//***************************************************************************
//***************************************************************************
//***************************************************************************



//***************************************************************************
//***************************************************************************
//***************************************************************************
// ARCHIVOS INDEXADOS — Inserción (playlist_items) — BEGIN

void InterfazUsuario::agregarCancionAPlaylist(qint64 playlistId, qint64 cancionId) {
    auto items = leerPlaylistItems();
    for (const auto &it : items)
        if (it.playlistId == playlistId && it.cancionId == cancionId) {
            QMessageBox::information(nullptr, "Playlist", "La canción ya está en la playlist.");
            return;
        }
    PlaylistItem it; it.playlistId = playlistId; it.cancionId = cancionId;
    items.append(it);
    guardarPlaylistItems(items);
    QMessageBox::information(nullptr, "Playlist", "Agregada a la playlist.");
}

// ARCHIVOS INDEXADOS — Inserción (playlist_items) — END
//***************************************************************************
//***************************************************************************
//***************************************************************************


QWidget* InterfazUsuario::crearTarjetaColeccion(const QString &nombreColeccion, const QString &portada, const QString &tipo) {
    QWidget *card = new QWidget;
    card->setFixedWidth(220);
    QVBoxLayout *v = new QVBoxLayout(card);
    v->addWidget(coverLabel(portada, QSize(220,220), true));
    QLabel *t = new QLabel(QString("%1 · %2").arg(nombreColeccion, tipo));
    t->setWordWrap(true);
    v->addWidget(t);
    return card;
}

// ---------- Navegación (diálogo Álbum/EP sin botón Play) ----------
void InterfazUsuario::abrirColeccionUsuario(const QString &nombreColeccion, const QString &tipo, const QString &artista) {
    QDialog dlg;
    dlg.setWindowTitle(nombreColeccion);
    dlg.setModal(true);
    dlg.resize(640, 560);
    dlg.setStyleSheet("QDialog{background:#151515;color:white;} QLabel{color:white;}");

    QVBoxLayout *root = new QVBoxLayout(&dlg);

    // Header con carátula real
    QString portadaColeccion;
    for (const Cancion &c : cacheCanciones) {
        if (!c.activo) continue;
        if (c.coleccion == nombreColeccion && (artista.isEmpty() || c.artista == artista)) {
            portadaColeccion = c.portada; break;
        }
    }

    QWidget *header = new QWidget; QHBoxLayout *h = new QHBoxLayout(header);
    QLabel *art = coverLabel(portadaColeccion, QSize(180,180), true);
    QLabel *lblTitulo = new QLabel(nombreColeccion); lblTitulo->setStyleSheet("font-size:20px;font-weight:700;");
    QLabel *lblTipo = new QLabel(QString("Tipo: %1").arg(tipo));
    QVBoxLayout *info = new QVBoxLayout; info->addWidget(lblTitulo); info->addWidget(lblTipo);
    h->addWidget(art); h->addLayout(info,1);
    root->addWidget(header);

    // Lista (doble clic para reproducir; muestra DURACIÓN, sin carátula por pista)
    auto *scroll = new SmartScrollAreaUser; scroll->setAutoHide(false,true);
    QWidget *contenedor = new QWidget; QVBoxLayout *v = new QVBoxLayout(contenedor);

    int n=1;
    for (const Cancion &c : cacheCanciones) {
        // 🔒 Filtrar SOLO las canciones que pertenecen a esta colección/tipo/artista
        if (!c.activo) continue;
        if (c.coleccion != nombreColeccion) continue;
        if (!(tipo == "Álbum" || tipo == "EP")) continue;   // la vista de colecciones solo es para Álbum/EP
        if (!artista.isEmpty() && c.artista != artista) continue;

        // === Fila de canción en Álbum/EP ===
        QWidget *row = new QWidget;
        QHBoxLayout *r = new QHBoxLayout(row);
        r->setContentsMargins(8,6,8,6);
        r->setSpacing(10);

        QLabel *num = new QLabel(QString::number(n++));
        QLabel *img = coverLabel(c.portada, QSize(56,56), true);
        QLabel *tit = new QLabel(c.titulo);
        tit->setWordWrap(true);
        QLabel *dur = new QLabel(c.duracion.isEmpty() ? "--:--" : c.duracion);
        dur->setMinimumWidth(48);
        dur->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        r->addWidget(num);
        r->addWidget(img);
        r->addWidget(tit, 1);

        // --- Botones acción SOLO ESTA PISTA ---
        auto mk = [](const QString& s){
            auto *b = new QPushButton(s);
            b->setFlat(true);
            b->setCursor(Qt::PointingHandCursor);
            b->setStyleSheet("QPushButton{font-size:16px;color:#bbb;} QPushButton:hover{color:white;}");
            return b;
        };
        QPushButton *aFav  = mk("❤");
        QPushButton *aAdd  = mk("+");
        QPushButton *aDown = mk("⤓");

        // Estado inicial por pista
        {
            QSet<qint64> favs = leerFavoritos();
            aFav->setStyleSheet(QString("QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}")
                                    .arg(favs.contains(c.id) ? "#E74C3C" : "#bbb"));
        }
        {
            QSet<qint64> downs = leerDescargas();
            aDown->setStyleSheet(QString("QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}")
                                     .arg(downs.contains(c.id) ? "#1DB954" : "#bbb"));
        }

        // ⚓️ CAPTURAS POR VALOR → afectan SOLO a 'c.id'
        const qint64 sid = c.id;
        const Cancion song = c;

        QObject::connect(aFav,  &QPushButton::clicked, row, [=](bool){ toggleFavorito(sid, aFav); });
        QObject::connect(aAdd,  &QPushButton::clicked, row, [=](bool){
            QMenu *m = menuAddToPlaylist(sid);
            m->exec(QCursor::pos());
            m->deleteLater();
        });
        QObject::connect(aDown, &QPushButton::clicked, row, [=](bool){
            QSet<qint64> s = leerDescargas();
            const bool on = !s.contains(sid);
            if (on) s.insert(sid); else s.remove(sid);
            guardarDescargas(s);
            aDown->setStyleSheet(QString("QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}")
                                     .arg(on ? "#1DB954" : "#bbb"));
        });

        r->addWidget(aFav);
        r->addWidget(aAdd);
        r->addWidget(aDown);

        r->addStretch();
        r->addWidget(dur);
        v->addWidget(row);

        // Doble clic → reproducir SOLO esta pista
        auto *f = new DblClickFilter([=](){ reproducirCancionMeta(song); }, row);
        row->installEventFilter(f);
        tit->installEventFilter(f);
        img->installEventFilter(f);

        // Relleno de duración si falta (solo esta fila)
        if (song.duracion.isEmpty()) {
            asegurarDuracion(song, [dur](const QString &mmss){ dur->setText(mmss); });
        }
    }
    v->addStretch(1);
    contenedor->setLayout(v);
    scroll->setWidget(contenedor);
    root->addWidget(scroll, 1);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Close);
    QObject::connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    root->addWidget(bb);

    dlg.exec();
}

void InterfazUsuario::abrirArtistaUsuario(const QString &artista) {
    QDialog dlg;
    dlg.setWindowTitle(artista);
    dlg.setModal(true);
    dlg.resize(640, 560);
    dlg.setStyleSheet("QDialog{background:#151515;color:white;} QLabel{color:white;}");

    QVBoxLayout *root = new QVBoxLayout(&dlg);

    QLabel *titulo = new QLabel(QString("Artista: %1").arg(artista));
    titulo->setStyleSheet("font-size:22px; font-weight:800;");
    root->addWidget(titulo);

    auto tarjetasPorTipo = [&](const QString &tipo){
        QList<QWidget*> tarjetas;
        QSet<QString> coleccionesVistas;
        for (const Cancion &c : cacheCanciones) {
            if (!c.activo || c.artista != artista) continue;
            if (tipo == "Single") {
                if (c.tipo == "Single") tarjetas << crearTarjetaCancion(c);
            } else {
                if (c.tipo == tipo && !c.coleccion.isEmpty() && !coleccionesVistas.contains(c.coleccion)) {
                    coleccionesVistas.insert(c.coleccion);
                    QWidget *tar = crearTarjetaColeccion(c.coleccion, c.portada, tipo);
                    auto *f = new DblClickFilter([=](){ abrirColeccionUsuario(c.coleccion, tipo, artista); }, tar);
                    tar->installEventFilter(f);
                    tarjetas << tar;
                }
            }
        }
        return tarjetas;
    };

    if (QWidget *w = crearSeccionHorizontal("Singles", tarjetasPorTipo("Single"))) root->addWidget(w);
    if (QWidget *w = crearSeccionHorizontal("Álbumes", tarjetasPorTipo("Álbum"))) root->addWidget(w);
    if (QWidget *w = crearSeccionHorizontal("EPs", tarjetasPorTipo("EP"))) root->addWidget(w);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Close);
    QObject::connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    root->addWidget(bb);

    dlg.exec();
}

// ================== Zona derecha (info + rate que recuerda selección) ==================
QWidget* InterfazUsuario::crearZonaDerecha() {
    QWidget *derecha = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(derecha);
    lay->setContentsMargins(6,6,6,6);

    QLabel *img = new QLabel;
    img->setObjectName("lblRightCover");
    img->setPixmap(QPixmap(":/img/default.jpg").scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    img->setFixedSize(120, 120);
    img->setStyleSheet("background-color: #2a2a2a;");
    lay->addWidget(img, 0, Qt::AlignCenter);

    QLabel *lblTitulo  = new QLabel("🎵 Título de la canción");  lblTitulo->setObjectName("lblRightTitulo");
    QLabel *lblArtista = new QLabel("👤 Artista");               lblArtista->setObjectName("lblRightArtista");
    QLabel *lblAlbum   = new QLabel("📀 Colección");             lblAlbum->setObjectName("lblRightAlbum");
    QLabel *lblDur     = new QLabel("🕒 Duración");              lblDur->setObjectName("lblRightDuracion");
    for (QLabel *l : {lblTitulo,lblArtista,lblAlbum,lblDur}) { l->setStyleSheet("color: white;"); lay->addWidget(l, 0, Qt::AlignCenter); }

    QLabel *lbl = new QLabel("⭐ Califica esta canción");
    lbl->setStyleSheet("font-weight: bold;");
    lay->addWidget(lbl);

    for (int i = 1; i <= 5; ++i) {
        QRadioButton *rb = new QRadioButton(QString("⭐ %1 estrellas").arg(i));
        rb->setObjectName(QString("rbRate%1").arg(i));
        lay->addWidget(rb);
    }

    QPushButton *btnEnviar = new QPushButton("Enviar Calificación");
    lay->addWidget(btnEnviar);

    QObject::connect(btnEnviar, &QPushButton::clicked, [this]() {
        QLabel *lblT = ventanaPrincipal->findChild<QLabel*>("lblRightTitulo");
        if (!lblT || !lblT->property("songId").isValid()) { QMessageBox::warning(nullptr,"Rate","Reproduce primero una canción."); return; }
        qint64 songId = lblT->property("songId").toLongLong();

        int estrellas = 0;
        for (int i=1;i<=5;++i) {
            if (auto *rb = ventanaPrincipal->findChild<QRadioButton*>(QString("rbRate%1").arg(i))) {
                if (rb->isChecked()) { estrellas = i; break; }
            }
        }
        if (!estrellas) { QMessageBox::warning(nullptr,"Rate","Selecciona de 1 a 5 estrellas."); return; }
        guardarCalificacion(songId, estrellas);
        QMessageBox::information(nullptr,"Gracias","¡Calificación registrada!");
    });

    lay->addStretch();
    return derecha;
}

// ================== Reproducir ==================
void InterfazUsuario::reproducirCancion(const QString &ruta) {
    Cancion c = buscarPorRuta(ruta);
    if (c.id == 0) {
        QMessageBox::warning(nullptr, "Error", "Ruta no corresponde a ninguna canción del catálogo.");
        return;
    }
    reproducirCancionMeta(c);
}

void InterfazUsuario::reproducirCancionMeta(const Cancion &c) {
    if (c.rutaArchivo.isEmpty()) { QMessageBox::warning(nullptr,"Error","Ruta de archivo vacía."); return; }

    QMediaPlayer *player = ventanaPrincipal->findChild<QMediaPlayer *>();
    QAudioOutput *audio = ventanaPrincipal->findChild<QAudioOutput *>();
    if (!player || !audio) { QMessageBox::warning(nullptr,"Error","No se encontró el reproductor de audio."); return; }

    player->setSource(QUrl::fromLocalFile(c.rutaArchivo));
    audio->setVolume(ventanaPrincipal->getSliderVolumen()->value() / 100.0);
    player->play();

    actualizarPanelDerecha(c);
    guardarReproduccion(c.id, 0);

    // Refrescar Home para que “Recién escuchados” se actualice sin salir/entrar
    if (zonaCentralWidget) construirHome();
}

void InterfazUsuario::actualizarPanelDerecha(const Cancion &c) {
    if (QLabel *img=ventanaPrincipal->findChild<QLabel*>("lblRightCover"))
        img->setPixmap(fitOrCover(c.portada, img->size(), true));
    if (QLabel *t = ventanaPrincipal->findChild<QLabel*>("lblRightTitulo"))  { t->setText("🎵 " + c.titulo); t->setProperty("songId", QVariant::fromValue(c.id)); }
    if (QLabel *a = ventanaPrincipal->findChild<QLabel*>("lblRightArtista")) a->setText("👤 " + c.artista);
    if (QLabel *al= ventanaPrincipal->findChild<QLabel*>("lblRightAlbum"))   al->setText("📀 " + (c.tipo=="Single" ? "Single" : c.coleccion));
    if (QLabel *d = ventanaPrincipal->findChild<QLabel*>("lblRightDuracion"))d->setText("🕒 " + c.duracion);

    if (QLabel *d = ventanaPrincipal->findChild<QLabel*>("lblRightDuracion")) {
        d->setText("🕒 " + (c.duracion.isEmpty() ? "--:--" : c.duracion));
        if (c.duracion.isEmpty()) {
            asegurarDuracion(c, [d](const QString &mmss){ d->setText("🕒 " + mmss); });
        }
    }

    // Pre-seleccionar rate si existe
    int estrellas = leerCalificacion(c.id);
    for (int i=1;i<=5;++i) {
        if (auto *rb = ventanaPrincipal->findChild<QRadioButton*>(QString("rbRate%1").arg(i))) {
            rb->setAutoExclusive(false);
            rb->setChecked(i==estrellas);
            rb->setAutoExclusive(true);
        }
    }
}

// ================== Utilidades búsqueda ==================
Cancion InterfazUsuario::buscarPorRuta(const QString &ruta) const {
    for (const Cancion &c : cacheCanciones) if (c.rutaArchivo == ruta) return c;
    return Cancion{};
}
Cancion InterfazUsuario::buscarPorId(qint64 id) const {
    for (const Cancion &c : cacheCanciones) if (c.id == id) return c;
    return Cancion{};
}

// ================== Reproducciones / Calificaciones ==================
QList<InterfazUsuario::Reproduccion> InterfazUsuario::leerReproducciones() const {
    QList<Reproduccion> list; QFile f("reproducciones.dat");
    if (!f.open(QIODevice::ReadOnly)) return list;
    QDataStream in(&f);
    while (!in.atEnd()) { Reproduccion r; in >> r.cancionId >> r.userId >> r.ms >> r.fecha; list.append(r); }
    return list;
}

void InterfazUsuario::guardarReproduccion(qint64 songId, qint64 ms) {
    QFile f("reproducciones.dat"); const bool existe = f.exists();
    if (!f.open(existe ? QIODevice::Append : (QIODevice::WriteOnly | QIODevice::Truncate))) return;
    QDataStream out(&f);
    int userId = usuarioActual.id; // usa el id real del usuario activo
    Reproduccion r{songId, userId, ms, QDate::currentDate()};
    out << r.cancionId << r.userId << r.ms << r.fecha;
}

int InterfazUsuario::leerCalificacion(qint64 songId) const {
    QFile f("calificaciones.dat");
    if (!f.open(QIODevice::ReadOnly)) return 0;
    QDataStream in(&f);
    while (!in.atEnd()) {
        qint64 cid; int uid; int estrellas; QDate fecha;
        in >> cid >> uid >> estrellas >> fecha;
        if (cid == songId && uid == usuarioActual.id) { f.close(); return estrellas; }
    }
    f.close(); return 0;
}

void InterfazUsuario::guardarCalificacion(qint64 songId, int estrellas) {
    QFile f("calificaciones.dat"); const bool existe = f.exists();
    if (!f.open(existe ? QIODevice::Append : (QIODevice::WriteOnly | QIODevice::Truncate))) return;
    QDataStream out(&f);
    out << songId << usuarioActual.id << estrellas << QDate::currentDate();
    f.close();
}

//***************************************************************************
//***************************************************************************
//***************************************************************************
// HASHTABLE — Favoritos (QSet<qint64>) — BEGIN

QSet<qint64> InterfazUsuario::leerFavoritos() const {
    QSet<qint64> s;
    QFile f(pathFav());
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream in(&f);
        while (!in.atEnd()) { qint64 id; in >> id; s.insert(id); }
    }
    return s;
}

void InterfazUsuario::guardarFavoritos(const QSet<qint64>& s) const {
    QFile f(pathFav());
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QDataStream out(&f);
        for (qint64 id : s) out << id;
    }
}

QSet<qint64> InterfazUsuario::leerDescargas() const {
    QSet<qint64> s;
    QFile f(pathDOWN());
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream in(&f);
        while (!in.atEnd()) { qint64 id; in >> id; s.insert(id); }
    }
    return s;
}
void InterfazUsuario::guardarDescargas(const QSet<qint64>& s) const {
    QFile f(pathDOWN());
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QDataStream out(&f);
        for (qint64 id : s) out << id;
    }
}

// HASHTABLE — Favoritos (QSet<qint64>) — END
//***************************************************************************
//***************************************************************************
//***************************************************************************



//***************************************************************************
//***************************************************************************
//***************************************************************************
// ARCHIVOS INDEXADOS — Playlists (cabeceras) — BEGIN

QList<InterfazUsuario::Playlist> InterfazUsuario::leerPlaylists() const {
    QList<Playlist> v;
    QFile f(pathPL());
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream in(&f);
        while (!in.atEnd()) { Playlist p; p.cargar(in); v.append(p); }
    }
    return v;
}

void InterfazUsuario::guardarPlaylists(const QList<Playlist>& v) const {
    QFile f(pathPL());
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QDataStream out(&f);
        for (const auto& p : v) p.guardar(out);
    }
}

// ARCHIVOS INDEXADOS — Playlists (cabeceras) — END
//***************************************************************************
//***************************************************************************
//***************************************************************************



//***************************************************************************
//***************************************************************************
//***************************************************************************
// ARCHIVOS INDEXADOS — Items (playlistId,cancionId) — BEGIN

QList<InterfazUsuario::PlaylistItem> InterfazUsuario::leerPlaylistItems() const {
    QList<PlaylistItem> v;
    QFile f(pathPLI());
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream in(&f);
        while (!in.atEnd()) { PlaylistItem it; it.cargar(in); v.append(it); }
    }
    return v;
}

void InterfazUsuario::guardarPlaylistItems(const QList<PlaylistItem>& v) const {
    QFile f(pathPLI());
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QDataStream out(&f);
        for (const auto& it : v) it.guardar(out);
    }
}

// ARCHIVOS INDEXADOS — Items (playlistId,cancionId) — END
//***************************************************************************
//***************************************************************************
//***************************************************************************

//***************************************************************************
//***************************************************************************
//***************************************************************************
// LISTAS ENLAZADAS — Render de favoritos desde hash — BEGIN

void InterfazUsuario::mostrarFavoritos() {
    if (!zonaCentralWidget) return;
    limpiarCentro();

    QVBoxLayout *l = new QVBoxLayout(zonaCentralWidget);
    l->setContentsMargins(8,8,8,8);

    QLabel *t = new QLabel("❤️ Tus Favoritos");
    t->setStyleSheet("font-size: 20px; font-weight: bold;");
    l->addWidget(t);

    QSet<qint64> favs = leerFavoritos();
    if (cacheCanciones.isEmpty()) { ManejadorCanciones mc; cacheCanciones = mc.obtenerTodas(); }

    QListWidget *lista = new QListWidget;
    lista->setStyleSheet("color:white;");

    for (const Cancion &c : cacheCanciones) {
        if (c.activo && favs.contains(c.id)) {
            auto *item = new QListWidgetItem;
            const QString base = c.titulo;
            const QString dur  = c.duracion.isEmpty() ? "--:--" : c.duracion;
            item->setText(QString("%1    ·    %2").arg(base, dur));
            item->setData(Qt::UserRole, QVariant::fromValue(c.id));
            item->setIcon(QIcon(fitOrCover(c.portada, QSize(64,64), true)));
            lista->addItem(item);

            if (c.duracion.isEmpty()) {
                asegurarDuracion(c, [lista, item, base](const QString &mmss){
                    // Reemplaza sólo el texto del item concreto
                    item->setText(QString("%1    ·    %2").arg(base, mmss));
                    // (no hace falta buscarlo; ya tenemos el puntero)
                });
            }
        }
    }
    l->addWidget(lista, 1);

    QObject::connect(lista, &QListWidget::itemDoubleClicked, lista, [=](QListWidgetItem *it){
        qint64 id = it->data(Qt::UserRole).toLongLong();
        for (const Cancion &c : cacheCanciones) if (c.id==id) { reproducirCancionMeta(c); break; }
    });
}

// LISTAS ENLAZADAS — Render de favoritos desde hash — END
//***************************************************************************
//***************************************************************************
//***************************************************************************

//***************************************************************************
//***************************************************************************
//***************************************************************************
// LISTAS ENLAZADAS — Grid de playlists desde cabeceras — BEGIN

void InterfazUsuario::mostrarPlaylists() {
    if (!zonaCentralWidget) return;
    limpiarCentro();

    QVBoxLayout *root = new QVBoxLayout(zonaCentralWidget);
    root->setContentsMargins(8,8,8,8);

    // Header con + rojo
    QWidget *hdr = new QWidget;
    QHBoxLayout *hh = new QHBoxLayout(hdr);
    hh->setContentsMargins(0,0,0,0);

    QLabel *t = new QLabel("🎶 Tus Playlists");
    t->setStyleSheet("font-size: 20px; font-weight: bold;");
    QPushButton *btnMas = new QPushButton("＋");
    btnMas->setFixedSize(36,36);
    btnMas->setStyleSheet(
        "QPushButton{background:#E74C3C;color:white;border:0;border-radius:18px;font-size:22px;font-weight:800;}"
        "QPushButton:hover{background:#ff6b63;}");
    btnMas->setCursor(Qt::PointingHandCursor);

    hh->addStretch();
    hh->addWidget(t, 0, Qt::AlignCenter);
    hh->addStretch();
    hh->addWidget(btnMas);
    root->addWidget(hdr);

    // Grid de tarjetas
    QWidget *gridW = new QWidget;
    QGridLayout *grid = new QGridLayout(gridW);
    grid->setContentsMargins(0,8,0,0);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(18);

    auto pls = leerPlaylists();
    const int COLS = 4;
    int r=0, c=0;
    for (const auto &p : pls) {
        QWidget *card = new QWidget;
        card->setStyleSheet("background:#1e1e1e;border-radius:12px;");
        QVBoxLayout *cv = new QVBoxLayout(card);
        cv->setContentsMargins(8,8,8,8);
        cv->setSpacing(8);

        // ⬇️ Carátula cuadrada centrada (tipo álbum)
        QLabel *img = coverLabel(p.portada, QSize(220,220), true);
        cv->addWidget(img, 0, Qt::AlignHCenter);

        // ⬇️ Nombre centrado debajo
        QLabel *nm  = new QLabel(p.nombre);
        nm->setStyleSheet("font-size:14px;font-weight:600;");
        nm->setWordWrap(true);
        nm->setAlignment(Qt::AlignHCenter);
        cv->addWidget(nm);

        // Doble clic → abrir playlist
        auto *f = new DblClickFilter([=](){ abrirPlaylist(p); }, card);
        card->installEventFilter(f);
        img->installEventFilter(f);
        nm->installEventFilter(f);

        grid->addWidget(card, r, c);
        if (++c == COLS) { c=0; ++r; }
    }
    root->addWidget(gridW, 1);

    QObject::connect(btnMas, &QPushButton::clicked, zonaCentralWidget, [=](bool){
        dialogCrearPlaylist([=](const Playlist&){ mostrarPlaylists(); });
    });
}

// LISTAS ENLAZADAS — Grid de playlists desde cabeceras — END
//***************************************************************************
//***************************************************************************
//***************************************************************************

//***************************************************************************
//***************************************************************************
//***************************************************************************
// ARCHIVOS INDEXADOS — Inserción de cabecera (nueva playlist) — BEGIN

void InterfazUsuario::dialogCrearPlaylist(std::function<void(const Playlist&)> cb) {
    QDialog dlg; dlg.setModal(true); dlg.setWindowTitle("Nueva playlist");
    dlg.setStyleSheet("QDialog{background:#151515;color:white;} QLabel, QLineEdit{color:white;}");
    QVBoxLayout *v = new QVBoxLayout(&dlg);

    QLineEdit *edNombre = new QLineEdit; edNombre->setPlaceholderText("Nombre de la playlist");
    QLabel *prev = coverLabel(QString(), QSize(320,200), true);
    QPushButton *btnImagen = new QPushButton("Seleccionar imagen…");

    v->addWidget(new QLabel("Nombre:")); v->addWidget(edNombre);
    v->addWidget(new QLabel("Carátula:")); v->addWidget(prev);
    v->addWidget(btnImagen);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    v->addWidget(bb);

    QString rutaImg;
    QObject::connect(btnImagen, &QPushButton::clicked, &dlg, [&](){
        QString r = QFileDialog::getOpenFileName(&dlg, "Seleccionar imagen", QString(),
                                                 "Imágenes (*.png *.jpg *.jpeg)");
        if (!r.isEmpty()) { rutaImg = r; prev->setPixmap(fitOrCover(rutaImg, prev->size(), true)); }
    });

    QObject::connect(bb, &QDialogButtonBox::accepted, &dlg, [&](){
        if (edNombre->text().trimmed().isEmpty()) { QMessageBox::warning(&dlg,"Playlist","Ponle un nombre."); return; }
        auto pls = leerPlaylists();
        Playlist p; p.id = QDateTime::currentMSecsSinceEpoch(); p.nombre = edNombre->text().trimmed(); p.portada = rutaImg; p.activa = true;
        pls.append(p); guardarPlaylists(pls);
        if (cb) cb(p);
        dlg.accept();
    });
    QObject::connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    dlg.exec();
}

// ARCHIVOS INDEXADOS — Inserción de cabecera (nueva playlist) — END
//***************************************************************************
//***************************************************************************
//***************************************************************************



//***************************************************************************
//***************************************************************************
//***************************************************************************
// LISTAS ENLAZADAS — Canciones de la playlist (playlistId) — BEGIN

void InterfazUsuario::abrirPlaylist(const Playlist &p) {
    QDialog dlg;
    dlg.setModal(true);
    dlg.resize(760, 540);
    dlg.setWindowTitle(p.nombre);
    dlg.setStyleSheet("QDialog{background:#151515;color:white;} QLabel{color:white;}");

    QVBoxLayout *root = new QVBoxLayout(&dlg);
    root->setContentsMargins(12,12,12,12);
    root->setSpacing(10);

    // Header con carátula y título
    QWidget *hdr = new QWidget;
    QHBoxLayout *hh = new QHBoxLayout(hdr);
    hh->setContentsMargins(0,0,0,0);
    hh->setSpacing(12);
    QLabel *imgHdr = coverLabel(p.portada, QSize(120,120), true);
    QLabel *ttlHdr = new QLabel("Playlist: " + p.nombre);
    ttlHdr->setStyleSheet("font-size:20px;font-weight:800;");
    ttlHdr->setWordWrap(true);
    hh->addWidget(imgHdr);
    hh->addWidget(ttlHdr, 1);
    root->addWidget(hdr);

    //***************************************************************************
    //***************************************************************************
    //***************************************************************************
    // ARCHIVOS INDEXADOS — Lectura de items (playlistId,cancionId) — BEGIN

    // Tomar los IDs de canciones pertenecientes a la playlist
    QSet<qint64> ids;
    {
        auto items = leerPlaylistItems();
        for (const auto &it : items)
            if (it.playlistId == p.id) ids.insert(it.cancionId);
    }

    // Asegurar catálogo en memoria
    if (cacheCanciones.isEmpty()) {
        ManejadorCanciones mc;
        cacheCanciones = mc.obtenerTodas();
    }

    // ARCHIVOS INDEXADOS — Lectura de items (playlistId,cancionId) — END
    //***************************************************************************
    //***************************************************************************
    //***************************************************************************

    // Contenedor con scroll
    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget *contenedor = new QWidget;
    QVBoxLayout *v = new QVBoxLayout(contenedor);
    v->setContentsMargins(0,0,0,0);
    v->setSpacing(4);

    int n = 1;
    for (const Cancion &c : cacheCanciones) {
        if (!ids.contains(c.id) || !c.activo) continue;

        QWidget *row = new QWidget;
        QHBoxLayout *r = new QHBoxLayout(row);
        r->setContentsMargins(8,6,8,6);
        r->setSpacing(10);

        QLabel *num = new QLabel(QString::number(n++));
        QLabel *img = coverLabel(c.portada, QSize(56,56), true);
        QLabel *tit = new QLabel(c.titulo);
        tit->setWordWrap(true);
        QLabel *dur = new QLabel(c.duracion.isEmpty() ? "--:--" : c.duracion);
        dur->setMinimumWidth(48);
        dur->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        r->addWidget(num);
        r->addWidget(img);
        r->addWidget(tit, 1);

        //***************************************************************************
        //***************************************************************************
        //***************************************************************************
        // HASHTABLE (Favoritos / Descargas) — USO EN UI (estado y toggle) — BEGIN

        auto mkBtn = [](const QString& s){
            auto *b = new QPushButton(s);
            b->setFlat(true);
            b->setCursor(Qt::PointingHandCursor);
            b->setStyleSheet("QPushButton{font-size:16px;color:#bbb;} QPushButton:hover{color:white;}");
            return b;
        };
        QPushButton *aFav  = mkBtn("❤");
        QPushButton *aAdd  = mkBtn("+");
        QPushButton *aDown = mkBtn("⤓");

        // Estado inicial ❤️
        {
            QSet<qint64> favs = leerFavoritos();
            aFav->setStyleSheet(QString("QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}")
                                    .arg(favs.contains(c.id) ? "#E74C3C" : "#bbb"));
        }
        // Estado inicial ⤓
        {
            QSet<qint64> downs = leerDescargas();
            aDown->setStyleSheet(QString("QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}")
                                     .arg(downs.contains(c.id) ? "#1DB954" : "#bbb"));
        }

        // Toggle favorito SOLO esta pista
        QObject::connect(aFav, &QPushButton::clicked, row, [=](bool){
            toggleFavorito(c.id, aFav);
        });

        // Menú “Agregar a playlist”
        QObject::connect(aAdd, &QPushButton::clicked, row, [=](bool){
            QMenu *m = menuAddToPlaylist(c.id);
            m->exec(QCursor::pos());
            m->deleteLater();
        });

        // Marcar/Desmarcar descarga
        QObject::connect(aDown, &QPushButton::clicked, row, [=](bool){
            QSet<qint64> s = leerDescargas();
            const bool on = !s.contains(c.id);
            if (on) s.insert(c.id); else s.remove(c.id);
            guardarDescargas(s);
            aDown->setStyleSheet(QString("QPushButton{font-size:16px;color:%1;} QPushButton:hover{color:white;}")
                                     .arg(on ? "#1DB954" : "#bbb"));
        });

        r->addWidget(aFav);
        r->addWidget(aAdd);
        r->addWidget(aDown);

        // HASHTABLE (Favoritos / Descargas) — USO EN UI (estado y toggle) — END
        //***************************************************************************
        //***************************************************************************
        //***************************************************************************

        r->addStretch();
        r->addWidget(dur);

        v->addWidget(row);

        // Doble clic → reproducir
        auto *f = new DblClickFilter([=](){ reproducirCancionMeta(c); }, row);
        row->installEventFilter(f);
        tit->installEventFilter(f);
        img->installEventFilter(f);

        //***************************************************************************
        //***************************************************************************
        //***************************************************************************
        // UTILIDADES — cálculo/relleno de duración — BEGIN (fila)

        if (c.duracion.isEmpty()) {
            asegurarDuracion(c, [dur](const QString &mmss){ dur->setText(mmss); });
        }

        // UTILIDADES — cálculo/relleno de duración — END (fila)
        //***************************************************************************
        //***************************************************************************
        //***************************************************************************
    }

    v->addStretch(1);
    contenedor->setLayout(v);
    scroll->setWidget(contenedor);
    root->addWidget(scroll, 1);

    // Botonera
    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Close);
    QObject::connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    root->addWidget(bb);

    dlg.exec();
}


// LISTAS ENLAZADAS — Canciones de la playlist (playlistId) — END
//***************************************************************************
//***************************************************************************
//***************************************************************************

//***************************************************************************
//***************************************************************************
//***************************************************************************
// UTILIDADES — cálculo/relleno de duración — BEGIN

QString InterfazUsuario::mmssFromMs(qint64 ms) {
    if (ms <= 0) return "--:--";
    qint64 total = ms / 1000;
    int m = int(total / 60);
    int s = int(total % 60);
    return QString("%1:%2").arg(m, 2, 10, QLatin1Char('0'))
        .arg(s, 2, 10, QLatin1Char('0'));
}

void InterfazUsuario::asegurarDuracion(Cancion c, std::function<void(const QString&)> aplicarUI) {
    // Si ya viene, úsala directo y persiste (por si faltaba en archivo)
    if (!c.duracion.isEmpty()) { if (aplicarUI) aplicarUI(c.duracion); return; }

    // Cálculo perezoso con QMediaPlayer (asíncrono y liviano)
    auto *mp = new QMediaPlayer(zonaCentralWidget);
    QObject::connect(mp, &QMediaPlayer::durationChanged, zonaCentralWidget, [=](qint64 ms){
        const QString mmss = mmssFromMs(ms);
        if (aplicarUI) aplicarUI(mmss);
        ManejadorCanciones mc; mc.actualizarDuracion(c.id, mmss);
        mp->deleteLater();
    });
    QObject::connect(mp, &QMediaPlayer::mediaStatusChanged, zonaCentralWidget, [=](QMediaPlayer::MediaStatus st){
        if (st == QMediaPlayer::InvalidMedia) {
            if (aplicarUI) aplicarUI("--:--");
            mp->deleteLater();
        }
    });
    mp->setSource(QUrl::fromLocalFile(c.rutaArchivo));
}

// UTILIDADES — cálculo/relleno de duración — END
//***************************************************************************
//***************************************************************************
//***************************************************************************

void InterfazUsuario::mostrarDescargas() {
    if (!zonaCentralWidget) return;
    limpiarCentro();

    QVBoxLayout *l = new QVBoxLayout(zonaCentralWidget);
    l->setContentsMargins(8,8,8,8);

    QLabel *t = new QLabel("📥 Descargas");
    t->setStyleSheet("font-size:20px;font-weight:bold;");
    l->addWidget(t);

    QSet<qint64> downs = leerDescargas();
    if (cacheCanciones.isEmpty()) { ManejadorCanciones mc; cacheCanciones = mc.obtenerTodas(); }

    QListWidget *lista = new QListWidget;
    for (const Cancion &c : cacheCanciones) {
        if (!c.activo || !downs.contains(c.id)) continue;
        auto *it = new QListWidgetItem(QString("%1    ·    %2")
                                           .arg(c.titulo, c.duracion.isEmpty()? "--:--" : c.duracion));
        it->setIcon(QIcon(fitOrCover(c.portada, QSize(64,64), true)));
        it->setData(Qt::UserRole, QVariant::fromValue(c.id));
        lista->addItem(it);

        if (c.duracion.isEmpty()) {
            // actualizar el texto de ese item cuando calcule
            const QString base = c.titulo;
            asegurarDuracion(c, [it, base](const QString& mmss){
                it->setText(QString("%1    ·    %2").arg(base, mmss));
            });
        }
    }
    l->addWidget(lista, 1);

    QObject::connect(lista, &QListWidget::itemDoubleClicked, lista, [=](QListWidgetItem *it){
        const qint64 sid = it->data(Qt::UserRole).toLongLong();
        for (const Cancion &c : cacheCanciones) if (c.id==sid) { reproducirCancionMeta(c); break; }
    });
}
