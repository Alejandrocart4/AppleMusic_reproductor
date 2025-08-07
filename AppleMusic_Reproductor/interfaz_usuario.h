#ifndef INTERFAZ_USUARIO_H
#define INTERFAZ_USUARIO_H

#include <QWidget>
#include <QPushButton>

class cframe;  // Declaración adelantada

class InterfazUsuario {
public:
    InterfazUsuario(cframe *ventana);
    void cargarEnPagina(QWidget *pagina);

private:
    cframe *ventanaPrincipal;

    QWidget* crearReproductor();
    QWidget* crearMenuIzquierdo();
    QWidget* crearZonaCentral();
    QWidget* crearZonaDerecha();

    QPushButton *btnBiblioteca;
    QPushButton *btnFavoritos;
    QPushButton *btnPlaylists;
    QPushButton *btnDescargas;
    QWidget *zonaCentral;


    void mostrarVistaBiblioteca();
    void mostrarVistaFavoritos();
    void mostrarVistaPlaylists();
    void mostrarVistaDescargas();
    void reproducirCancion(const QString &ruta);
};

#endif // INTERFAZ_USUARIO_H
