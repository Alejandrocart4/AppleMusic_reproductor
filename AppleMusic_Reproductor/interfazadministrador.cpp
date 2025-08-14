#include "interfazadministrador.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QFormLayout>
#include <QPixmap>
#include <QStackedWidget>
#include <QFile>
#include <QDataStream>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QImageReader>
#include <QGridLayout>
#include <QPainter>
#include <QStyleOption>
#include <QDialogButtonBox>
#include <QPainterPath>
#include <QEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QListWidget>
#include <QSet>
#include <QMap>
#include <algorithm>

// ===== Qt Charts =====
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>

// Necesario para usar cframe::regresarAlInicio()
#include "cframe.h"

#include "cancion.h"
#include "manejadorcanciones.h"
#include "usuarios.h"

// =================== Utils ===================

static QPixmap circularPixmapFromFile(const QString &ruta, const QSize &tam) {
    QPixmap dst(tam);
    dst.fill(Qt::transparent);
    QPainter p(&dst);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path; path.addEllipse(QRectF(QPointF(0,0), tam));
    p.setClipPath(path);
    QPixmap src;
    if (!ruta.isEmpty() && QFile::exists(ruta)) src.load(ruta);
    if (!src.isNull())
        p.drawPixmap(0,0, src.scaled(tam, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    else
        p.fillRect(QRect(QPoint(0,0), tam), QColor("#444"));
    p.end();
    return dst;
}

bool InterfazAdministrador::esImagenValida(const QString &ruta) {
    if (ruta.isEmpty() || !QFile::exists(ruta)) return false;
    QImageReader r(ruta);
    return r.canRead();
}

static QString formatoTiempoLargo(qint64 ms) {
    qint64 s = ms / 1000;
    qint64 h = s / 3600;
    qint64 m = (s % 3600) / 60;
    return QString("%1h %2m").arg(h).arg(m);
}

// =================== Constructor / Setters ===================

InterfazAdministrador::InterfazAdministrador(QWidget *parent)
    : QWidget(parent) {}

void InterfazAdministrador::setNombreArtista(const QString &nombre) {
    nombreArtistaLogueado = nombre;
}

void InterfazAdministrador::setPaginaPrincipal(cframe *pagina) {
    paginaPrincipal = pagina;
}

// =================== Cargar página ===================

void InterfazAdministrador::cargarEnPagina(QWidget *pagina) {
    pagina->setStyleSheet("background-color: #121212; color: white;");
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(pagina);
    layoutPrincipal->setContentsMargins(12, 22, 12, 12);

    QHBoxLayout *cuerpo = new QHBoxLayout;

    // panel izquierdo
    cuerpo->addWidget(crearMenuIzquierdo(), 1);

    // ---- Centro con SCROLL VERTICAL (auto-hide) ----
    SmartScrollArea *scrollCentro = new SmartScrollArea;
    scrollCentro->setAutoHide(false, true);          // solo vertical
    scrollCentro->setAutoHideDelay(1200);
    zonaCentral = crearZonaCentral();
    scrollCentro->setWidget(zonaCentral);
    cuerpo->addWidget(scrollCentro, 3);

    zonaDerecha  = crearZonaDerecha();
    cuerpo->addWidget(zonaDerecha, 1);

    layoutPrincipal->addLayout(cuerpo);
    pagina->setLayout(layoutPrincipal);

    // Vista inicial: Home
    slotHome();
    refrescarEstadisticasDerecha();
}

// =================== Construcción UI ===================

// -------- PERFIL (header) ----------
QWidget* InterfazAdministrador::crearHeaderPerfilEnMenu() {
    // Carga datos del admin actual
    ManejadorUsuarios mu;
    QList<Usuario> usuarios = mu.obtenerUsuarios();
    Usuario artista; bool ok=false;
    for (const Usuario &u : usuarios) {
        if (u.tipo == Administrador && u.nombreArtistico == nombreArtistaLogueado) { artista = u; ok = true; break; }
    }

    // Tarjeta clicable (abre Perfil)
    hdrBtn = new QPushButton;
    hdrBtn->setObjectName("HeaderPerfilAdmin");
    hdrBtn->setCursor(Qt::PointingHandCursor);
    hdrBtn->setFlat(true);
    hdrBtn->setMinimumHeight(120);                         // ← evita que se corte en vertical
    hdrBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    hdrBtn->setStyleSheet(
        "QPushButton#HeaderPerfilAdmin{background:#1e1e1e;border-radius:12px;}"
        "QPushButton#HeaderPerfilAdmin:hover{background:#292929;}"
        );

    // Contenido interno
    QWidget *contenedor = new QWidget(hdrBtn);
    contenedor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QVBoxLayout *root = new QVBoxLayout(contenedor);
    root->setContentsMargins(12,12,12,12);
    root->setSpacing(6);

    QLabel *etiqueta = new QLabel("Panel Administrador");
    etiqueta->setStyleSheet("font-size:12px;color:#aaa;");

    QHBoxLayout *fila = new QHBoxLayout;
    fila->setSpacing(12);

    hdrAvatar = new QLabel;
    hdrAvatar->setFixedSize(72,72);                        // avatar grande
    hdrAvatar->setStyleSheet("border-radius:36px; background:#333;");
    hdrAvatar->setPixmap(circularPixmapFromFile(ok? artista.rutaImagen : QString(), hdrAvatar->size()));

    hdrNombre = new QLabel(ok? artista.nombreArtistico : nombreArtistaLogueado);
    hdrNombre->setWordWrap(true);                          // ← NO se corta el texto
    hdrNombre->setStyleSheet("font-size:20px;font-weight:700;");

    fila->addWidget(hdrAvatar);
    fila->addWidget(hdrNombre, 1);

    root->addWidget(etiqueta);
    root->addLayout(fila);

    // Montaje en el botón
    QVBoxLayout *btnLay = new QVBoxLayout(hdrBtn);
    btnLay->setContentsMargins(0,0,0,0);
    btnLay->addWidget(contenedor);

    // Acción
    connect(hdrBtn, &QPushButton::clicked, this, &InterfazAdministrador::slotMostrarPerfil);

    return hdrBtn;
}

void InterfazAdministrador::refreshHeaderPerfil() {
    if (!hdrAvatar || !hdrNombre) return;

    ManejadorUsuarios mu;
    QList<Usuario> usuarios = mu.obtenerUsuarios();
    Usuario artista; bool ok=false;
    for (const Usuario &u : usuarios) {
        if (u.tipo == Administrador && u.nombreArtistico == nombreArtistaLogueado) { artista = u; ok=true; break; }
    }

    hdrNombre->setText(ok ? artista.nombreArtistico : nombreArtistaLogueado);
    hdrAvatar->setPixmap(circularPixmapFromFile(ok? artista.rutaImagen : QString(), hdrAvatar->size()));
}

void InterfazAdministrador::setHeaderAvatarPreview(const QString &ruta) {
    if (hdrAvatar && !ruta.isEmpty())
        hdrAvatar->setPixmap(circularPixmapFromFile(ruta, hdrAvatar->size()));
}
// -----------------------------------

QWidget* InterfazAdministrador::crearMenuIzquierdo() {
    // Panel contenedor transparente
    QWidget *panel = new QWidget;
    panel->setMinimumWidth(280); // ← asegura espacio para que el header no se corte
    QVBoxLayout *root = new QVBoxLayout(panel);
    root->setContentsMargins(12, 22, 12, 12);
    root->setSpacing(12);

    // 1) Tarjeta superior del PERFIL (fuera del contenedor de botones)
    root->addWidget(crearHeaderPerfilEnMenu());

    // 2) Tarjeta para los BOTONES (ahora sí el “slot de botones”)
    QWidget *cardBotones = new QWidget;
    cardBotones->setStyleSheet("background:#1e1e1e; border-radius:10px;");
    QVBoxLayout *layout = new QVBoxLayout(cardBotones);
    layout->setContentsMargins(12, 18, 12, 12);
    layout->setSpacing(10);

    auto mkBtn = [](const QString &t){
        auto *b=new QPushButton(t);
        b->setMinimumHeight(46);
        b->setStyleSheet(
            "QPushButton{background:#2a2a2a;color:white;padding:10px;border-radius:8px;"
            "font-size:16px; font-weight:600; text-align:center;}"
            "QPushButton:hover{background:#3a3a3a;}"
            "QPushButton:pressed{background:#454545;}"
            );
        b->setCursor(Qt::PointingHandCursor);
        return b;
    };

    QPushButton *btnHome      = mkBtn("🏠  Home");
    QPushButton *btnMis       = mkBtn("🎵  Mis Canciones");
    QPushButton *btnAgregar   = mkBtn("➕  Agregar Canción");
    QPushButton *btnEditar    = mkBtn("✏️  Editar / Eliminar");
    QPushButton *btnStats     = mkBtn("📊  Estadísticas");
    QPushButton *btnCrearAlb  = mkBtn("📀  Crear Álbum");
    QPushButton *btnCrearEP   = mkBtn("🎚️  Crear EP");
    QPushButton *btnCerrar    = mkBtn("🚪  Cerrar Sesión");

    for (auto *b : {btnHome,btnMis,btnAgregar,btnEditar,btnStats,btnCrearAlb,btnCrearEP,btnCerrar})
        layout->addWidget(b);

    // Conexiones
    connect(btnHome,     &QPushButton::clicked, this, &InterfazAdministrador::slotHome);
    connect(btnMis,      &QPushButton::clicked, this, &InterfazAdministrador::slotMisCanciones);
    connect(btnAgregar,  &QPushButton::clicked, this, &InterfazAdministrador::slotAgregarCancion);
    connect(btnEditar,   &QPushButton::clicked, this, &InterfazAdministrador::slotEditarEliminarCancion);
    connect(btnStats,    &QPushButton::clicked, this, &InterfazAdministrador::slotVerEstadisticas);
    connect(btnCrearAlb, &QPushButton::clicked, this, &InterfazAdministrador::slotCrearAlbum);
    connect(btnCrearEP,  &QPushButton::clicked, this, &InterfazAdministrador::slotCrearEP);
    connect(btnCerrar,   &QPushButton::clicked, this, &InterfazAdministrador::slotCerrarSesion);

    root->addWidget(cardBotones);
    root->addStretch();
    return panel;
}

QWidget* InterfazAdministrador::crearZonaCentral() {
    QWidget *zona = new QWidget;
    zona->setLayout(new QVBoxLayout);
    return zona;
}

QWidget* InterfazAdministrador::crearZonaDerecha() {
    QWidget *zona = new QWidget;
    zona->setStyleSheet("background-color: #1e1e1e; border-radius: 10px;");
    QVBoxLayout *layout = new QVBoxLayout(zona);

    QLabel *titulo = new QLabel("📊 Estadísticas");
    titulo->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(titulo);

    layout->addWidget(new QLabel("Total Canciones: 0"));
    layout->addWidget(new QLabel("Más Escuchada: -"));
    layout->addWidget(new QLabel("Promedio Calificación (global): -"));

    layout->addStretch();
    return zona;
}

void InterfazAdministrador::limpiarZonaCentral() {
    if (!zonaCentral) return;
    if (auto *lay = qobject_cast<QVBoxLayout*>(zonaCentral->layout())) {
        QLayoutItem *child;
        while ((child = lay->takeAt(0)) != nullptr) {
            if (child->widget()) child->widget()->deleteLater();
            delete child;
        }
    }
}

void InterfazAdministrador::limpiarResiduosEdicionGlobal() {
    QWidget *root = this;
    const auto contenedores = root->findChildren<QWidget*>("panelEdicionCanciones", Qt::FindChildrenRecursively);
    for (QWidget *w : contenedores) w->deleteLater();
}

// =================== HOME ===================

void InterfazAdministrador::construirHome() {
    limpiarResiduosEdicionGlobal();
    limpiarZonaCentral();

    auto *lay = qobject_cast<QVBoxLayout*>(zonaCentral->layout());

    construirSeccionSingles(lay);
    construirSeccionColecciones(lay, "Albums", "albumes.dat");
    construirSeccionColecciones(lay, "EPs", "eps.dat");

    lay->addStretch();
}

void InterfazAdministrador::construirSeccionSingles(QVBoxLayout *layout) {
    QLabel *titulo = new QLabel("Songs (Singles)");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold; margin: 8px 0 6px 0;");
    layout->addWidget(titulo);

    const int altoItem   = 64;
    const int porColumna = 3;

    SmartScrollArea *scroll = new SmartScrollArea;
    scroll->setAutoHide(true, false);
    scroll->setAutoHideDelay(1200);
    scroll->setFixedHeight(porColumna * altoItem + 32);

    QWidget *viewport = new QWidget;
    QHBoxLayout *h = new QHBoxLayout(viewport);
    h->setSpacing(12);
    h->setContentsMargins(0,0,0,0);

    ManejadorCanciones mc;
    QList<Cancion> singles;
    for (const Cancion &c : mc.obtenerPorArtista(nombreArtistaLogueado))
        if (c.activo && c.tipo == "Single") singles.append(c);

    for (int i = 0; i < singles.size(); i += porColumna) {
        QWidget *columna = new QWidget;
        QVBoxLayout *v = new QVBoxLayout(columna);
        v->setSpacing(6); v->setContentsMargins(0,0,0,0);

        int fin = qMin(i + porColumna, singles.size());
        for (int j = i; j < fin; ++j) v->addWidget(crearItemListaCancion(singles[j], altoItem));
        v->addStretch();

        columna->setFixedWidth(340);
        h->addWidget(columna);
    }
    h->addStretch();

    viewport->setLayout(h);
    scroll->setWidget(viewport);
    layout->addWidget(scroll);
}

QWidget* InterfazAdministrador::crearItemListaCancion(const Cancion &c, int alto) const {
    QWidget *fila = new QWidget;
    fila->setStyleSheet("QWidget{background:#1c1c1c;border-radius:8px;} QLabel{color:white;}");
    QHBoxLayout *h = new QHBoxLayout(fila);
    h->setContentsMargins(8,6,8,6);
    h->setSpacing(10);

    QLabel *thumb = new QLabel;
    thumb->setFixedSize(alto, alto);
    thumb->setStyleSheet("background:white;border-radius:6px;");
    if (esImagenValida(c.portada)) {
        QPixmap px(c.portada);
        thumb->setPixmap(px.scaled(thumb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else thumb->setText("No art");

    QLabel *lblTitulo = new QLabel(c.titulo);
    lblTitulo->setStyleSheet("font-size:15px;font-weight:700;");

    QString subt;
    if (c.tipo == "Single") subt = "Single";
    else if (c.tipo == "Álbum") subt = "Álbum: " + (c.coleccion.isEmpty()? "(sin nombre)": c.coleccion);
    else if (c.tipo == "EP")     subt = "EP: "     + (c.coleccion.isEmpty()? "(sin nombre)": c.coleccion);
    QLabel *lblSub = new QLabel(subt);
    lblSub->setStyleSheet("color:#bbb; font-size:12px;");

    QVBoxLayout *centro = new QVBoxLayout;
    centro->addWidget(lblTitulo);
    centro->addWidget(lblSub);

    h->addWidget(thumb);
    h->addLayout(centro, 1);
    return fila;
}

QList<InterfazAdministrador::Coleccion> InterfazAdministrador::leerColecciones(const QString &ruta) const {
    QList<Coleccion> lista;
    QFile f(ruta);
    if (!f.open(QIODevice::ReadOnly)) return lista;
    QDataStream in(&f);
    while (!in.atEnd()) {
        Coleccion c;
        in >> c.nombre >> c.descripcion >> c.rutaImagen >> c.activo;
        if (!in.atEnd()) {
            qint64 pos = f.pos();
            QString g; QDate fecha; in >> g >> fecha;
            if (in.status()==QDataStream::Ok) { c.genero=g; c.fecha=fecha; }
            else { in.setStatus(QDataStream::Ok); f.seek(pos); }
        }
        if (c.activo) lista.append(c);
    }
    return lista;
}

QWidget* InterfazAdministrador::crearTarjetaColeccionPequena(const Coleccion &col) const {
    QWidget *card = new QWidget;
    card->setStyleSheet("QWidget{background:#1c1c1c;border-radius:10px;} QLabel{color:white;}");
    QVBoxLayout *v = new QVBoxLayout(card);
    v->setContentsMargins(8,8,8,8); v->setSpacing(8);

    QLabel *art = new QLabel;
    art->setFixedSize(200, 200);
    art->setStyleSheet("background:white;border-radius:8px;");
    if (esImagenValida(col.rutaImagen)) {
        QPixmap px(col.rutaImagen);
        art->setPixmap(px.scaled(art->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else art->setText("No art");

    QLabel *nom = new QLabel(col.nombre.isEmpty()? "(sin nombre)": col.nombre);
    nom->setStyleSheet("font-weight:700; font-size:14px;");
    nom->setWordWrap(true);

    v->addWidget(art, 0, Qt::AlignCenter);
    v->addWidget(nom);
    card->setProperty("nombreColeccion", col.nombre);
    return card;
}

void InterfazAdministrador::construirSeccionColecciones(QVBoxLayout *layout, const QString &titulo, const QString &archivoColecciones) {
    QLabel *t = new QLabel(titulo);
    t->setStyleSheet("font-size: 20px; font-weight: bold; margin: 12px 0 6px 0;");
    layout->addWidget(t);

    SmartScrollArea *scroll = new SmartScrollArea;
    scroll->setAutoHide(true, false);
    scroll->setAutoHideDelay(1200);
    scroll->setFixedHeight(260);

    QWidget *viewport = new QWidget;
    QHBoxLayout *h = new QHBoxLayout(viewport);
    h->setSpacing(12);
    h->setContentsMargins(0,0,0,0);

    auto cols = leerColecciones(archivoColecciones);
    for (const auto &col : cols) {
        QWidget *columna = new QWidget;
        QVBoxLayout *v = new QVBoxLayout(columna);
        v->setSpacing(6); v->setContentsMargins(0,0,0,0);

        QWidget *card = crearTarjetaColeccionPequena(col);
        card->installEventFilter(this);
        card->setProperty("archivoColecciones", archivoColecciones);

        columna->setFixedWidth(220);
        v->addWidget(card);
        v->addStretch();
        h->addWidget(columna);
    }
    h->addStretch();

    viewport->setLayout(h);
    scroll->setWidget(viewport);
    layout->addWidget(scroll);
}

bool InterfazAdministrador::eventFilter(QObject *obj, QEvent *ev) {
    if (ev->type() == QEvent::MouseButtonDblClick) {
        QWidget *w = qobject_cast<QWidget*>(obj);
        if (w && w->property("archivoColecciones").isValid()) {
            abrirDialogoColeccion(w->property("archivoColecciones").toString(),
                                  w->property("nombreColeccion").toString());
            return true;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void InterfazAdministrador::abrirDialogoColeccion(const QString &archivoColecciones, const QString &nombreColeccion) {
    Coleccion objetivo; bool ok=false;
    for (const auto &c : leerColecciones(archivoColecciones))
        if (c.nombre == nombreColeccion) { objetivo = c; ok = true; break; }
    if (!ok) { QMessageBox::warning(this,"Colección","No se encontró la colección."); return; }

    QDialog dlg(this);
    dlg.setWindowTitle(nombreColeccion);
    dlg.setModal(true);
    dlg.resize(580, 540);
    dlg.setStyleSheet("QDialog{background:#151515;color:white;} QLabel{color:white;}");
    QVBoxLayout *root = new QVBoxLayout(&dlg);

    QWidget *header = new QWidget;
    QHBoxLayout *h = new QHBoxLayout(header);
    QLabel *art = new QLabel; art->setFixedSize(180,180);
    art->setStyleSheet("background:white;border-radius:10px;");
    if (esImagenValida(objetivo.rutaImagen)) {
        QPixmap px(objetivo.rutaImagen);
        art->setPixmap(px.scaled(art->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else art->setText("No art");

    QLabel *lblTitulo = new QLabel(objetivo.nombre);
    lblTitulo->setStyleSheet("font-size:20px;font-weight:700;");
    QLabel *lblGenero = new QLabel(QString("Género: %1").arg(objetivo.genero.isEmpty()? "-" : objetivo.genero));
    QString fechaTxt = objetivo.fecha.isValid()? objetivo.fecha.toString("dd 'de' MMMM 'de' yyyy") : "(sin fecha)";
    QLabel *lblFecha  = new QLabel(QString("Creado: %1").arg(fechaTxt));
    QLabel *lblDesc   = new QLabel(objetivo.descripcion); lblDesc->setWordWrap(true);

    QVBoxLayout *info = new QVBoxLayout;
    info->addWidget(lblTitulo); info->addWidget(lblGenero); info->addWidget(lblFecha); info->addWidget(lblDesc);
    h->addWidget(art); h->addLayout(info,1);
    root->addWidget(header);

    SmartScrollArea *scroll = new SmartScrollArea;
    scroll->setAutoHide(false, true);
    QWidget *contenedor = new QWidget;
    QVBoxLayout *v = new QVBoxLayout(contenedor);

    ManejadorCanciones mc;
    for (const Cancion &c : mc.obtenerPorArtista(nombreArtistaLogueado))
        if (c.activo && c.coleccion == nombreColeccion)
            v->addWidget(crearItemListaCancion(c, 56));
    v->addStretch();
    scroll->setWidget(contenedor);
    root->addWidget(scroll, 1);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Close);
    QObject::connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    QObject::connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    root->addWidget(bb);
    dlg.exec();
}

// =================== MIS CANCIONES ===================

void InterfazAdministrador::construirMisCancionesLista() {
    limpiarResiduosEdicionGlobal();
    limpiarZonaCentral();

    QLabel *titulo = new QLabel("🎵 Mis Canciones");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(titulo);

    SmartScrollArea *scroll = new SmartScrollArea;
    scroll->setAutoHide(false, true);
    QWidget *contenedor = new QWidget;
    QVBoxLayout *v = new QVBoxLayout(contenedor);
    v->setSpacing(8);

    ManejadorCanciones mc;
    for (const Cancion &c : mc.obtenerPorArtista(nombreArtistaLogueado))
        if (c.activo) v->addWidget(crearItemListaCancion(c, 64));
    v->addStretch();
    scroll->setWidget(contenedor);
    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(scroll);
}

// =================== PERFIL ===================

void InterfazAdministrador::mostrarPerfilArtista() {
    limpiarResiduosEdicionGlobal();
    limpiarZonaCentral();

    ManejadorUsuarios mu;
    QList<Usuario> usuarios = mu.obtenerUsuarios();
    Usuario artista; bool ok = false;
    for (const Usuario &u : usuarios)
        if (u.tipo==Administrador && u.nombreArtistico==nombreArtistaLogueado) { artista=u; ok=true; break; }

    auto *lay = qobject_cast<QVBoxLayout*>(zonaCentral->layout());
    QLabel *titulo = new QLabel("👤 Perfil del Artista");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    lay->addWidget(titulo);

    if (!ok) { lay->addWidget(new QLabel("No se encontró el perfil del administrador actual.")); lay->addStretch(); return; }

    QWidget *cab = new QWidget; QHBoxLayout *hc = new QHBoxLayout(cab);
    lblImagenPerfil = new QLabel; lblImagenPerfil->setFixedSize(160,160);
    lblImagenPerfil->setAlignment(Qt::AlignCenter);
    lblImagenPerfil->setStyleSheet("border:1px solid #333;background:white;border-radius:10px;");
    if (esImagenValida(artista.rutaImagen)) {
        QPixmap img(artista.rutaImagen);
        lblImagenPerfil->setPixmap(img.scaled(lblImagenPerfil->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else lblImagenPerfil->setText("Sin imagen");

    QVBoxLayout *info = new QVBoxLayout;
    auto add = [&](const QString &label, const QString &val){
        QLabel *l = new QLabel(QString("<b>%1:</b> %2").arg(label, val.isEmpty()? "-" : val));
        info->addWidget(l);
    };
    QLabel *nombreGrand = new QLabel(artista.nombreArtistico);
    nombreGrand->setStyleSheet("font-size:22px;font-weight:700;");
    info->addWidget(nombreGrand);
    add("Nombre real", artista.nombreReal);
    add("País", artista.paisOrigen);
    add("Género musical", artista.generoMusical);
    add("Correo", artista.correoElectronico);
    add("Bio", artista.biografia);

    QPushButton *btnEditar = new QPushButton("Editar");
    btnEditar->setStyleSheet("background:#2a2a2a;padding:6px 10px;border-radius:8px;");
    connect(btnEditar, &QPushButton::clicked, this, &InterfazAdministrador::mostrarFormularioEditarPerfil);

    info->addSpacing(8); info->addWidget(btnEditar, 0, Qt::AlignLeft);
    hc->addWidget(lblImagenPerfil); hc->addLayout(info,1);
    lay->addWidget(cab);
    lay->addStretch();
}

void InterfazAdministrador::mostrarFormularioAgregar(bool limpiar) {
    limpiarResiduosEdicionGlobal();
    limpiarZonaCentral();

    SmartScrollArea *scroll = new SmartScrollArea;
    scroll->setAutoHide(false, true);
    QWidget *contenedor = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(contenedor);

    QLabel *titulo = new QLabel("➕ Agregar Canción");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titulo);

    leTitulo   = new QLineEdit;
    cbGenero   = new QComboBox; cbGenero->addItems({"Pop","Corridos","Cristianos","Electrónica","Reguetón","Rock","Clásicas"});
    cbCategoria= new QComboBox; cbCategoria->addItems({"Playlist","Recomendado","Favorito","Infantil","Instrumental"});
    leDuracion = new QLineEdit; leDuracion->setPlaceholderText("mm:ss (se autocompleta al elegir audio)");

    cbTipo = new QComboBox; cbTipo->addItems({"Single","EP","Álbum"});
    connect(cbTipo, &QComboBox::currentTextChanged, this, &InterfazAdministrador::slotTipoCancionCambio);

    teDescripcion = new QTextEdit; teDescripcion->setFixedHeight(60);

    lblPortada = new QLabel("Sin portada");
    lblPortada->setFixedSize(120, 120);
    lblPortada->setAlignment(Qt::AlignCenter);
    lblPortada->setStyleSheet("border:1px solid gray;background:white;");

    lblAudio = new QLabel("Sin archivo de audio"); lblAudio->setAlignment(Qt::AlignCenter);

    btnCargarPortada = new QPushButton("Cargar Portada");
    btnCargarAudio   = new QPushButton("Cargar Audio");
    btnGuardar       = new QPushButton("Guardar Canción");

    seccionColeccion = new QWidget;
    QHBoxLayout *colecLay = new QHBoxLayout(seccionColeccion);
    QLabel *lblColec = new QLabel("Selecciona Álbum/EP:");
    cbSeleccionColeccion = new QComboBox;
    colecLay->addWidget(lblColec); colecLay->addWidget(cbSeleccionColeccion);
    seccionColeccion->setVisible(false);

    if (limpiar) {
        rutaPortadaSeleccionada.clear(); rutaAudioSeleccionada.clear();
        cbTipo->setCurrentText("Single"); seccionColeccion->setVisible(false);
    }

    connect(btnCargarPortada, &QPushButton::clicked, this, [=]() {
        QString ruta = QFileDialog::getOpenFileName(this, "Seleccionar Portada", "", "Imágenes (*.png *.jpg *.jpeg *.bmp)");
        if (esImagenValida(ruta)) {
            rutaPortadaSeleccionada = ruta;
            QPixmap img(ruta);
            lblPortada->setPixmap(img.scaled(lblPortada->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else if (!ruta.isEmpty()) QMessageBox::warning(this, "Imagen inválida", "No se pudo abrir la imagen seleccionada.");
    });

    connect(btnCargarAudio, &QPushButton::clicked, this, [=]() {
        QString ruta = QFileDialog::getOpenFileName(this, "Seleccionar Audio", "", "Audio (*.mp3 *.wav *.m4a *.flac)");
        if (!ruta.isEmpty()) {
            rutaAudioSeleccionada = ruta;
            lblAudio->setText(QFileInfo(ruta).fileName());
            auto *tmpPlayer = new QMediaPlayer(this);
            auto *tmpOut    = new QAudioOutput(this);
            tmpPlayer->setAudioOutput(tmpOut);
            connect(tmpPlayer, &QMediaPlayer::durationChanged, this, [=](qint64 ms){
                const qint64 total = ms/1000; const int mm=int(total/60), ss=int(total%60);
                leDuracion->setText(QString("%1:%2").arg(mm,2,10,QChar('0')).arg(ss,2,10,QChar('0')));
                tmpPlayer->stop(); tmpPlayer->deleteLater(); tmpOut->deleteLater();
            });
            tmpPlayer->setSource(QUrl::fromLocalFile(ruta));
        }
    });

    disconnect(btnGuardar, nullptr, nullptr, nullptr);
    connect(btnGuardar, &QPushButton::clicked, this, &InterfazAdministrador::slotGuardarCancionNueva);

    layout->addWidget(new QLabel("Título:"));     layout->addWidget(leTitulo);
    layout->addWidget(new QLabel("Género:"));     layout->addWidget(cbGenero);
    layout->addWidget(new QLabel("Categoría:"));  layout->addWidget(cbCategoria);
    layout->addWidget(new QLabel("Duración:"));   layout->addWidget(leDuracion);
    layout->addWidget(new QLabel("Tipo:"));       layout->addWidget(cbTipo);
    layout->addWidget(seccionColeccion);
    layout->addWidget(new QLabel("Descripción:")); layout->addWidget(teDescripcion);

    QHBoxLayout *archivosLayout = new QHBoxLayout;
    QVBoxLayout *portadaLayout = new QVBoxLayout; portadaLayout->addWidget(lblPortada); portadaLayout->addWidget(btnCargarPortada);
    QVBoxLayout *audioLayout   = new QVBoxLayout; audioLayout->addWidget(lblAudio);     audioLayout->addWidget(btnCargarAudio);
    archivosLayout->addLayout(portadaLayout); archivosLayout->addSpacing(20); archivosLayout->addLayout(audioLayout);
    layout->addLayout(archivosLayout);

    layout->addWidget(btnGuardar);
    layout->addStretch();

    scroll->setWidget(contenedor);
    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(scroll);
}

void InterfazAdministrador::mostrarFormularioCrearAlbumEP(const QString &tipo) {
    limpiarResiduosEdicionGlobal();
    limpiarZonaCentral();
    rutaImagenColeccionTmp.clear();

    QLabel *titulo = new QLabel(QString("🎶 Crear %1").arg(tipo));
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(titulo);

    QWidget *w = new QWidget; QVBoxLayout *layout = new QVBoxLayout(w);

    QLineEdit *leNombre = new QLineEdit;
    QTextEdit *teDescripcion = new QTextEdit; teDescripcion->setFixedHeight(60);
    QLineEdit *leGeneroCol = new QLineEdit; leGeneroCol->setPlaceholderText("Género (opcional)");

    QLabel *lblImagen = new QLabel("Sin imagen");
    lblImagen->setFixedSize(160, 160);
    lblImagen->setAlignment(Qt::AlignCenter);
    lblImagen->setStyleSheet("border: 1px solid gray; background-color: white;");

    QPushButton *btnCargar = new QPushButton("Cargar Imagen");
    QPushButton *btnGuardar = new QPushButton("Guardar");

    connect(btnCargar, &QPushButton::clicked, this, [=]() {
        QString file = QFileDialog::getOpenFileName(this, "Seleccionar Imagen", "", "Imágenes (*.jpg *.png *.jpeg *.bmp)");
        if (esImagenValida(file)) {
            rutaImagenColeccionTmp = file;
            QPixmap img(file);
            lblImagen->setPixmap(img.scaled(lblImagen->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else if (!file.isEmpty()) QMessageBox::warning(this, "Imagen inválida", "No se pudo abrir la imagen seleccionada.");
    });

    connect(btnGuardar, &QPushButton::clicked, this, [=]() {
        if (leNombre->text().isEmpty() || rutaImagenColeccionTmp.isEmpty()) {
            QMessageBox::warning(this, "Error", "Faltan campos obligatorios (Nombre e Imagen)."); return;
        }
        guardarColeccionEnArchivo(tipo, leNombre->text(), teDescripcion->toPlainText(), rutaImagenColeccionTmp);
        QFile f(tipo == "Álbum" ? "albumes.dat" : "eps.dat");
        if (f.open(QIODevice::Append)) { QDataStream out(&f); out << leGeneroCol->text() << QDate::currentDate(); }
        QMessageBox::information(this, tipo, tipo + " creado correctamente.");
        construirHome();
    });

    layout->addWidget(new QLabel("Nombre:"));      layout->addWidget(leNombre);
    layout->addWidget(new QLabel("Descripción:")); layout->addWidget(teDescripcion);
    layout->addWidget(new QLabel("Género (opcional):")); layout->addWidget(leGeneroCol);
    layout->addWidget(lblImagen); layout->addWidget(btnCargar); layout->addWidget(btnGuardar);
    layout->addStretch();

    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(w);
}

void InterfazAdministrador::mostrarListaCancionesEditable() {
    limpiarResiduosEdicionGlobal();
    limpiarZonaCentral();

    QLabel *titulo = new QLabel("✏️ Editar / Eliminar canciones");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(titulo);

    listaCanciones = new QListWidget;
    ManejadorCanciones mc;
    for (const Cancion &c : mc.obtenerPorArtista(nombreArtistaLogueado))
        if (c.activo) {
            auto *it = new QListWidgetItem(QString("%1  —  %2  (%3)").arg(c.titulo, c.genero, c.tipo));
            it->setData(Qt::UserRole, c.id); listaCanciones->addItem(it);
        }
    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(listaCanciones);

    QWidget *panelEdicion = new QWidget; panelEdicion->setObjectName("panelEdicionCanciones");
    QHBoxLayout *fila = new QHBoxLayout(panelEdicion);
    QPushButton *btnEditar   = new QPushButton("✏️ Editar");
    QPushButton *btnEliminar = new QPushButton("🗑️ Eliminar");
    fila->addWidget(btnEditar); fila->addWidget(btnEliminar);

    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(panelEdicion);
    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addStretch();

    connect(btnEditar,  &QPushButton::clicked, this, &InterfazAdministrador::slotAccionEditarCancion);
    connect(btnEliminar,&QPushButton::clicked, this, &InterfazAdministrador::slotAccionEliminarCancion);
}

void InterfazAdministrador::mostrarFormularioEditarPerfil() {
    limpiarResiduosEdicionGlobal();
    limpiarZonaCentral();

    ManejadorUsuarios mu;
    QList<Usuario> usuarios = mu.obtenerUsuarios();
    Usuario artista; bool ok = false;
    for (const Usuario &u : usuarios)
        if (u.tipo==Administrador && u.nombreArtistico==nombreArtistaLogueado) { artista=u; ok=true; break; }
    if (!ok) {
        qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(new QLabel("No se encontró el perfil del administrador actual."));
        qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addStretch(); return;
    }

    QLabel *titulo = new QLabel("📝 Editar Perfil (Artista)");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(titulo);

    lblImagenPerfil = new QLabel;
    lblImagenPerfil->setFixedSize(120,120);
    lblImagenPerfil->setAlignment(Qt::AlignCenter);
    lblImagenPerfil->setStyleSheet("border:1px solid gray;background:white;border-radius:8px;");
    if (esImagenValida(artista.rutaImagen)) {
        QPixmap img(artista.rutaImagen);
        lblImagenPerfil->setPixmap(img.scaled(lblImagenPerfil->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else lblImagenPerfil->setText("Sin imagen");
    QPushButton *btnCambiarFoto = new QPushButton("Cambiar imagen…");

    QLineEdit *leCorreo          = new QLineEdit(artista.correoElectronico);
    QLineEdit *leNombreReal      = new QLineEdit(artista.nombreReal);
    QLineEdit *leNombreArtistico = new QLineEdit(artista.nombreArtistico);
    QLineEdit *leGeneroMusical   = new QLineEdit(artista.generoMusical);
    QLineEdit *lePais            = new QLineEdit(artista.paisOrigen);
    QTextEdit *teBiografia       = new QTextEdit(artista.biografia); teBiografia->setFixedHeight(80);

    QPushButton *btnGuardarPerfil = new QPushButton("Guardar Cambios");

    auto *lay = qobject_cast<QVBoxLayout*>(zonaCentral->layout());
    lay->addWidget(lblImagenPerfil);
    lay->addWidget(btnCambiarFoto);
    lay->addWidget(new QLabel("Correo electrónico:"));   lay->addWidget(leCorreo);
    lay->addWidget(new QLabel("Nombre Real:"));          lay->addWidget(leNombreReal);
    lay->addWidget(new QLabel("Nombre Artístico:"));     lay->addWidget(leNombreArtistico);
    lay->addWidget(new QLabel("Género Musical:"));       lay->addWidget(leGeneroMusical);
    lay->addWidget(new QLabel("País de Origen:"));       lay->addWidget(lePais);
    lay->addWidget(new QLabel("Biografía:"));            lay->addWidget(teBiografia);
    lay->addWidget(btnGuardarPerfil);
    lay->addStretch();

    // Preview inmediato: avatar del header y nombre
    connect(btnCambiarFoto, &QPushButton::clicked, this, [=]() {
        QString ruta = QFileDialog::getOpenFileName(this, "Seleccionar imagen", "", "Imágenes (*.png *.jpg *.jpeg *.bmp)");
        if (esImagenValida(ruta)) {
            rutaImagenPerfilTmp = ruta;
            QPixmap img(ruta);
            lblImagenPerfil->setPixmap(img.scaled(lblImagenPerfil->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            setHeaderAvatarPreview(ruta); // ← se ve de inmediato en el menú izquierdo
        } else if (!ruta.isEmpty()) QMessageBox::warning(this, "Imagen inválida", "No se pudo abrir la imagen seleccionada.");
    });
    connect(leNombreArtistico, &QLineEdit::textChanged, this, [=](const QString &t){
        if (hdrNombre) hdrNombre->setText(t);  // preview del nombre en header
    });

    connect(btnGuardarPerfil, &QPushButton::clicked, this, [=]() {
        QList<Usuario> lista = mu.obtenerUsuarios();
        for (Usuario &u : lista) {
            if (u.tipo == Administrador && u.nombreArtistico == nombreArtistaLogueado) {
                u.correoElectronico = leCorreo->text();
                u.nombreReal        = leNombreReal->text();
                u.nombreArtistico   = leNombreArtistico->text();
                u.generoMusical     = leGeneroMusical->text();
                u.paisOrigen        = lePais->text();
                u.biografia         = teBiografia->toPlainText();
                if (!rutaImagenPerfilTmp.isEmpty()) u.rutaImagen = rutaImagenPerfilTmp;
                nombreArtistaLogueado = u.nombreArtistico; // por si cambia
                break;
            }
        }
        QFile archivo("usuarios.dat");
        if (archivo.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QDataStream out(&archivo);
            for (const Usuario &uu : lista) uu.guardar(out);
            archivo.close();
            refreshHeaderPerfil();              // ← refresca imagen y nombre desde archivo
            QMessageBox::information(this, "Éxito", "Perfil actualizado correctamente.");
            mostrarPerfilArtista();
        } else QMessageBox::warning(this, "Error", "No se pudo abrir usuarios.dat para escribir.");
    });
}

// ========== Definición: Editar Canción ==========

void InterfazAdministrador::mostrarFormularioEditarCancion(const Cancion &c) {
    limpiarResiduosEdicionGlobal();
    mostrarFormularioAgregar(false);

    idCancionEnEdicion = c.id;

    if (leTitulo)   leTitulo->setText(c.titulo);
    if (cbGenero)   cbGenero->setCurrentText(c.genero);
    if (cbCategoria)cbCategoria->setCurrentText(c.categoria);
    if (leDuracion) leDuracion->setText(c.duracion);
    if (cbTipo)     cbTipo->setCurrentText(c.tipo);
    if (teDescripcion) teDescripcion->setText(c.descripcion);

    if (lblPortada) {
        if (esImagenValida(c.portada)) {
            rutaPortadaSeleccionada = c.portada;
            QPixmap img(c.portada);
            lblPortada->setPixmap(img.scaled(lblPortada->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else { lblPortada->setText("Sin portada"); rutaPortadaSeleccionada.clear(); }
    }

    if (lblAudio) {
        if (!c.rutaArchivo.isEmpty() && QFile::exists(c.rutaArchivo)) {
            rutaAudioSeleccionada = c.rutaArchivo;
            lblAudio->setText(QFileInfo(c.rutaArchivo).fileName());
        } else { lblAudio->setText("Sin archivo de audio"); rutaAudioSeleccionada.clear(); }
    }

    if (cbTipo) slotTipoCancionCambio(cbTipo->currentText());
    if (seccionColeccion && seccionColeccion->isVisible() && cbSeleccionColeccion && !c.coleccion.isEmpty()) {
        int idx = cbSeleccionColeccion->findText(c.coleccion);
        if (idx >= 0) cbSeleccionColeccion->setCurrentIndex(idx);
    }

    if (btnGuardar) {
        disconnect(btnGuardar, nullptr, nullptr, nullptr);
        connect(btnGuardar, &QPushButton::clicked, this, &InterfazAdministrador::slotGuardarCancionEditada);
    }
}

// =================== Guardados / Stats / Slots ===================

QStringList InterfazAdministrador::cargarNombresAlbumes() const {
    QStringList nombres; for (const auto &c : leerColecciones("albumes.dat")) nombres << c.nombre; return nombres;
}
QStringList InterfazAdministrador::cargarNombresEPs() const {
    QStringList nombres; for (const auto &c : leerColecciones("eps.dat")) nombres << c.nombre; return nombres;
}
void InterfazAdministrador::guardarColeccionEnArchivo(const QString &tipo, const QString &nombre, const QString &descripcion, const QString &rutaImagen) {
    QString archivo = (tipo == "Álbum") ? "albumes.dat" : "eps.dat";
    QFile f(archivo); const bool existe = f.exists();
    if (!f.open(existe ? QIODevice::Append : (QIODevice::WriteOnly | QIODevice::Truncate))) {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo de colecciones."); return;
    }
    QDataStream out(&f); bool activo = true; out << nombre << descripcion << rutaImagen << activo; f.close();
}

QList<InterfazAdministrador::Reproduccion> InterfazAdministrador::leerReproducciones() const {
    QList<Reproduccion> list; QFile f("reproducciones.dat");
    if (!f.open(QIODevice::ReadOnly)) return list; QDataStream in(&f);
    while (!in.atEnd()) { Reproduccion r; in >> r.cancionId >> r.userId >> r.ms >> r.fecha; list.append(r); }
    return list;
}
QList<InterfazAdministrador::Calificacion> InterfazAdministrador::leerCalificaciones() const {
    QList<Calificacion> list; QFile f("calificaciones.dat");
    if (!f.open(QIODevice::ReadOnly)) return list; QDataStream in(&f);
    while (!in.atEnd()) { Calificacion c; in >> c.cancionId >> c.userId >> c.estrellas >> c.fecha; list.append(c); }
    return list;
}

void InterfazAdministrador::refrescarEstadisticasDerecha() {
    if (!zonaDerecha) return;
    auto *lay = qobject_cast<QVBoxLayout*>(zonaDerecha->layout()); if (!lay) return;
    while (lay->count() > 1) { QLayoutItem *it = lay->takeAt(1); if (it->widget()) it->widget()->deleteLater(); delete it; }

    ManejadorCanciones mc; QList<Cancion> mias = mc.obtenerPorArtista(nombreArtistaLogueado);
    int total = 0; for (const auto &c : mias) if (c.activo) ++total;
    lay->addWidget(new QLabel(QString("Total Canciones: %1").arg(total)));

    auto reps = leerReproducciones(); QMap<qint64,int> conteo; for (const auto &r : reps) conteo[r.cancionId]++;
    qint64 topId=0; int topCnt=0; for (auto it=conteo.begin(); it!=conteo.end(); ++it) if (it.value()>topCnt){ topCnt=it.value(); topId=it.key(); }
    QString masEscuchada = "-"; if (topId!=0) for (const auto &c : mias) if (c.id==topId){ masEscuchada=c.titulo; break; }
    lay->addWidget(new QLabel(QString("Más Escuchada: %1").arg(masEscuchada)));

    auto cals = leerCalificaciones();
    if (cals.isEmpty()) lay->addWidget(new QLabel("Promedio Calificación (global): -"));
    else { double suma=0; for (const auto &ca : cals) suma+=ca.estrellas;
        lay->addWidget(new QLabel(QString("Promedio Calificación (global): %1").arg(QString::number(suma/cals.size(),'f',2)))); }
    lay->addStretch();
}

void InterfazAdministrador::construirVistaEstadisticasCentro() {
    limpiarResiduosEdicionGlobal(); limpiarZonaCentral();

    // ============ contenedor con SCROLL vertical ============
    SmartScrollArea *scroll = new SmartScrollArea;
    scroll->setAutoHide(false, true);
    QWidget *contenedor = new QWidget;
    QVBoxLayout *root = new QVBoxLayout(contenedor);
    root->setSpacing(16);

    QLabel *titulo = new QLabel("📊 Estadísticas del catálogo");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    root->addWidget(titulo);

    // Datos base
    ManejadorCanciones mc; QList<Cancion> mias = mc.obtenerPorArtista(nombreArtistaLogueado);
    QSet<qint64> idsPropios; for (const auto &c : mias) if (c.activo) idsPropios.insert(c.id);

    auto reps = leerReproducciones();
    auto cals = leerCalificaciones();

    // Agregados
    qint64 totalPlays = 0, totalMs = 0;
    QMap<qint64,int> playsPorCancion;
    QMap<QString,int> playsPorGenero;
    QMap<QDate,int>    playsPorDia;

    for (const auto &r : reps) {
        if (!idsPropios.contains(r.cancionId)) continue;
        totalPlays++; totalMs += r.ms;
        playsPorCancion[r.cancionId]++;
        playsPorDia[r.fecha]++;

        // género
        for (const auto &c : mias) if (c.id == r.cancionId) { playsPorGenero[c.genero]++; break; }
    }

    double promGlobal = 0.0; int cntCal=0;
    for (const auto &ca : cals) { if (!idsPropios.contains(ca.cancionId)) continue; promGlobal += ca.estrellas; cntCal++; }
    if (cntCal) promGlobal /= cntCal;

    // ===== KPI CARDS =====
    auto cardKpi = [](const QString &title, const QString &big, const QString &small)->QWidget*{
        QWidget *w = new QWidget; w->setStyleSheet("background:#1e1e1e;border-radius:10px;");
        QVBoxLayout *v = new QVBoxLayout(w);
        QLabel *t = new QLabel(title); t->setStyleSheet("color:#bbb;font-size:12px;");
        QLabel *b = new QLabel(big);   b->setStyleSheet("font-size:28px;font-weight:800;");
        QLabel *s = new QLabel(small); s->setStyleSheet("color:#aaa;font-size:12px;");
        v->addWidget(t); v->addWidget(b); v->addWidget(s); return w;
    };

    // Top 3 canciones
    QList<QPair<qint64,int>> top;
    for (auto it=playsPorCancion.begin(); it!=playsPorCancion.end(); ++it) top.append({it.key(),it.value()});
    std::sort(top.begin(), top.end(), [](auto a, auto b){ return a.second>b.second; });
    QString top3txt;
    for (int i=0;i<qMin(3, top.size());++i){
        QString nom="(?)"; for (const auto &c : mias) if (c.id==top[i].first){ nom=c.titulo; break; }
        top3txt += QString("%1. %2\n").arg(i+1).arg(nom);
    }
    if (top3txt.isEmpty()) top3txt = "(sin datos)";

    QWidget *kpis = new QWidget;
    QGridLayout *grid = new QGridLayout(kpis);
    grid->setHorizontalSpacing(12); grid->setVerticalSpacing(12);
    grid->addWidget(cardKpi("Reproducciones", QString::number(totalPlays), "Total del catálogo"), 0,0);
    grid->addWidget(cardKpi("Tiempo total", formatoTiempoLargo(totalMs), "Sumatoria escuchas"),     0,1);
    grid->addWidget(cardKpi("Top 3", top3txt.trimmed(), "Más escuchadas"),                          1,0);
    grid->addWidget(cardKpi("Promedio calificación", (cntCal? QString::number(promGlobal,'f',1)+" / 5" : "-"), "Global"), 1,1);
    root->addWidget(kpis);

    // ===== Gráfica: TOP canciones (barras) =====
    if (!top.isEmpty()) {
        QStringList categorias;
        QBarSet *set = new QBarSet("Reproducciones");
        int max = qMin(10, top.size());
        for (int i=0;i<max;++i){
            QString nom="(?)"; for (const auto &c : mias) if (c.id==top[i].first){ nom=c.titulo; break; }
            categorias << nom;
            *set << top[i].second;
        }
        QBarSeries *series = new QBarSeries; series->append(set);

        QChart *chart = new QChart; chart->addSeries(series);
        chart->setTitle("Top canciones");
        chart->setTheme(QChart::ChartThemeDark);

        QBarCategoryAxis *axisX = new QBarCategoryAxis; axisX->append(categorias);
        chart->addAxis(axisX, Qt::AlignBottom); series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis; axisY->setTitleText("Reproducciones");
        chart->addAxis(axisY, Qt::AlignLeft); series->attachAxis(axisY);

        QChartView *view = new QChartView(chart); view->setMinimumHeight(360); view->setRenderHint(QPainter::Antialiasing);
        root->addWidget(view);
    } else {
        root->addWidget(new QLabel("Top canciones: (sin datos)"));
    }

    // ===== Gráfica: Reproducciones por día (línea) =====
    if (!playsPorDia.isEmpty()) {
        QLineSeries *line = new QLineSeries; line->setName("Reproducciones por día");
        QList<QDate> fechas = playsPorDia.keys(); std::sort(fechas.begin(), fechas.end());
        for (const QDate &d : fechas) {
            line->append(QDateTime(d, QTime(0,0)).toMSecsSinceEpoch(), playsPorDia[d]);
        }
        QChart *chart = new QChart; chart->addSeries(line); chart->setTheme(QChart::ChartThemeDark);
        chart->setTitle("Reproducciones por día");

        QDateTimeAxis *axisX = new QDateTimeAxis; axisX->setFormat("dd MMM");
        axisX->setTitleText("Fecha");
        chart->addAxis(axisX, Qt::AlignBottom); line->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis; axisY->setTitleText("Reproducciones");
        chart->addAxis(axisY, Qt::AlignLeft); line->attachAxis(axisY);

        QChartView *view = new QChartView(chart); view->setMinimumHeight(360); view->setRenderHint(QPainter::Antialiasing);
        root->addWidget(view);
    } else {
        root->addWidget(new QLabel("Reproducciones por día: (sin datos)"));
    }

    // ===== Gráfica: Distribución por género (pastel) =====
    if (!playsPorGenero.isEmpty()) {
        QPieSeries *serie = new QPieSeries;
        for (auto it=playsPorGenero.begin(); it!=playsPorGenero.end(); ++it) {
            serie->append(it.key().isEmpty()? "(sin género)": it.key(), it.value());
        }
        QChart *chart = new QChart; chart->addSeries(serie); chart->setTheme(QChart::ChartThemeDark);
        chart->setTitle("Distribución de reproducciones por género");
        QChartView *view = new QChartView(chart); view->setMinimumHeight(360); view->setRenderHint(QPainter::Antialiasing);
        root->addWidget(view);
    } else {
        root->addWidget(new QLabel("Distribución por género: (sin datos)"));
    }

    root->addStretch();
    scroll->setWidget(contenedor);
    qobject_cast<QVBoxLayout*>(zonaCentral->layout())->addWidget(scroll);
}

// Slots menú
void InterfazAdministrador::slotHome()                   { construirHome(); }
void InterfazAdministrador::slotMisCanciones()           { construirMisCancionesLista(); }
void InterfazAdministrador::slotMostrarPerfil()          { mostrarPerfilArtista(); }
void InterfazAdministrador::slotAgregarCancion()         { mostrarFormularioAgregar(); }
void InterfazAdministrador::slotEditarEliminarCancion()  { mostrarListaCancionesEditable(); }
void InterfazAdministrador::slotCrearAlbum()             { mostrarFormularioCrearAlbumEP("Álbum"); }
void InterfazAdministrador::slotCrearEP()                { mostrarFormularioCrearAlbumEP("EP"); }
void InterfazAdministrador::slotVerEstadisticas()        { construirVistaEstadisticasCentro(); }

// 🔴 Cerrar sesión (usa cframe que recibimos por setPaginaPrincipal(...))
void InterfazAdministrador::slotCerrarSesion() {
    if (paginaPrincipal) {
        paginaPrincipal->regresarAlInicio();   // vuelve al login
    } else {
        if (auto *stack = this->parentWidget() ? this->parentWidget()->findChild<QStackedWidget *>() : nullptr)
            stack->setCurrentIndex(0);
    }
}

// Guardados canciones
void InterfazAdministrador::slotTipoCancionCambio(const QString &texto) {
    if (!seccionColeccion || !cbSeleccionColeccion) return;
    if (texto == "Álbum") { cbSeleccionColeccion->clear(); cbSeleccionColeccion->addItems(cargarNombresAlbumes()); seccionColeccion->setVisible(true); }
    else if (texto == "EP") { cbSeleccionColeccion->clear(); cbSeleccionColeccion->addItems(cargarNombresEPs()); seccionColeccion->setVisible(true); }
    else { cbSeleccionColeccion->clear(); seccionColeccion->setVisible(false); }
}

void InterfazAdministrador::slotGuardarCancionNueva() {
    if (!leTitulo||!cbGenero||!cbCategoria||!leDuracion||!cbTipo||!teDescripcion) return;
    if (leTitulo->text().isEmpty() || rutaAudioSeleccionada.isEmpty() || rutaPortadaSeleccionada.isEmpty()) {
        QMessageBox::warning(this, "Error", "Faltan campos obligatorios (Título, Portada, Audio)."); return;
    }
    Cancion c; c.id=QDateTime::currentMSecsSinceEpoch(); c.titulo=leTitulo->text(); c.artista=nombreArtistaLogueado;
    c.genero=cbGenero->currentText(); c.categoria=cbCategoria->currentText(); c.duracion=leDuracion->text();
    c.descripcion=teDescripcion->toPlainText(); c.portada=rutaPortadaSeleccionada; c.rutaArchivo=rutaAudioSeleccionada;
    c.tipo=cbTipo->currentText(); c.coleccion = seccionColeccion->isVisible()? cbSeleccionColeccion->currentText() : "";
    c.fechaCarga=QDate::currentDate(); c.activo=true;
    ManejadorCanciones mc; mc.agregarCancion(c);
    QMessageBox::information(this, "Éxito", "Canción guardada correctamente.");
    construirHome(); refrescarEstadisticasDerecha();
}

void InterfazAdministrador::slotGuardarCancionEditada() {
    if (!leTitulo||!cbGenero||!cbCategoria||!leDuracion||!cbTipo||!teDescripcion) return;
    if (idCancionEnEdicion==0) return;
    ManejadorCanciones mc; auto todas = mc.obtenerTodas(); bool hallada=false;
    for (Cancion &c : todas) if (c.id==idCancionEnEdicion) {
            c.titulo=leTitulo->text(); c.genero=cbGenero->currentText(); c.categoria=cbCategoria->currentText();
            c.duracion=leDuracion->text(); c.descripcion=teDescripcion->toPlainText(); c.tipo=cbTipo->currentText();
            c.coleccion=(seccionColeccion->isVisible()? cbSeleccionColeccion->currentText() : "");
            if (!rutaPortadaSeleccionada.isEmpty()) c.portada=rutaPortadaSeleccionada;
            if (!rutaAudioSeleccionada.isEmpty())   c.rutaArchivo=rutaAudioSeleccionada;
            hallada=true; break;
        }
    if (!hallada) { QMessageBox::warning(this, "Error", "No se encontró la canción a editar."); return; }

    QFile archivo("canciones.dat");
    if (archivo.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QDataStream out(&archivo); for (const Cancion &c : todas) c.guardar(out); archivo.close();
        QMessageBox::information(this, "Éxito", "Canción actualizada.");
        idCancionEnEdicion = 0; construirHome(); refrescarEstadisticasDerecha();
    } else QMessageBox::warning(this, "Error", "No se pudo escribir canciones.dat");
}

void InterfazAdministrador::slotAccionEditarCancion() {
    if (!listaCanciones || !listaCanciones->currentItem()) { QMessageBox::warning(this, "Selecciona", "Selecciona una canción."); return; }
    const qint64 id = listaCanciones->currentItem()->data(Qt::UserRole).toLongLong();
    ManejadorCanciones mc;
    for (const auto &c : mc.obtenerPorArtista(nombreArtistaLogueado)) if (c.id==id) { mostrarFormularioEditarCancion(c); return; }
    QMessageBox::warning(this, "Error", "No se encontró la canción seleccionada.");
}

void InterfazAdministrador::slotAccionEliminarCancion() {
    if (!listaCanciones || !listaCanciones->currentItem()) { QMessageBox::warning(this, "Selecciona", "Selecciona una canción."); return; }
    if (QMessageBox::question(this, "Confirmar", "¿Seguro que deseas eliminar esta canción?") != QMessageBox::Yes) return;
    const qint64 idEliminar = listaCanciones->currentItem()->data(Qt::UserRole).toLongLong();
    ManejadorCanciones mc; auto todas = mc.obtenerTodas();
    for (Cancion &c : todas) if (c.id==idEliminar) { c.activo=false; break; }
    QFile archivo("canciones.dat");
    if (archivo.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QDataStream out(&archivo); for (const Cancion &c : todas) c.guardar(out); archivo.close();
        QMessageBox::information(this, "Eliminado", "Canción eliminada.");
        construirHome(); refrescarEstadisticasDerecha();
    } else QMessageBox::warning(this, "Error", "No se pudo escribir canciones.dat");
}
