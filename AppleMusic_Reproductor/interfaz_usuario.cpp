#include "interfaz_usuario.h"
#include "cframe.h"
#include "ui_cframe.h"
#include "manejadorcanciones.h"
#include "usuarios.h"
#include "cancion.h"

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
#include <QDate>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QRandomGenerator>
#include <QtAlgorithms>
#include <QSpacerItem>
#include <QFile>
#include <QDataStream>

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

// ================== Ctor / armado base ==================
InterfazUsuario::InterfazUsuario(cframe *ventana)
    : ventanaPrincipal(ventana) {}

void InterfazUsuario::cargarEnPagina(QWidget *pagina) {
    pagina->setStyleSheet("background-color: #121212; color: white;");
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(pagina);
    layoutPrincipal->setContentsMargins(12, 22, 0, 0);

    // Reproductor (arriba)
    layoutPrincipal->addWidget(crearReproductor());

    // Cuerpo 3 columnas
    QHBoxLayout *cuerpo = new QHBoxLayout;
    cuerpo->addWidget(crearMenuIzquierdo(), 1);
    zonaCentral = crearZonaCentral();  // Home
    cuerpo->addWidget(zonaCentral, 3);
    cuerpo->addWidget(crearZonaDerecha(), 1);
    layoutPrincipal->addLayout(cuerpo);

    pagina->setLayout(layoutPrincipal);
}

// ================== Reproductor (top) ==================
QWidget* InterfazUsuario::crearReproductor() {
    QWidget *w = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout(w);
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

    QLabel *lblPerfil = new QLabel("👤 Mi perfil");
    lblPerfil->setStyleSheet("font-weight: bold;");
    lay->addWidget(lblPerfil);

    QLineEdit *buscador = new QLineEdit;
    buscador->setPlaceholderText("🔎 Buscar...");
    buscador->setStyleSheet("background-color: white; color: black;");
    lay->addWidget(buscador);

    btnHome       = new QPushButton("🏠 Home");
    btnBiblioteca = new QPushButton("📚 Biblioteca");
    btnFavoritos  = new QPushButton("❤️ Favoritos");
    btnPlaylists  = new QPushButton("🎶 Playlists");
    btnDescargas  = new QPushButton("📁 Descargas");

    QList<QPushButton*> botones = {btnHome, btnBiblioteca, btnFavoritos, btnPlaylists, btnDescargas};
    for (QPushButton *btn : botones) {
        btn->setStyleSheet("text-align: left; padding: 6px; border-radius:6px;");
        lay->addWidget(btn);
    }

    // Conexiones
    QObject::connect(btnHome,       &QPushButton::clicked, [=]() { construirHome();             });
    QObject::connect(btnBiblioteca, &QPushButton::clicked, [=]() { mostrarVistaBiblioteca();    });
    QObject::connect(btnFavoritos,  &QPushButton::clicked, [=]() { mostrarVistaFavoritos();     });
    QObject::connect(btnPlaylists,  &QPushButton::clicked, [=]() { mostrarVistaPlaylists();     });
    QObject::connect(btnDescargas,  &QPushButton::clicked, [=]() { mostrarVistaDescargas();     });

    lay->addStretch();
    return menu;
}

// ================== Zona central (HOME) ==================
QWidget* InterfazUsuario::crearZonaCentral() {
    QWidget *centro = new QWidget;
    centro->setObjectName("ZonaCentralUsuario");
    QVBoxLayout *lay = new QVBoxLayout(centro);
    lay->setSpacing(14);
    lay->setContentsMargins(0, 0, 0, 0);

    zonaCentral = centro;
    construirHome();     // Vista inicial

    return centro;
}

static void mezclar(QList<Cancion> &v) {
    if (!v.isEmpty()) std::shuffle(v.begin(), v.end(), *QRandomGenerator::global());
}

