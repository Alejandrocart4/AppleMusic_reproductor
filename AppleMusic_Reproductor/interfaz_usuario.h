#ifndef INTERFAZ_USUARIO_H
#define INTERFAZ_USUARIO_H

#include <QWidget>

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
};

#endif // INTERFAZ_USUARIO_H
