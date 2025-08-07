#ifndef CFRAME_H
#define CFRAME_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "manejadorcanciones.h"
#include "cancion.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class cframe;
}
QT_END_NAMESPACE

class QMediaPlayer;

class cframe : public QMainWindow
{
    Q_OBJECT

public:
    cframe(QWidget *parent = nullptr);
    ~cframe();
    QPushButton* getBtnPlay() const;
    QPushButton* getBtnPause() const;
    QPushButton* getBtnStop() const;
    QPushButton* getBtnAbrir() const;
    QPushButton* getBtnMute() const;
    QSlider* getSliderProgreso() const;
    QSlider* getSliderVolumen() const;
    QLabel* getLabelTiempo() const;
    QPushButton* getBtnCerrarSesion() const;
    void regresarAlInicio();
    void mostrarCancionesParaUsuario();

private slots:
    void on_abrir_clicked();

    void on_play_clicked();

    void on_pause_clicked();

    void on_stop_clicked();

    void on_mute_clicked();

    void on_volumen_valueChanged(int value);


    void on_btnCrearCuenta_clicked();

    void on_btnVolver1_clicked();

    void on_btnCargarImagen_clicked();

    void on_btnIniciarSesion_clicked();

    void on_Cerrar_sesion1_clicked();

    void on_Cerrar_sesion1_2_clicked();

private:
    Ui::cframe *ui;
    QMediaPlayer *reproducer;
    QAudioOutput *audioOutput;
    bool usuarioControlandoSlider = false;
    QString rutaSeleccionadaDesdeBoton;

    void cargarVistaAppleMusic();
    QWidget* crearSeccionHorizontal(const QString &titulo, const QStringList &items);
    //void mostrarCancionesParaUsuario();
};
#endif // CFRAME_H