void InterfazUsuario::construirHome() {
    if (!zonaCentral) return;

    // Limpia lo previo
    if (QLayout *old = zonaCentral->layout()) { delete old; }
    QVBoxLayout *root = new QVBoxLayout(zonaCentral);
    root->setSpacing(18);
    root->setContentsMargins(8, 0, 8, 0);

    // ------------- Datos base -------------
    ManejadorCanciones mc;
    QList<Cancion> todas = mc.obtenerTodas();

    // Género preferido del usuario activo (tomaremos el 1er usuario común activo si no hay sesión)
    QString generoPreferido;
    {
        ManejadorUsuarios mu;
        for (const Usuario &u : mu.obtenerUsuarios()) {
            if (u.tipo == UsuarioComun && u.activo) { generoPreferido = u.generoPreferido; break; }
        }
    }

    // Filtrados para HOME
    QList<Cancion> delGenero, otros, recientes = todas;
    for (const Cancion &c : todas) {
        if (!c.activo) continue;
        if (generoPreferido.isEmpty() || c.genero == generoPreferido) delGenero << c;
        if (!generoPreferido.isEmpty() && c.genero != generoPreferido) otros << c;
    }

    // RECIÉN ESCUCHADOS: a partir de reproducciones.dat (últimos distintos)
    QList<Reproduccion> reps = leerReproducciones();
    std::sort(recientes.begin(), recientes.end(),
              [](const Cancion&a, const Cancion&b){ return a.fechaCarga > b.fechaCarga; });
    // Si hay registros de reproducción, usar eso en lugar de fechaCarga
    QList<Cancion> recienEscuchados;
    if (!reps.isEmpty()) {
        // ordenar de más reciente a más antiguo
        std::sort(reps.begin(), reps.end(), [](const Reproduccion&a, const Reproduccion&b){ return a.fecha > b.fecha; });
        QSet<qint64> vistos;
        for (const Reproduccion &r : reps) {
            if (vistos.contains(r.cancionId)) continue;
            for (const Cancion &c : todas) if (c.id==r.cancionId && c.activo) { recienEscuchados << c; break; }
            vistos.insert(r.cancionId);
            if (recienEscuchados.size()>=8) break;
        }
    }

    mezclar(delGenero);
    mezclar(otros);

    // ============= Sección: RECOMENDADOS =============
    // Pediste: artistas del género preferido + carátulas (álbum/EP/single) → 8 tarjetas horizontales
    QList<QWidget*> tarjetasRec;
    // 1) Canciones del género
    for (int i=0; i<delGenero.size() && (int)tarjetasRec.size()<8; ++i)
        tarjetasRec << crearTarjetaCancion(delGenero[i]);
    // 2) Artistas únicos del género (si aún faltan para 8)
    if ((int)tarjetasRec.size()<8) {
        QSet<QString> artistas;
        for (const Cancion &c : delGenero) artistas.insert(c.artista);
        for (const QString &a : artistas) {
            if ((int)tarjetasRec.size()>=8) break;
            // portada hint: usa la primera coincidencia
            QString portada;
            for (const Cancion &c : delGenero) if (c.artista==a) { portada=c.portada; break; }
            tarjetasRec << crearTarjetaArtista(a, portada);
        }
    }
    root->addWidget(crearSeccionHorizontal(
        QString("⭐ Recomendados %1").arg(generoPreferido.isEmpty()? "" : "· " + generoPreferido), tarjetasRec));

    // ============= Sección: RECIÉN ESCUCHADOS =============
    QList<QWidget*> tarjetasRecent;
    const QList<Cancion> fuenteRecientes = recienEscuchados.isEmpty()? recientes : recienEscuchados;
    for (int i=0; i<qMin(8, fuenteRecientes.size()); ++i)
        tarjetasRecent << crearTarjetaCancion(fuenteRecientes[i]);
    root->addWidget(crearSeccionHorizontal("🕒 Recién escuchados", tarjetasRecent));

    // ============= Sección: EXPLORAR (aleatorio de todos los géneros) =============
    // Mezclamos “otros” + añadimos artistas únicos de “otros”
    QList<QWidget*> tarjetasExplorar;
    QList<Cancion> pool = otros; mezclar(pool);
    for (int i=0; i<pool.size() && (int)tarjetasExplorar.size()<8; ++i)
        tarjetasExplorar << crearTarjetaCancion(pool[i]);

    if ((int)tarjetasExplorar.size()<8) {
        QSet<QString> artistasUnicos;
        for (const Cancion &c : todas) {
            if (generoPreferido.isEmpty() || c.genero != generoPreferido) {
                if (artistasUnicos.contains(c.artista)) continue;
                artistasUnicos.insert(c.artista);
                tarjetasExplorar << crearTarjetaArtista(c.artista, c.portada);
                if ((int)tarjetasExplorar.size()>=8) break;
            }
        }
    }
    root->addWidget(crearSeccionHorizontal("🔎 Explorar", tarjetasExplorar));

    root->addStretch();
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

    // Doble clic → si es álbum/EP con coleccion → reproducir completo; si es Single → reproducir pista
    auto onDbl = [=]() {
        if (!c.coleccion.isEmpty() && (c.tipo == "Álbum" || c.tipo == "EP"))
            reproducirColeccionCompleta(c.coleccion, c.tipo, c.artista);
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

QWidget* InterfazUsuario::crearTarjetaArtista(const QString &artista, const QString &portadaHint) {
    QWidget *card = new QWidget;
    card->setFixedWidth(180);
    QVBoxLayout *v = new QVBoxLayout(card);
    v->setSpacing(8);

    QLabel *img = coverLabel(portadaHint, QSize(180,180), /*cover=*/true);
    v->addWidget(img);

    QLabel *lblTitulo = new QLabel(artista);
    lblTitulo->setStyleSheet("font-weight:700; font-size:15px;");
    lblTitulo->setWordWrap(true);
    v->addWidget(lblTitulo);

    // Doble clic → abrir vista del artista (colecciones + singles)
    auto onDbl = [=]() { abrirArtistaUsuario(artista); };
    auto *f = new DblClickFilter(onDbl, card);
    card->installEventFilter(f); img->installEventFilter(f); lblTitulo->installEventFilter(f);

    card->setStyleSheet("QWidget{background:#1e1e1e; border-radius:12px;} QWidget:hover{background:#262626;}");
    return card;
}

QWidget* InterfazUsuario::crearTarjetaColeccion(const QString &nombreColeccion, const QString &portada, const QString &tipo) {
    QWidget *card = new QWidget;
    card->setFixedWidth(220);
    QVBoxLayout *v = new QVBoxLayout(card);
    v->addWidget(coverLabel(portada, QSize(220,220), true));
    QLabel *t = new QLabel(QString("%1 · %2").arg(nombreColeccion, tipo));
    t->setWordWrap(true);
    v->addWidget(t);

    // Doble clic → reproducir la colección completa
    auto onDbl = [=]() { reproducirColeccionCompleta(nombreColeccion, tipo, QString()); };
    auto *f = new DblClickFilter(onDbl, card);
    card->installEventFilter(f); t->installEventFilter(f);

    return card;
}

// ---------- Navegación (diálogos informativos, por si se usan en otros flujos) ----------
void InterfazUsuario::abrirColeccionUsuario(const QString &nombreColeccion, const QString &tipo, const QString &artista) {
    // (Se mantiene por si lo usas desde otra vista; en Home ahora reproducimos directo)
    QDialog dlg;
    dlg.setWindowTitle(nombreColeccion);
    dlg.setModal(true);
    dlg.resize(600, 540);
    dlg.setStyleSheet("QDialog{background:#151515;color:white;} QLabel{color:white;}");

    QVBoxLayout *root = new QVBoxLayout(&dlg);

    // Header
    QWidget *header = new QWidget; QHBoxLayout *h = new QHBoxLayout(header);
    QLabel *art = coverLabel(QString(), QSize(180,180), true);
    QLabel *lblTitulo = new QLabel(nombreColeccion); lblTitulo->setStyleSheet("font-size:20px;font-weight:700;");
    QLabel *lblTipo = new QLabel(QString("Tipo: %1").arg(tipo));
    QVBoxLayout *info = new QVBoxLayout; info->addWidget(lblTitulo); info->addWidget(lblTipo);
    h->addWidget(art); h->addLayout(info,1);
    root->addWidget(header);

    // Lista de canciones de esa colección
    auto *scroll = new SmartScrollAreaUser; scroll->setAutoHide(false,true);
    QWidget *contenedor = new QWidget; QVBoxLayout *v = new QVBoxLayout(contenedor);

    ManejadorCanciones mc;
    QList<Cancion> todas = mc.obtenerTodas();
    int i=1;
    for (const Cancion &c : todas) {
        if (!c.activo) continue;
        if (c.coleccion == nombreColeccion && (artista.isEmpty() || c.artista == artista)) {
            QWidget *row = new QWidget; QHBoxLayout *r = new QHBoxLayout(row);
            QLabel *num = new QLabel(QString::number(i++) + ".");
            num->setFixedWidth(26);
            QLabel *tit = new QLabel(c.titulo);
            QLabel *dur = new QLabel(c.duracion);
            QPushButton *play = new QPushButton("▶");
            play->setFixedWidth(36);
            r->addWidget(num); r->addWidget(tit,1); r->addWidget(dur); r->addWidget(play);
            v->addWidget(row);
            QObject::connect(play, &QPushButton::clicked, [=]() { reproducirCancion(c.rutaArchivo); });
            auto *f = new DblClickFilter([=](){ reproducirCancion(c.rutaArchivo); }, row);
            row->installEventFilter(f); tit->installEventFilter(f);
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

    ManejadorCanciones mc;
    QList<Cancion> todas = mc.obtenerTodas();

    auto tarjetasPorTipo = [&](const QString &tipo){
        QList<QWidget*> tarjetas;
        QSet<QString> coleccionesVistas; // para no duplicar tarjetas de álbum/EP
        for (const Cancion &c : todas) {
            if (!c.activo || c.artista != artista) continue;
            if (tipo == "Single") {
                if (c.tipo == "Single") tarjetas << crearTarjetaCancion(c);
            } else {
                if (c.tipo == tipo && !c.coleccion.isEmpty() && !coleccionesVistas.contains(c.coleccion)) {
                    coleccionesVistas.insert(c.coleccion);
                    tarjetas << crearTarjetaColeccion(c.coleccion, c.portada, tipo);
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

// ================== Zona derecha (info + rate placeholder) ==================
QWidget* InterfazUsuario::crearZonaDerecha() {
    QWidget *derecha = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(derecha);

    QLabel *img = new QLabel;
    img->setPixmap(QPixmap(":/img/default.jpg").scaled(100, 100, Qt::KeepAspectRatio));
    img->setFixedSize(120, 120);
    img->setStyleSheet("background-color: #2a2a2a;");
    lay->addWidget(img, 0, Qt::AlignCenter);

    QLabel *lblTitulo = new QLabel("🎵 Título de la canción");
    QLabel *lblArtista = new QLabel("👤 Artista");
    QLabel *lblDuracion = new QLabel("🕒 Duración");
    for (QLabel *l : {lblTitulo, lblArtista, lblDuracion}) {
        l->setStyleSheet("color: white;");
        lay->addWidget(l, 0, Qt::AlignCenter);
    }

    QLabel *lbl = new QLabel("⭐ Califica esta canción");
    lbl->setStyleSheet("font-weight: bold;");
    lay->addWidget(lbl);

    for (int i = 1; i <= 5; ++i) {
        QRadioButton *rb = new QRadioButton(QString("⭐ %1 estrellas").arg(i));
        lay->addWidget(rb);
    }

    QPushButton *btnEnviar = new QPushButton("Enviar Calificación");
    lay->addWidget(btnEnviar);

    lay->addStretch();
    return derecha;
}

// ================== Vistas menú (centro cambia) ==================
void InterfazUsuario::limpiarZonaCentral() {
    if (!zonaCentral) return;
    if (QLayout *old = zonaCentral->layout()) { delete old; }
}

void InterfazUsuario::mostrarVistaBiblioteca() {
    limpiarZonaCentral();
    QVBoxLayout *lay = new QVBoxLayout(zonaCentral);
    QLabel *titulo = new QLabel("📚 Tu Biblioteca");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    lay->addWidget(titulo);

    QListWidget *lista = new QListWidget;
    lista->setStyleSheet("color: white;");

    ManejadorCanciones manejador;
    QList<Cancion> canciones = manejador.obtenerTodas();
    for (const Cancion &c : canciones) {
        if (!c.activo) continue;
        QListWidgetItem *item = new QListWidgetItem(QString("🎵 %1 - %2").arg(c.titulo, c.artista));
        item->setData(Qt::UserRole, c.rutaArchivo);
        lista->addItem(item);
    }

    lista->setMaximumHeight(250);
    lay->addWidget(lista);
    lay->addStretch();

    QObject::connect(lista, &QListWidget::itemDoubleClicked, [=](QListWidgetItem *item) {
        reproducirCancion(item->data(Qt::UserRole).toString());
    });
}

void InterfazUsuario::mostrarVistaFavoritos() {
    limpiarZonaCentral();
    QVBoxLayout *lay = new QVBoxLayout(zonaCentral);
    QLabel *titulo = new QLabel("❤️ Tus Favoritos");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    lay->addWidget(titulo);
    QLabel *msg = new QLabel("📌 Aquí se mostrarán tus canciones favoritas.");
    msg->setStyleSheet("color: white;");
    lay->addWidget(msg);
    lay->addStretch();
}

void InterfazUsuario::mostrarVistaPlaylists() {
    limpiarZonaCentral();
    QVBoxLayout *lay = new QVBoxLayout(zonaCentral);
    QLabel *titulo = new QLabel("🎶 Tus Playlists");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    lay->addWidget(titulo);
    QLabel *msg = new QLabel("📌 Aquí se mostrarán tus listas de reproducción personalizadas.");
    msg->setStyleSheet("color: white;");
    lay->addWidget(msg);
    lay->addStretch();
}

void InterfazUsuario::mostrarVistaDescargas() {
    limpiarZonaCentral();
    QVBoxLayout *lay = new QVBoxLayout(zonaCentral);
    QLabel *titulo = new QLabel("📁 Tus Descargas");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    lay->addWidget(titulo);
    QLabel *msg = new QLabel("📌 Aquí se mostrarán las canciones descargadas.");
    msg->setStyleSheet("color: white;");
    lay->addWidget(msg);
    lay->addStretch();
}

// ================== Reproducción ==================
void InterfazUsuario::conectarAvanceAutomatico() {
    if (conexionSiguienteHecha) return;
    conexionSiguienteHecha = true;

    QMediaPlayer *player = ventanaPrincipal->findChild<QMediaPlayer *>();
    if (!player) return;

    QObject::connect(player, &QMediaPlayer::mediaStatusChanged, ventanaPrincipal, [=](QMediaPlayer::MediaStatus st){
        if (st == QMediaPlayer::EndOfMedia && idxCola >= 0) {
            idxCola++;
            if (idxCola < colaReproduccion.size()) {
                player->setSource(QUrl::fromLocalFile(colaReproduccion[idxCola]));
                player->play();
            } else {
                // fin de la cola
                idxCola = -1;
                colaReproduccion.clear();
            }
        }
    });
}

void InterfazUsuario::reproducirColeccionCompleta(const QString &coleccion, const QString &tipo, const QString &artista) {
    ManejadorCanciones mc;
    QList<Cancion> pistas = obtenerCancionesPorColeccion(coleccion, artista, tipo);
    if (pistas.isEmpty()) { QMessageBox::warning(nullptr, "Colección", "No hay pistas en la colección."); return; }

    colaReproduccion.clear();
    for (const Cancion& c : pistas) if (!c.rutaArchivo.isEmpty()) colaReproduccion << c.rutaArchivo;
    if (colaReproduccion.isEmpty()) { QMessageBox::warning(nullptr,"Colección","Pistas sin ruta de archivo."); return; }

    conectarAvanceAutomatico();

    QMediaPlayer *player = ventanaPrincipal->findChild<QMediaPlayer *>();
    QAudioOutput *audio = ventanaPrincipal->findChild<QAudioOutput *>();
    if (!player || !audio) { QMessageBox::warning(nullptr,"Audio","No se encontró el reproductor."); return; }

    idxCola = 0;
    player->setSource(QUrl::fromLocalFile(colaReproduccion[idxCola]));
    audio->setVolume(ventanaPrincipal->getSliderVolumen()->value() / 100.0);
    player->play();
}

void InterfazUsuario::reproducirCancion(const QString &ruta) {
    if (ruta.isEmpty()) {
        QMessageBox::warning(nullptr, "Error", "Ruta de archivo vacía.");
        return;
    }

    // Si había una cola en curso la “apagamos”
    idxCola = -1; colaReproduccion.clear();

    QMediaPlayer *player = ventanaPrincipal->findChild<QMediaPlayer *>();
    QAudioOutput *audio = ventanaPrincipal->findChild<QAudioOutput *>();

    if (!player || !audio) {
        QMessageBox::warning(nullptr, "Error", "No se encontró el reproductor de audio.");
        return;
    }

    player->setSource(QUrl::fromLocalFile(ruta));
    audio->setVolume(ventanaPrincipal->getSliderVolumen()->value() / 100.0);
    player->play();
}

// ================== Datos auxiliares ==================
QList<InterfazUsuario::Reproduccion> InterfazUsuario::leerReproducciones() const {
    QList<Reproduccion> list; QFile f("reproducciones.dat");
    if (!f.open(QIODevice::ReadOnly)) return list;
    QDataStream in(&f);
    while (!in.atEnd()) { Reproduccion r; in >> r.cancionId >> r.userId >> r.ms >> r.fecha; list.append(r); }
    return list;
}

QList<Cancion> InterfazUsuario::obtenerCancionesTodas() const {
    ManejadorCanciones mc; return mc.obtenerTodas();
}

QList<Cancion> InterfazUsuario::obtenerCancionesPorArtista(const QString& artista) const {
    ManejadorCanciones mc; return mc.obtenerPorArtista(artista);
}

QList<Cancion> InterfazUsuario::obtenerCancionesPorColeccion(const QString& col, const QString& artista, const QString& tipo) const {
    ManejadorCanciones mc;
    QList<Cancion> res;
    for (const Cancion &c : mc.obtenerTodas()) {
        if (!c.activo) continue;
        if (c.coleccion == col && (artista.isEmpty() || c.artista == artista) && (tipo.isEmpty() || c.tipo==tipo))
            res << c;
    }
    // orden simple por título para reproducciones consistentes
    std::sort(res.begin(), res.end(), [](const Cancion &a, const Cancion &b){ return a.titulo.toLower() < b.titulo.toLower(); });
    return res;
}
