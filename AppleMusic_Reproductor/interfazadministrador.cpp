/*#include "interfazadministrador.h"
#include <QLineEdit>
#include <QMessageBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QDebug>
#include <QVBoxLayout>
#include <QDateTime>
#include <QComboBox>
#include <QTextEdit>
#include "cframe.h"
#include "ui_cframe.h"

InterfazAdministrador::InterfazAdministrador(QWidget *parent)
    : QWidget(parent),
    zonaCentral(nullptr),
    zonaDerecha(nullptr),
    paginaPrincipal(nullptr)
{
}

void InterfazAdministrador::setNombreArtista(const QString &nombre) {
    nombreArtistaLogueado = nombre;
}

void InterfazAdministrador::setPaginaPrincipal(cframe *pagina) {
    paginaPrincipal = pagina;
}

void InterfazAdministrador::cargarEnPagina(QWidget *pagina) {
    pagina->setStyleSheet("background-color: #121212; color: white;");
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(pagina);
    layoutPrincipal->setContentsMargins(12, 22, 12, 12);

    QHBoxLayout *cuerpo = new QHBoxLayout;
    cuerpo->addWidget(crearMenuIzquierdo(), 1);
    zonaCentral = crearZonaCentral();
    zonaDerecha = crearZonaDerecha();
    cuerpo->addWidget(zonaCentral, 3);
    cuerpo->addWidget(zonaDerecha, 1);

    layoutPrincipal->addLayout(cuerpo);
    pagina->setLayout(layoutPrincipal);
}

QWidget* InterfazAdministrador::crearMenuIzquierdo() {
    QWidget *menu = new QWidget;
    menu->setStyleSheet("background-color: #1e1e1e; border-radius: 8px;");
    QVBoxLayout *layout = new QVBoxLayout(menu);

    QLabel *lblPerfil = new QLabel("🎤 Perfil Artista");
    lblPerfil->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(lblPerfil);

    QPushButton *btnPerfil = new QPushButton("📝 Editar Perfil");
    QPushButton *btnAgregar = new QPushButton("➕ Agregar Canción");
    QPushButton *btnEditar = new QPushButton("✏️ Editar/Eliminar Canción");
    QPushButton *btnStats = new QPushButton("📊 Ver Estadísticas");
    QPushButton *btnCerrar = new QPushButton("🚪 Cerrar Sesión");

    QList<QPushButton*> botones = {btnPerfil, btnAgregar, btnEditar, btnStats, btnCerrar};
    for (auto *btn : botones) {
        btn->setStyleSheet("background-color: #2a2a2a; color: white; font-size: 14px; padding: 8px; border-radius: 5px;");
        btn->setCursor(Qt::PointingHandCursor);
        layout->addWidget(btn);
    }

    connect(btnPerfil, &QPushButton::clicked, this, &InterfazAdministrador::slotEditarPerfil);
    connect(btnAgregar, &QPushButton::clicked, this, &InterfazAdministrador::slotAgregarCancion);
    connect(btnEditar, &QPushButton::clicked, this, &InterfazAdministrador::slotEditarEliminarCancion);
    connect(btnStats, &QPushButton::clicked, this, &InterfazAdministrador::slotVerEstadisticas);
    connect(btnCerrar, &QPushButton::clicked, this, &InterfazAdministrador::slotCerrarSesion);

    layout->addStretch();
    return menu;
}

QWidget* InterfazAdministrador::crearZonaCentral() {
    QWidget *zona = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(zona);
    QLabel *titulo = new QLabel("🎵 Mis Canciones");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titulo);
    layout->addStretch();
    return zona;
}

QWidget* InterfazAdministrador::crearZonaDerecha() {
    QWidget *zona = new QWidget;
    zona->setStyleSheet("background-color: #1e1e1e; border-radius: 8px;");
    QVBoxLayout *layout = new QVBoxLayout(zona);

    QLabel *titulo = new QLabel("📊 Estadísticas");
    titulo->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(titulo);

    layout->addWidget(new QLabel("Total Canciones: 0"));
    layout->addWidget(new QLabel("Más Escuchada: -"));
    layout->addWidget(new QLabel("Promedio Calificación: -"));

    layout->addStretch();
    return zona;
}

void InterfazAdministrador::mostrarFormularioAgregar() {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(zonaCentral->layout());
    if (!layout) return;

    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QLabel *titulo = new QLabel("➕ Agregar Canción");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titulo);

    QLineEdit *leTitulo = new QLineEdit;
    QComboBox *cbGenero = new QComboBox;
    cbGenero->addItems({"Pop", "Corridos", "Cristianos", "Electrónica", "Reguetón", "Rock", "Clásicas"});

    QComboBox *cbCategoria = new QComboBox;
    cbCategoria->addItems({"Playlist", "Recomendado", "Favorito", "Infantil", "Instrumental"});

    QLineEdit *leDuracion = new QLineEdit;
    QComboBox *cbTipo = new QComboBox;
    cbTipo->addItems({"Single", "EP", "Álbum"});

    QTextEdit *teDescripcion = new QTextEdit;

    QLabel *lblPortada = new QLabel("Sin portada");
    lblPortada->setAlignment(Qt::AlignCenter);
    lblPortada->setFixedSize(150, 150);
    lblPortada->setStyleSheet("border: 1px solid gray; background-color: white;");

    QLabel *lblAudio = new QLabel("Sin archivo de audio");
    lblAudio->setAlignment(Qt::AlignCenter);

    QPushButton *btnCargarPortada = new QPushButton("Cargar Portada");
    QPushButton *btnCargarAudio = new QPushButton("Cargar Audio");
    QPushButton *btnGuardar = new QPushButton("Guardar Canción");

    QString *rutaPortada = new QString;
    QString *rutaAudio = new QString;

    connect(btnCargarPortada, &QPushButton::clicked, [=]() {
        QString ruta = QFileDialog::getOpenFileName(nullptr, "Seleccionar Portada", "", "Imágenes (*.png *.jpg *.jpeg *.bmp)");
        if (!ruta.isEmpty()) {
            *rutaPortada = ruta;
            QPixmap img(ruta);
            lblPortada->setPixmap(img.scaled(lblPortada->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    });

    connect(btnCargarAudio, &QPushButton::clicked, [=]() {
        QString ruta = QFileDialog::getOpenFileName(nullptr, "Seleccionar Audio", "", "Audio (*.mp3 *.wav)");
        if (!ruta.isEmpty()) {
            *rutaAudio = ruta;
            lblAudio->setText(QFileInfo(ruta).fileName());
        }
    });

    connect(cbTipo, &QComboBox::currentTextChanged, this, [=](const QString &texto) {
        if (texto == "Álbum") {
            mostrarFormularioCrearAlbum();
        }
    });

    connect(btnGuardar, &QPushButton::clicked, [=]() {
        if (leTitulo->text().isEmpty() || *rutaAudio == "" || *rutaPortada == "") {
            QMessageBox::warning(nullptr, "Error", "Faltan campos obligatorios (Título, Portada, Audio).");
            return;
        }

        Cancion nueva;
        nueva.id = QDateTime::currentMSecsSinceEpoch();
        nueva.titulo = leTitulo->text();
        nueva.artista = nombreArtistaLogueado;
        nueva.genero = cbGenero->currentText();
        nueva.categoria = cbCategoria->currentText();
        nueva.duracion = leDuracion->text();
        nueva.descripcion = teDescripcion->toPlainText();
        nueva.portada = *rutaPortada;
        nueva.rutaArchivo = *rutaAudio;
        nueva.tipo = cbTipo->currentText();
        nueva.fechaCarga = QDate::currentDate();
        nueva.activo = true;

        ManejadorCanciones manejador;
        manejador.agregarCancion(nueva);

        QMessageBox::information(nullptr, "Éxito", "Canción guardada correctamente.");
        mostrarListaCanciones();
    });

    layout->addWidget(new QLabel("Título:"));
    layout->addWidget(leTitulo);

    layout->addWidget(new QLabel("Género:"));
    layout->addWidget(cbGenero);

    layout->addWidget(new QLabel("Categoría:"));
    layout->addWidget(cbCategoria);

    layout->addWidget(new QLabel("Duración:"));
    layout->addWidget(leDuracion);

    layout->addWidget(new QLabel("Tipo:"));
    layout->addWidget(cbTipo);

    layout->addWidget(new QLabel("Descripción:"));
    layout->addWidget(teDescripcion);

    layout->addWidget(lblPortada);
    layout->addWidget(btnCargarPortada);

    layout->addWidget(lblAudio);
    layout->addWidget(btnCargarAudio);

    layout->addWidget(btnGuardar);

    layout->addStretch();
}

void InterfazAdministrador::mostrarFormularioCrearAlbum() {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(zonaCentral->layout());
    if (!layout) return;

    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QLabel *titulo = new QLabel("📀 Crear Álbum");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titulo);

    QLineEdit *leNombreAlbum = new QLineEdit;
    QLabel *lblImagenAlbum = new QLabel("Sin imagen");
    lblImagenAlbum->setAlignment(Qt::AlignCenter);
    lblImagenAlbum->setFixedSize(150, 150);
    lblImagenAlbum->setStyleSheet("border: 1px solid gray; background-color: white;");
    QPushButton *btnCargarImagenAlbum = new QPushButton("Cargar Imagen");
    QPushButton *btnGuardarAlbum = new QPushButton("Guardar Álbum");

    QString *rutaImagenAlbum = new QString;

    connect(btnCargarImagenAlbum, &QPushButton::clicked, [=]() {
        QString ruta = QFileDialog::getOpenFileName(nullptr, "Seleccionar Imagen de Álbum", "", "Imágenes (*.png *.jpg *.jpeg *.bmp)");
        if (!ruta.isEmpty()) {
            *rutaImagenAlbum = ruta;
            QPixmap img(ruta);
            lblImagenAlbum->setPixmap(img.scaled(lblImagenAlbum->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    });

    connect(btnGuardarAlbum, &QPushButton::clicked, [=]() {
        if (leNombreAlbum->text().isEmpty() || *rutaImagenAlbum == "") {
            QMessageBox::warning(nullptr, "Error", "Faltan campos obligatorios (Nombre, Imagen).");
            return;
        }
        QMessageBox::information(nullptr, "Álbum", "Álbum '" + leNombreAlbum->text() + "' creado correctamente el " + QDateTime::currentDateTime().toString());
        mostrarFormularioAgregar();
    });

    layout->addWidget(new QLabel("Nombre del Álbum:"));
    layout->addWidget(leNombreAlbum);
    layout->addWidget(lblImagenAlbum);
    layout->addWidget(btnCargarImagenAlbum);
    layout->addWidget(btnGuardarAlbum);
    layout->addStretch();
}

void InterfazAdministrador::mostrarListaCanciones() {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(zonaCentral->layout());
    if (!layout) return;

    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QLabel *titulo = new QLabel("🎵 Mis Canciones");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titulo);

    ManejadorCanciones manejador;
    QList<Cancion> canciones = manejador.obtenerTodas();
    for (const Cancion &c : canciones) {
        if (!c.activo || c.artista != nombreArtistaLogueado) continue;
        QLabel *info = new QLabel(QString("🎵 %1 - %2").arg(c.titulo, c.genero));
        layout->addWidget(info);
    }

    layout->addStretch();
}

// Slots
void InterfazAdministrador::slotEditarPerfil() {
    QMessageBox::information(this, "Editar Perfil", "Aquí se editará el perfil.");
}
void InterfazAdministrador::slotAgregarCancion() {
    mostrarFormularioAgregar();
}
void InterfazAdministrador::slotEditarEliminarCancion() {
    mostrarListaCanciones();
}
void InterfazAdministrador::slotVerEstadisticas() {
    QMessageBox::information(this, "Estadísticas", "Aquí se mostrarán las estadísticas.");
}
void InterfazAdministrador::slotCerrarSesion() {
    if (paginaPrincipal) {
        paginaPrincipal->regresarAlInicio();  // ✅ ya no accede a ui directamente
    }
}*/

