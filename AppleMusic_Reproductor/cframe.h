#ifndef CFRAME_H
#define CFRAME_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>

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

private slots:
    void on_abrir_clicked();

    void on_play_clicked();

    void on_pause_clicked();

    void on_stop_clicked();

    void on_mute_clicked();

    void on_volumen_valueChanged(int value);


private:
    Ui::cframe *ui;
    QMediaPlayer *reproducer;
    QAudioOutput *audioOutput;
    bool usuarioControlandoSlider = false;
};
#endif // CFRAME_H
