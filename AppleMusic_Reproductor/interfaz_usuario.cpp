#include "interfaz_usuario.h"
#include "cframe.h"
#include "ui_cframe.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QScrollArea>
#include <QRadioButton>
#include <QPixmap>

InterfazUsuario::InterfazUsuario(cframe *ventana)
    : ventanaPrincipal(ventana) {}

void InterfazUsuario::cargarEnPagina(QWidget *pagina) {
    pagina->setStyleSheet("background-color: #121212; color: white;");
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(pagina);
    layoutPrincipal->setContentsMargins(12, 22, 0, 0);

    layoutPrincipal->addWidget(crearReproductor());

    QHBoxLayout *cuerpo = new QHBoxLayout;
    cuerpo->addWidget(crearMenuIzquierdo(), 1);
    cuerpo->addWidget(crearZonaCentral(), 3);
    cuerpo->addWidget(crearZonaDerecha(), 1);

    layoutPrincipal->addLayout(cuerpo);
    QHBoxLayout *filaCerrar = new QHBoxLayout;
    filaCerrar->addStretch(); // Empuja el botón a la derecha
    QPushButton *btnCerrarSesion = new QPushButton("🚪 Cerrar sesión");
    btnCerrarSesion->setStyleSheet(
        "background-color: #E74C3C; color: white; font-weight: bold; padding: 6px; border-radius: 4px;"
        );
    filaCerrar->addWidget(btnCerrarSesion);
    layoutPrincipal->addLayout(filaCerrar);

    // Acción para volver al login (índice 0 del stackedWidget)
    QObject::connect(btnCerrarSesion, &QPushButton::clicked, [=]() {
        // Subir al stackedWidget y cambiar al index 0
        QStackedWidget *stack = pagina->parentWidget()->findChild<QStackedWidget *>();
        if (stack) {
            stack->setCurrentIndex(0);
        }
    });
    pagina->setLayout(layoutPrincipal);
}



QWidget* InterfazUsuario::crearReproductor() {
    QWidget *w = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    // Usamos los widgets ya definidos en tu .ui
    lay->addWidget(ventanaPrincipal->getBtnAbrir());
    lay->addWidget(ventanaPrincipal->getBtnPause());
    lay->addWidget(ventanaPrincipal->getBtnPlay());
    lay->addWidget(ventanaPrincipal->getBtnStop());
    lay->addWidget(ventanaPrincipal->getBtnMute());

    QSlider *volumen = ventanaPrincipal->getSliderVolumen();
    volumen->setFixedSize(120, 12);  // ancho 80px, alto 12px
    lay->addWidget(volumen);

    lay->addWidget(ventanaPrincipal->getSliderProgreso());
    lay->addWidget(ventanaPrincipal->getLabelTiempo());

    lay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    return w;
}

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

    QStringList opciones = {"📚 Biblioteca", "❤️ Favoritos", "🎶 Playlists", "📁 Descargas"};
    for (const QString &text : opciones) {
        QPushButton *btn = new QPushButton(text);
        btn->setStyleSheet("text-align: left; padding: 5px;");
        lay->addWidget(btn);
    }

    lay->addStretch();
    return menu;
}

QWidget* InterfazUsuario::crearZonaCentral() {
    QWidget *centro = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(centro);

    QLabel *titulo = new QLabel("🎧 Tus canciones destacadas");
    titulo->setStyleSheet("font-size: 18px; font-weight: bold;");
    lay->addWidget(titulo);

    QScrollArea *scroll = new QScrollArea;
    QWidget *contenedor = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout(contenedor);

    QStringList canciones = {"Estación Chill", "Reggaetón Hits", "Top 2025", "Baladas Pop", "Word Trap"};
    for (const QString &nombre : canciones) {
        QLabel *card = new QLabel(nombre);
        card->setFixedSize(140, 140);
        card->setAlignment(Qt::AlignCenter);
        card->setStyleSheet("background-color: #2a2a2a; border-radius: 8px; padding: 10px;");
        hbox->addWidget(card);
    }

    contenedor->setLayout(hbox);
    scroll->setWidget(contenedor);
    scroll->setWidgetResizable(true);
    lay->addWidget(scroll);

    return centro;
}

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


    QPushButton *btnCerrarSesion = ventanaPrincipal->getBtnCerrarSesion();
    btnCerrarSesion->setStyleSheet(
        "background-color: #E74C3C; color: white; font-weight: bold; padding: 6px; border-radius: 4px;"
        );

    lay->addSpacing(300);
    lay->setContentsMargins(0, 0, 30, 0); // (izquierda, arriba, derecha, abajo)
    lay->addWidget(btnCerrarSesion, 0, Qt::AlignRight); // alineado a la derecha

    lay->addStretch();
    return derecha;
}