#include "interfazadministrador.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QFormLayout>
#include <QPixmap>
#include <QScrollArea>
#include "manejadorcanciones.h"
#include "cancion.h"
#include <QStackedWidget>

InterfazAdministrador::InterfazAdministrador(QWidget *parent)
    : QWidget(parent),
    zonaCentral(nullptr),
    zonaDerecha(nullptr),
    paginaPrincipal(nullptr)
{
}

void InterfazAdministrador::setNombreArtista(const QString &nombre) {
    nombreArtistaLogueado = nombre;
}

void InterfazAdministrador::setPaginaPrincipal(cframe *pagina) {
    paginaPrincipal = pagina;
}

void InterfazAdministrador::cargarEnPagina(QWidget *pagina) {
    pagina->setStyleSheet("background-color: #121212; color: white;");
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(pagina);
    layoutPrincipal->setContentsMargins(12, 22, 12, 12);

    if (paginaPrincipal) {
        connect(paginaPrincipal->getBtnCerrarSesion(), &QPushButton::clicked, this, &InterfazAdministrador::slotCerrarSesion);
    }

    QHBoxLayout *cuerpo = new QHBoxLayout;
    cuerpo->addWidget(crearMenuIzquierdo(), 1);
    zonaCentral = crearZonaCentral();
    zonaDerecha = crearZonaDerecha();
    cuerpo->addWidget(zonaCentral, 3);
    cuerpo->addWidget(zonaDerecha, 1);

    layoutPrincipal->addLayout(cuerpo);
    pagina->setLayout(layoutPrincipal);
}

