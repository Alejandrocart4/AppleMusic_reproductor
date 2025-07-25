#include "cframe.h"
#include "ui_cframe.h"
#include <QFileDialog>

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
