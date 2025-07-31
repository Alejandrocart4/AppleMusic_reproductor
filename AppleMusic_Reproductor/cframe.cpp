#include "cframe.h"
#include "ui_cframe.h"
#include <QFileDialog>
#include <QMessageBox>
#include "usuarios.h"

cframe::cframe(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::cframe)
{
    ui->setupUi(this);
    reproducer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    reproducer ->setAudioOutput(audioOutput);

    // Usuario empieza a mover el slider
    connect(ui->sliderProgreso, &QSlider::sliderPressed, this, [=]() {
        usuarioControlandoSlider = true;
    });

    // Usuario suelta el slider
    connect(ui->sliderProgreso, &QSlider::sliderReleased, this, [=]() {
        usuarioControlandoSlider = false;
        reproducer->setPosition(ui->sliderProgreso->value());
    });

    // Canción avanza: actualizamos el slider solo si el usuario no lo está tocando
    connect(reproducer, &QMediaPlayer::positionChanged, this, [=](qint64 pos){
        if (!usuarioControlandoSlider) {
            ui->sliderProgreso->setValue(pos);
        }

        QTime tiempoActual(0, 0, 0);
        QTime tiempoTotal(0, 0, 0);
        tiempoActual = tiempoActual.addMSecs(pos);
        tiempoTotal = tiempoTotal.addMSecs(reproducer->duration());

        ui->labelTiempo->setText(tiempoActual.toString("mm:ss") + " / " + tiempoTotal.toString("mm:ss"));
    });

    connect(reproducer, &QMediaPlayer::durationChanged, this, [=](qint64 dur) {
        ui->sliderProgreso->setMaximum(dur);
    });

    ui->sliderProgreso->setStyleSheet(R"(
    /* 🟫 Riel completo del slider */
    QSlider::groove:horizontal {
        height: 6px;
        background: #e0e0e0;
        border-radius: 3px;
    }

    /* 🟩 Parte ya recorrida */
    QSlider::sub-page:horizontal {
        background: #9E9E9E;  /* Gris medio */
        border-radius: 3px;
    }

    /* ⬜ Parte restante (opcional) */
    QSlider::add-page:horizontal {
        background: #e0e0e0;
        border-radius: 3px;
    }

    /* 🔲 Perilla rectangular, delgada, gris claro */
    QSlider::handle:horizontal {
        background: #CCCCCC;          /* Gris claro */
        border: 1px solid #888888;    /* Gris medio oscuro */
        width: 4px;                   /* Delgado */
        height: 8px;                 /* Un poco alto para distinguirlo */
        margin: -7px 0;               /* Centrar verticalmente */
        border-radius: 1px;           /* Casi sin curvas → rectangular */
    }

    QSlider::handle:horizontal:hover {
        background: #DDDDDD;
        border: 1px solid #666666;
    }
)");

    // boton trasnparente login
    ui->btnEnlaceLogin->setStyleSheet(R"(
    QPushButton {
        background-color: transparent;
        border: none;
        color: #1a73e8;
        font-size: 14px;
        font-weight: normal;
        text-decoration: underline;
    }

    QPushButton:hover {
        color: #0b59d0;
        text-decoration: underline;
    }
)");
    connect(ui->btnEnlaceLogin, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(1);
    });

    //Grupos ui
    // Ocultar todo al inicio
    ui->gbAdministrador->hide();
    ui->gbUsuarioComun->hide();

    connect(ui->comboBoxTipoUsuario, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int index) {

                // Mostrar según selección
                switch (index) {
                case 1: // Administrador (Artista)
                    ui->gbUsuarioComun->hide();
                    ui->gbAdministrador->show();
                    break;
                case 2: // Usuario común
                    ui->gbAdministrador->hide();
                    ui->gbUsuarioComun->show();
                    break;
                default:
                    break;
                }
            });

    // Agarra fecha de la compu
    ui->deFechaRegistro->setDate(QDate::currentDate());



}