QWidget* InterfazAdministrador::crearMenuIzquierdo() {
    QWidget *menu = new QWidget;
    menu->setStyleSheet("background-color: #1e1e1e; border-radius: 8px;");
    QVBoxLayout *layout = new QVBoxLayout(menu);

    QLabel *lblPerfil = new QLabel("🎤 Perfil Artista");
    lblPerfil->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(lblPerfil);

    QPushButton *btnPerfil = new QPushButton("📝 Editar Perfil");
    QPushButton *btnAgregar = new QPushButton("➕ Agregar Canción");
    QPushButton *btnEditar = new QPushButton("✏️ Editar/Eliminar Canción");
    QPushButton *btnCrearAlbum = new QPushButton("📀 Crear Álbum");
    QPushButton *btnCrearEP = new QPushButton("🎚️ Crear EP");
    QPushButton *btnCerrar = new QPushButton("🚪 Cerrar Sesión");

    QList<QPushButton*> botones = {
        btnPerfil, btnAgregar, btnEditar, btnCrearAlbum, btnCrearEP, btnCerrar
    };

    for (auto *btn : botones) {
        btn->setStyleSheet("background-color: #2a2a2a; color: white; font-size: 14px; padding: 8px; border-radius: 5px;");
        btn->setCursor(Qt::PointingHandCursor);
        layout->addWidget(btn);
    }

    connect(btnPerfil, &QPushButton::clicked, this, &InterfazAdministrador::slotEditarPerfil);
    connect(btnAgregar, &QPushButton::clicked, this, &InterfazAdministrador::slotAgregarCancion);
    connect(btnEditar, &QPushButton::clicked, this, &InterfazAdministrador::slotEditarEliminarCancion);
    connect(btnCrearAlbum, &QPushButton::clicked, this, &InterfazAdministrador::slotCrearAlbum);
    connect(btnCrearEP, &QPushButton::clicked, this, &InterfazAdministrador::slotCrearEP);
    connect(btnCerrar, &QPushButton::clicked, this, &InterfazAdministrador::slotCerrarSesion);

    layout->addStretch();
    return menu;
}

