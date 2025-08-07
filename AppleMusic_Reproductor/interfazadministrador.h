#ifndef INTERFAZADMINISTRADOR_H
#define INTERFAZADMINISTRADOR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include "manejadorcanciones.h"
#include "cframe.h"

class cframe;  // Forward declaration

class InterfazAdministrador : public QWidget
{
    Q_OBJECT
public:
    explicit InterfazAdministrador(QWidget *parent = nullptr);

    void cargarEnPagina(QWidget *pagina);
    void setNombreArtista(const QString &nombre);
    void setPaginaPrincipal(cframe *pagina);  // Para controlar el stackedWidget al cerrar sesión

private slots:
    void slotEditarPerfil();
    void slotAgregarCancion();
    void slotEditarEliminarCancion();
    void slotVerEstadisticas();
    void slotCerrarSesion();

private:
    QString nombreArtistaLogueado;
    QWidget *zonaCentral;
    QWidget *zonaDerecha;
    cframe *paginaPrincipal;

    QWidget* crearMenuIzquierdo();
    QWidget* crearZonaCentral();
    QWidget* crearZonaDerecha();

    void mostrarListaCanciones();
    void mostrarFormularioAgregar();
    void mostrarFormularioCrearAlbum();
};

#endif // INTERFAZADMINISTRADOR_H