cframe::~cframe()
{
    delete ui;
}



void cframe::on_abrir_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Abrir");
    if (!filename.isEmpty()) {
        reproducer->setSource(QUrl::fromLocalFile(filename));
        audioOutput->setVolume(ui->volumen->value() / 100.0);
        reproducer->play();
    }


}


void cframe::on_play_clicked()
{
    reproducer->play();

}


void cframe::on_pause_clicked()
{
    reproducer->pause();
}


void cframe::on_stop_clicked()
{
    reproducer->stop();
}


void cframe::on_mute_clicked()
{
    if(ui->mute->text()=="Mute"){
        audioOutput->setMuted(true);
        ui->mute->setText("Unmute");
    }else {
        audioOutput->setMuted(false);
        ui->mute->setText("Mute");
    }
}


void cframe::on_volumen_valueChanged(int value)
{
    audioOutput->setVolume(ui->volumen->value() / 100.0);
}

void cframe::on_btnCrearCuenta_clicked()
{
    QString Alias = ui->leAlias->text().trimmed();
    QString contra = ui->leContrasenia->text().trimmed();
    bool activo = ui->cbActivo->isChecked();

    if (Alias.isEmpty() || contra.isEmpty()) {
        QMessageBox::warning(this, "Error", "Nombre de usuario y contraseña son obligatorios.");
        return;
    }

    int tipoSeleccionado = ui->comboBoxTipoUsuario->currentIndex();

    if (tipoSeleccionado != 1 && tipoSeleccionado != 2) {
        QMessageBox::warning(this, "Error", "Selecciona un tipo de usuario válido.");
        return;
    }

    // Verificar si el nombre de usuario ya existe
    ManejadorUsuarios manejador;
    QList<Usuario> lista = manejador.obtenerUsuarios();
    for (const Usuario &u : lista) {
        if (u.aliasre.compare(Alias, Qt::CaseInsensitive) == 0) {
            QMessageBox::warning(this, "Error", "Este nombre de usuario ya está registrado.");
            return;
        }
    }

    Usuario nuevo;
    nuevo.aliasre = Alias;
    nuevo.contrasenia = contra;
    nuevo.activo = activo;
    nuevo.fechaRegistro = QDate::currentDate();

    if (tipoSeleccionado == 1) { // ADMINISTRADOR
        QString nombreArtistico = ui->leNombreArtistico->text().trimmed();
        QString nombreReal = ui->leNombreReal->text().trimmed();
        QString pais = ui->lePaisOrigen->text().trimmed();
        QString genero = ui->leGeneroMusical->text().trimmed();
        QString bio = ui->teBiografia->toPlainText().trimmed();

        if (nombreArtistico.isEmpty() ||
            nombreReal.isEmpty() ||
            pais.isEmpty() ||
            genero.isEmpty() ||
            bio.isEmpty() //||
            //rutaSeleccionadaDesdeBoton.isEmpty()
            ) {
            QMessageBox::warning(this, "Error", "Todos los campos del artista son obligatorios (incluyendo imagen).");
            return;
        }

        nuevo.tipo = Administrador;
        nuevo.nombreArtistico = nombreArtistico;
        nuevo.nombreReal = nombreReal;
        nuevo.paisOrigen = pais;
        nuevo.generoMusical = genero;
        nuevo.biografia = bio;
        nuevo.rutaImagen = rutaSeleccionadaDesdeBoton;
    }
    else if (tipoSeleccionado == 2) { // USUARIO COMÚN
        QString NombreRealUsuario = ui->leNombreUsuario->text().trimmed();
        QString generoPref = ui->leGeneroPreferido->text().trimmed();
        QDate fechaNac = ui->deFechaNacimiento->date();

        if (NombreRealUsuario.isEmpty() ||
            generoPref.isEmpty() ||
            !fechaNac.isValid()) {
            QMessageBox::warning(this, "Error", "Todos los campos del usuario común son obligatorios.");
            return;
        }

        nuevo.tipo = UsuarioComun;
        nuevo.NombreUsuario = NombreRealUsuario;
        nuevo.generoPreferido = generoPref;
        nuevo.fechaNacimiento = fechaNac;
    }

    // Si todo está bien, guardar el usuario
    manejador.agregarUsuario(nuevo);
    QMessageBox::information(this, "Éxito", "¡Cuenta creada correctamente!");

    // Limpiar campos
    ui->leAlias->clear();
    ui->leContrasenia->clear();
    ui->cbActivo->setChecked(false);
    ui->leNombreArtistico->clear();
    ui->leNombreReal->clear();
    ui->lePaisOrigen->clear();
    ui->leGeneroMusical->clear();
    ui->teBiografia->clear();
    ui->lblPreviewImagen->clear();
    rutaSeleccionadaDesdeBoton.clear();
    ui->leNombreUsuario->clear();
    ui->comboBoxTipoUsuario->setCurrentIndex(0);
    ui->deFechaNacimiento->setDate(ui->deFechaNacimiento->date());
    ui->leGeneroPreferido->clear();

    //grupos
    ui->gbAdministrador->hide();
    ui->gbUsuarioComun->hide();

}