QWidget* InterfazAdministrador::crearZonaCentral() {
    QWidget *zona = new QWidget;
    zona->setLayout(new QVBoxLayout);
    return zona;
}

QWidget* InterfazAdministrador::crearZonaDerecha() {
    QWidget *zona = new QWidget;
    zona->setStyleSheet("background-color: #1e1e1e; border-radius: 8px;");
    QVBoxLayout *layout = new QVBoxLayout(zona);

    QLabel *titulo = new QLabel("📊 Estadísticas");
    titulo->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(titulo);

    layout->addWidget(new QLabel("Total Canciones: 0"));
    layout->addWidget(new QLabel("Más Escuchada: -"));
    layout->addWidget(new QLabel("Promedio Calificación: -"));

    layout->addStretch();
    return zona;
}

void InterfazAdministrador::mostrarFormularioAgregar() {
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(zonaCentral->layout());
    if (!mainLayout) return;

    // Limpiar zonaCentral
    QLayoutItem *child;
    while ((child = mainLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // Scroll para evitar desbordamiento visual
    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget *contenedor = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(contenedor);

    QLabel *titulo = new QLabel("➕ Agregar Canción");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titulo);

    QLineEdit *leTitulo = new QLineEdit;
    QComboBox *cbGenero = new QComboBox;
    cbGenero->addItems({"Pop", "Corridos", "Cristianos", "Electrónica", "Reguetón", "Rock", "Clásicas"});

    QComboBox *cbCategoria = new QComboBox;
    cbCategoria->addItems({"Playlist", "Recomendado", "Favorito", "Infantil", "Instrumental"});

    QLineEdit *leDuracion = new QLineEdit;
    QComboBox *cbTipo = new QComboBox;
    cbTipo->addItems({"Single", "EP", "Álbum"});

    QTextEdit *teDescripcion = new QTextEdit;
    teDescripcion->setFixedHeight(60);  // ✅ Descripción más compacta

    QLabel *lblPortada = new QLabel("Sin portada");
    lblPortada->setFixedSize(120, 120);
    lblPortada->setAlignment(Qt::AlignCenter);
    lblPortada->setStyleSheet("border: 1px solid gray; background-color: white;");

    QLabel *lblAudio = new QLabel("Sin archivo de audio");
    lblAudio->setAlignment(Qt::AlignCenter);

    QPushButton *btnCargarPortada = new QPushButton("Cargar Portada");
    QPushButton *btnCargarAudio = new QPushButton("Cargar Audio");
    QPushButton *btnGuardar = new QPushButton("Guardar Canción");

    QString *rutaPortada = new QString;
    QString *rutaAudio = new QString;

    // Cargar imagen
    connect(btnCargarPortada, &QPushButton::clicked, [=]() {
        QString ruta = QFileDialog::getOpenFileName(nullptr, "Seleccionar Portada", "", "Imágenes (*.png *.jpg *.jpeg *.bmp)");
        if (!ruta.isEmpty()) {
            *rutaPortada = ruta;
            QPixmap img(ruta);
            lblPortada->setPixmap(img.scaled(lblPortada->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    });

    // Cargar audio
    connect(btnCargarAudio, &QPushButton::clicked, [=]() {
        QString ruta = QFileDialog::getOpenFileName(nullptr, "Seleccionar Audio", "", "Audio (*.mp3 *.wav)");
        if (!ruta.isEmpty()) {
            *rutaAudio = ruta;
            lblAudio->setText(QFileInfo(ruta).fileName());
        }
    });

    // Guardar canción
    connect(btnGuardar, &QPushButton::clicked, [=]() {
        if (leTitulo->text().isEmpty() || *rutaAudio == "" || *rutaPortada == "") {
            QMessageBox::warning(nullptr, "Error", "Faltan campos obligatorios (Título, Portada, Audio).");
            return;
        }

        Cancion nueva;
        nueva.id = QDateTime::currentMSecsSinceEpoch();
        nueva.titulo = leTitulo->text();
        nueva.artista = nombreArtistaLogueado;
        nueva.genero = cbGenero->currentText();
        nueva.categoria = cbCategoria->currentText();
        nueva.duracion = leDuracion->text();
        nueva.descripcion = teDescripcion->toPlainText();
        nueva.portada = *rutaPortada;
        nueva.rutaArchivo = *rutaAudio;
        nueva.tipo = cbTipo->currentText();
        nueva.fechaCarga = QDate::currentDate();
        nueva.activo = true;

        ManejadorCanciones manejador;
        manejador.agregarCancion(nueva);

        QMessageBox::information(nullptr, "Éxito", "Canción guardada correctamente.");
    mostrarListaCancionesEditable();
    });

    // ---------- Añadir widgets al layout ----------

    layout->addWidget(new QLabel("Título:"));
    layout->addWidget(leTitulo);

    layout->addWidget(new QLabel("Género:"));
    layout->addWidget(cbGenero);

    layout->addWidget(new QLabel("Categoría:"));
    layout->addWidget(cbCategoria);

    layout->addWidget(new QLabel("Duración:"));
    layout->addWidget(leDuracion);

    layout->addWidget(new QLabel("Tipo:"));
    layout->addWidget(cbTipo);

    layout->addWidget(new QLabel("Descripción:"));
    layout->addWidget(teDescripcion);

    // Imagen y audio horizontal
    QHBoxLayout *archivosLayout = new QHBoxLayout;
    QVBoxLayout *portadaLayout = new QVBoxLayout;
    portadaLayout->addWidget(lblPortada);
    portadaLayout->addWidget(btnCargarPortada);

    QVBoxLayout *audioLayout = new QVBoxLayout;
    audioLayout->addWidget(lblAudio);
    audioLayout->addWidget(btnCargarAudio);

    archivosLayout->addLayout(portadaLayout);
    archivosLayout->addSpacing(20);
    archivosLayout->addLayout(audioLayout);
    layout->addLayout(archivosLayout);

    layout->addWidget(btnGuardar);
    layout->addStretch();

    scroll->setWidget(contenedor);
    mainLayout->addWidget(scroll);
}

void InterfazAdministrador::mostrarFormularioCrearAlbumEP(const QString &tipo) {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(zonaCentral->layout());
    if (!layout) return;

    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QLabel *titulo = new QLabel(QString("🎶 Crear %1").arg(tipo));
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titulo);

    QLineEdit *leNombre = new QLineEdit;
    QTextEdit *teDescripcion = new QTextEdit;
    teDescripcion->setFixedHeight(60);

    QLabel *lblImagen = new QLabel("Sin imagen");
    lblImagen->setFixedSize(120, 120);
    lblImagen->setAlignment(Qt::AlignCenter);
    lblImagen->setStyleSheet("border: 1px solid gray; background-color: white;");
    QPushButton *btnCargar = new QPushButton("Cargar Imagen");

    QPushButton *btnGuardar = new QPushButton("Guardar");

    QString *ruta = new QString;
    connect(btnCargar, &QPushButton::clicked, [=]() {
        QString file = QFileDialog::getOpenFileName(nullptr, "Seleccionar Imagen", "", "Imágenes (*.jpg *.png)");
        if (!file.isEmpty()) {
            *ruta = file;
            QPixmap img(file);
            lblImagen->setPixmap(img.scaled(lblImagen->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    });

    connect(btnGuardar, &QPushButton::clicked, [=]() {
        if (leNombre->text().isEmpty() || ruta->isEmpty()) {
            QMessageBox::warning(nullptr, "Error", "Faltan campos obligatorios.");
            return;
        }
        QMessageBox::information(nullptr, tipo, tipo + " creado correctamente.");
        mostrarFormularioAgregar();
    });

    layout->addWidget(new QLabel("Nombre:"));
    layout->addWidget(leNombre);
    layout->addWidget(new QLabel("Descripción:"));
    layout->addWidget(teDescripcion);
    layout->addWidget(lblImagen);
    layout->addWidget(btnCargar);
    layout->addWidget(btnGuardar);
    layout->addStretch();
}

void InterfazAdministrador::mostrarListaCancionesEditable() {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(zonaCentral->layout());
    if (!layout) return;

    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QLabel *titulo = new QLabel("✏️ Mis Canciones");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titulo);

    QListWidget *lista = new QListWidget;
    ManejadorCanciones manejador;
    QList<Cancion> canciones = manejador.obtenerPorArtista(nombreArtistaLogueado);

    for (const Cancion &c : canciones) {
        if (c.activo) {
            lista->addItem(QString("%1 - %2").arg(c.titulo, c.genero));
        }
    }

    layout->addWidget(lista);
    layout->addStretch();
}

void InterfazAdministrador::mostrarFormularioEditarPerfil() {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(zonaCentral->layout());
    if (!layout) return;

    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QLabel *titulo = new QLabel("📝 Editar Perfil");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold;");
    layout->addWidget(titulo);

    QLineEdit *leNombreReal = new QLineEdit;
    QLineEdit *leNombreArtistico = new QLineEdit;
    QTextEdit *teBiografia = new QTextEdit;
    teBiografia->setFixedHeight(60);

    QComboBox *cbPais = new QComboBox;
    cbPais->addItems({"Honduras", "México", "Colombia", "España", "Otro"});

    QPushButton *btnGuardar = new QPushButton("Guardar Cambios");

    layout->addWidget(new QLabel("Nombre Real:"));
    layout->addWidget(leNombreReal);

    layout->addWidget(new QLabel("Nombre Artístico:"));
    layout->addWidget(leNombreArtistico);

    layout->addWidget(new QLabel("Biografía:"));
    layout->addWidget(teBiografia);

    layout->addWidget(new QLabel("País de Origen:"));
    layout->addWidget(cbPais);

    layout->addWidget(btnGuardar);
    layout->addStretch();

    connect(btnGuardar, &QPushButton::clicked, [=]() {
        QMessageBox::information(nullptr, "Éxito", "Perfil actualizado correctamente.");
    });
}


// ----------------- SLOTS -----------------

void InterfazAdministrador::slotEditarPerfil() {
    mostrarFormularioEditarPerfil();  // Puedes crear esta función si aún no existe
}

void InterfazAdministrador::slotAgregarCancion() {
    mostrarFormularioAgregar();
}

void InterfazAdministrador::slotEditarEliminarCancion() {
    mostrarListaCancionesEditable();
}

void InterfazAdministrador::slotCrearAlbum() {
    mostrarFormularioCrearAlbumEP("Álbum");
}

void InterfazAdministrador::slotCrearEP() {
    mostrarFormularioCrearAlbumEP("EP");
}

void InterfazAdministrador::slotCerrarSesion() {
    // Subir al stackedWidget y cambiar al index 0 (login)
    QStackedWidget *stack = this->parentWidget()->findChild<QStackedWidget *>();
    if (stack) {
        stack->setCurrentIndex(0);
    } else {
        QMessageBox::warning(this, "Error", "No se encontró el stackedWidget.");
    }
}