void cframe::on_btnVolver1_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->leAliasLogin->clear();
    ui->leContraseniaLogin->clear();
}


void cframe::on_btnCargarImagen_clicked()
{
    QString ruta = QFileDialog::getOpenFileName(
        this,
        "Seleccionar imagen del artista",
        QDir::homePath(),
        "Imágenes (*.png *.jpg *.jpeg *.bmp)"
        );

    if (!ruta.isEmpty()) {
        QPixmap imagen(ruta);
        ui->lblPreviewImagen->setPixmap(imagen.scaled(
            ui->lblPreviewImagen->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));

        rutaSeleccionadaDesdeBoton = ruta; // <- guarda la ruta para luego guardarla en binario
    }
}


void cframe::on_btnIniciarSesion_clicked()
{
    QString alias = ui->leAliasLogin->text().trimmed();
    QString contra = ui->leContraseniaLogin->text().trimmed();

    if (alias.isEmpty() || contra.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debes ingresar alias y contraseña.");
        return;
    }

    ManejadorUsuarios manejador;
    QList<Usuario> usuarios = manejador.obtenerUsuarios();

    for (const Usuario &u : usuarios) {
        if (u.aliasre.compare(alias, Qt::CaseInsensitive) == 0) {

            if (!u.activo) {
                QMessageBox::warning(this, "Cuenta inactiva", "Este usuario está deshabilitado.");
                return;
            }

            if (u.contrasenia != contra) {
                QMessageBox::warning(this, "Contraseña incorrecta", "La contraseña ingresada no es válida.");
                return;
            }

            if (u.tipo == Administrador) {
                QMessageBox::information(this, "Bienvenido", "Bienvenido, " + u.nombreArtistico);

                // Mostrar imagen del administrador en el QLabel
                if (!u.rutaImagen.isEmpty() && QFile::exists(u.rutaImagen)) {
                    QPixmap img(u.rutaImagen);
                    ui->lblImagenAdminInicio->setPixmap(img.scaled(
                        ui->lblImagenAdminInicio->size(),
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation
                        ));
                } else {
                    ui->lblImagenAdminInicio->clear();
                }

                ui->stackedWidget->setCurrentIndex(3); // Página del admin
            }
            else if (u.tipo == UsuarioComun) {
                QMessageBox::information(this, "Bienvenido", "Bienvenido, " + u.NombreUsuario);
                ui->stackedWidget->setCurrentIndex(2); // Página del usuario
            }

            // Limpiar campos de login
            ui->leAliasLogin->clear();
            ui->leContraseniaLogin->clear();

            return;
        }
    }

    // Usuario no encontrado
    QMessageBox::warning(this, "Error", "El alias ingresado no está registrado.");
}


void cframe::on_Cerrar_sesion1_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void cframe::on_Cerrar_sesion1_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

