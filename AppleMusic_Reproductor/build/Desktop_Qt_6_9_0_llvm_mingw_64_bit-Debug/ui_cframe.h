/********************************************************************************
** Form generated from reading UI file 'cframe.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CFRAME_H
#define UI_CFRAME_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cframe
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *login;
    QPushButton *btnIniciarSesion;
    QLineEdit *leContraseniaLogin;
    QLineEdit *leAliasLogin;
    QLabel *label;
    QLabel *label_2;
    QPushButton *btnEnlaceLogin;
    QWidget *page;
    QComboBox *comboBoxTipoUsuario;
    QLineEdit *leContrasenia;
    QCheckBox *cbActivo;
    QGroupBox *gbAdministrador;
    QDateEdit *deFechaRegistro;
    QPushButton *btnCargarImagen;
    QTextEdit *teBiografia;
    QLineEdit *leGeneroMusical;
    QLineEdit *lePaisOrigen;
    QLineEdit *leNombreArtistico;
    QLineEdit *leNombreReal;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *lblPreviewImagen;
    QGroupBox *gbUsuarioComun;
    QDateEdit *deFechaNacimiento;
    QLineEdit *leGeneroPreferido;
    QLabel *label_11;
    QLabel *label_9;
    QLineEdit *leNombreUsuario;
    QLabel *label_10;
    QPushButton *btnCrearCuenta;
    QLabel *label_13;
    QPushButton *btnVolver1;
    QLineEdit *leAlias;
    QLabel *label_12;
    QWidget *Usuario;
    QGroupBox *groupBox;
    QGroupBox *groupBox_2;
    QPushButton *stop;
    QPushButton *pause;
    QPushButton *abrir;
    QSlider *volumen;
    QLabel *labelTiempo;
    QPushButton *play;
    QSlider *sliderProgreso;
    QPushButton *mute;
    QGroupBox *groupBox_3;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QGroupBox *groupBox_4;
    QPushButton *Cerrar_sesion1_2;
    QWidget *admin;
    QLabel *lblBienvenidaAdmin;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QLabel *lblImagenAdminInicio;
    QPushButton *Cerrar_sesion1;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *cframe)
    {
        if (cframe->objectName().isEmpty())
            cframe->setObjectName("cframe");
        cframe->resize(1596, 834);
        centralwidget = new QWidget(cframe);
        centralwidget->setObjectName("centralwidget");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(-20, -30, 1581, 871));
        stackedWidget->setStyleSheet(QString::fromUtf8(""));
        login = new QWidget();
        login->setObjectName("login");
        login->setStyleSheet(QString::fromUtf8("#login {\n"
"    background-image: url(:/1/Imagenes/Fondo apple.webp);\n"
"    background-repeat: no-repeat;\n"
"    background-position: center;\n"
"    background-attachment: fixed;\n"
"}"));
        btnIniciarSesion = new QPushButton(login);
        btnIniciarSesion->setObjectName("btnIniciarSesion");
        btnIniciarSesion->setGeometry(QRect(742, 680, 101, 29));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        btnIniciarSesion->setFont(font);
        leContraseniaLogin = new QLineEdit(login);
        leContraseniaLogin->setObjectName("leContraseniaLogin");
        leContraseniaLogin->setGeometry(QRect(860, 580, 221, 28));
        QFont font1;
        font1.setPointSize(10);
        leContraseniaLogin->setFont(font1);
        leAliasLogin = new QLineEdit(login);
        leAliasLogin->setObjectName("leAliasLogin");
        leAliasLogin->setGeometry(QRect(500, 580, 221, 28));
        leAliasLogin->setFont(font1);
        label = new QLabel(login);
        label->setObjectName("label");
        label->setGeometry(QRect(500, 560, 201, 21));
        label->setFont(font);
        label_2 = new QLabel(login);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(860, 560, 161, 21));
        label_2->setFont(font);
        btnEnlaceLogin = new QPushButton(login);
        btnEnlaceLogin->setObjectName("btnEnlaceLogin");
        btnEnlaceLogin->setGeometry(QRect(1310, 750, 231, 29));
        stackedWidget->addWidget(login);
        page = new QWidget();
        page->setObjectName("page");
        page->setStyleSheet(QString::fromUtf8(""));
        comboBoxTipoUsuario = new QComboBox(page);
        comboBoxTipoUsuario->addItem(QString());
        comboBoxTipoUsuario->addItem(QString());
        comboBoxTipoUsuario->addItem(QString());
        comboBoxTipoUsuario->setObjectName("comboBoxTipoUsuario");
        comboBoxTipoUsuario->setGeometry(QRect(960, 160, 161, 28));
        QFont font2;
        font2.setPointSize(10);
        font2.setBold(false);
        comboBoxTipoUsuario->setFont(font2);
        comboBoxTipoUsuario->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        leContrasenia = new QLineEdit(page);
        leContrasenia->setObjectName("leContrasenia");
        leContrasenia->setGeometry(QRect(650, 160, 161, 28));
        leContrasenia->setFont(font2);
        leContrasenia->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        cbActivo = new QCheckBox(page);
        cbActivo->setObjectName("cbActivo");
        cbActivo->setGeometry(QRect(840, 160, 93, 26));
        cbActivo->setFont(font2);
        cbActivo->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        gbAdministrador = new QGroupBox(page);
        gbAdministrador->setObjectName("gbAdministrador");
        gbAdministrador->setGeometry(QRect(470, 250, 651, 381));
        deFechaRegistro = new QDateEdit(gbAdministrador);
        deFechaRegistro->setObjectName("deFechaRegistro");
        deFechaRegistro->setEnabled(false);
        deFechaRegistro->setGeometry(QRect(450, 60, 131, 29));
        deFechaRegistro->setFont(font);
        deFechaRegistro->setDate(QDate(2004, 5, 3));
        btnCargarImagen = new QPushButton(gbAdministrador);
        btnCargarImagen->setObjectName("btnCargarImagen");
        btnCargarImagen->setGeometry(QRect(500, 320, 83, 29));
        btnCargarImagen->setFont(font);
        teBiografia = new QTextEdit(gbAdministrador);
        teBiografia->setObjectName("teBiografia");
        teBiografia->setGeometry(QRect(20, 180, 361, 131));
        leGeneroMusical = new QLineEdit(gbAdministrador);
        leGeneroMusical->setObjectName("leGeneroMusical");
        leGeneroMusical->setGeometry(QRect(210, 90, 171, 28));
        lePaisOrigen = new QLineEdit(gbAdministrador);
        lePaisOrigen->setObjectName("lePaisOrigen");
        lePaisOrigen->setGeometry(QRect(10, 90, 171, 28));
        leNombreArtistico = new QLineEdit(gbAdministrador);
        leNombreArtistico->setObjectName("leNombreArtistico");
        leNombreArtistico->setGeometry(QRect(10, 30, 171, 28));
        leNombreReal = new QLineEdit(gbAdministrador);
        leNombreReal->setObjectName("leNombreReal");
        leNombreReal->setGeometry(QRect(210, 30, 171, 28));
        label_5 = new QLabel(gbAdministrador);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 10, 161, 20));
        label_5->setFont(font);
        label_6 = new QLabel(gbAdministrador);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 70, 161, 20));
        label_6->setFont(font);
        label_7 = new QLabel(gbAdministrador);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(210, 10, 161, 20));
        label_7->setFont(font);
        label_8 = new QLabel(gbAdministrador);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(210, 70, 161, 20));
        label_8->setFont(font);
        label_3 = new QLabel(gbAdministrador);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(50, 160, 191, 20));
        label_3->setFont(font);
        label_4 = new QLabel(gbAdministrador);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(440, 40, 161, 20));
        label_4->setFont(font);
        lblPreviewImagen = new QLabel(gbAdministrador);
        lblPreviewImagen->setObjectName("lblPreviewImagen");
        lblPreviewImagen->setGeometry(QRect(430, 100, 200, 200));
        lblPreviewImagen->setStyleSheet(QString::fromUtf8("border: 1px solid gray;"));
        lblPreviewImagen->setScaledContents(true);
        gbUsuarioComun = new QGroupBox(page);
        gbUsuarioComun->setObjectName("gbUsuarioComun");
        gbUsuarioComun->setGeometry(QRect(520, 240, 521, 91));
        deFechaNacimiento = new QDateEdit(gbUsuarioComun);
        deFechaNacimiento->setObjectName("deFechaNacimiento");
        deFechaNacimiento->setGeometry(QRect(350, 50, 110, 29));
        deFechaNacimiento->setDate(QDate(2004, 5, 3));
        leGeneroPreferido = new QLineEdit(gbUsuarioComun);
        leGeneroPreferido->setObjectName("leGeneroPreferido");
        leGeneroPreferido->setGeometry(QRect(190, 50, 141, 28));
        label_11 = new QLabel(gbUsuarioComun);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(190, 30, 151, 20));
        label_11->setFont(font);
        label_9 = new QLabel(gbUsuarioComun);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(350, 30, 151, 20));
        label_9->setFont(font);
        leNombreUsuario = new QLineEdit(gbUsuarioComun);
        leNombreUsuario->setObjectName("leNombreUsuario");
        leNombreUsuario->setGeometry(QRect(20, 50, 151, 28));
        label_10 = new QLabel(gbUsuarioComun);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(20, 30, 81, 20));
        label_10->setFont(font);
        btnCrearCuenta = new QPushButton(page);
        btnCrearCuenta->setObjectName("btnCrearCuenta");
        btnCrearCuenta->setGeometry(QRect(750, 660, 83, 29));
        btnCrearCuenta->setFont(font);
        label_13 = new QLabel(page);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(650, 140, 81, 20));
        label_13->setFont(font);
        label_13->setScaledContents(false);
        btnVolver1 = new QPushButton(page);
        btnVolver1->setObjectName("btnVolver1");
        btnVolver1->setGeometry(QRect(1460, 750, 83, 29));
        leAlias = new QLineEdit(page);
        leAlias->setObjectName("leAlias");
        leAlias->setGeometry(QRect(470, 160, 161, 28));
        leAlias->setFont(font2);
        leAlias->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        label_12 = new QLabel(page);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(468, 140, 61, 20));
        label_12->setFont(font);
        stackedWidget->addWidget(page);
        Usuario = new QWidget();
        Usuario->setObjectName("Usuario");
        groupBox = new QGroupBox(Usuario);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(50, 50, 321, 731));
        groupBox_2 = new QGroupBox(Usuario);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(420, 30, 781, 121));
        stop = new QPushButton(groupBox_2);
        stop->setObjectName("stop");
        stop->setGeometry(QRect(400, 70, 83, 29));
        pause = new QPushButton(groupBox_2);
        pause->setObjectName("pause");
        pause->setGeometry(QRect(220, 70, 83, 29));
        abrir = new QPushButton(groupBox_2);
        abrir->setObjectName("abrir");
        abrir->setGeometry(QRect(90, 70, 83, 29));
        volumen = new QSlider(groupBox_2);
        volumen->setObjectName("volumen");
        volumen->setGeometry(QRect(560, 80, 160, 18));
        volumen->setValue(20);
        volumen->setOrientation(Qt::Orientation::Horizontal);
        labelTiempo = new QLabel(groupBox_2);
        labelTiempo->setObjectName("labelTiempo");
        labelTiempo->setGeometry(QRect(610, 40, 101, 20));
        play = new QPushButton(groupBox_2);
        play->setObjectName("play");
        play->setGeometry(QRect(310, 70, 83, 29));
        sliderProgreso = new QSlider(groupBox_2);
        sliderProgreso->setObjectName("sliderProgreso");
        sliderProgreso->setGeometry(QRect(130, 40, 481, 20));
        sliderProgreso->setOrientation(Qt::Orientation::Horizontal);
        mute = new QPushButton(groupBox_2);
        mute->setObjectName("mute");
        mute->setGeometry(QRect(500, 80, 51, 21));
        groupBox_3 = new QGroupBox(Usuario);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(1240, 30, 321, 731));
        label_14 = new QLabel(groupBox_3);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(80, 20, 161, 151));
        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(30, 230, 63, 20));
        label_16 = new QLabel(groupBox_3);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(30, 300, 63, 20));
        label_17 = new QLabel(groupBox_3);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(30, 370, 63, 20));
        groupBox_4 = new QGroupBox(Usuario);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setGeometry(QRect(420, 160, 781, 621));
        Cerrar_sesion1_2 = new QPushButton(Usuario);
        Cerrar_sesion1_2->setObjectName("Cerrar_sesion1_2");
        Cerrar_sesion1_2->setGeometry(QRect(1450, 770, 111, 31));
        stackedWidget->addWidget(Usuario);
        admin = new QWidget();
        admin->setObjectName("admin");
        lblBienvenidaAdmin = new QLabel(admin);
        lblBienvenidaAdmin->setObjectName("lblBienvenidaAdmin");
        lblBienvenidaAdmin->setGeometry(QRect(620, 280, 261, 101));
        lblBienvenidaAdmin->setStyleSheet(QString::fromUtf8("font-size: 16px;\n"
"font-weight: bold;"));
        pushButton = new QPushButton(admin);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(740, 220, 111, 31));
        pushButton_2 = new QPushButton(admin);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(740, 300, 111, 31));
        pushButton_3 = new QPushButton(admin);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setGeometry(QRect(740, 380, 111, 31));
        pushButton_4 = new QPushButton(admin);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setGeometry(QRect(740, 540, 111, 31));
        pushButton_5 = new QPushButton(admin);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setGeometry(QRect(740, 460, 111, 31));
        lblImagenAdminInicio = new QLabel(admin);
        lblImagenAdminInicio->setObjectName("lblImagenAdminInicio");
        lblImagenAdminInicio->setGeometry(QRect(280, 80, 200, 200));
        lblImagenAdminInicio->setStyleSheet(QString::fromUtf8("border: 1px solid gray;"));
        lblImagenAdminInicio->setScaledContents(true);
        Cerrar_sesion1 = new QPushButton(admin);
        Cerrar_sesion1->setObjectName("Cerrar_sesion1");
        Cerrar_sesion1->setGeometry(QRect(1460, 760, 111, 31));
        stackedWidget->addWidget(admin);
        cframe->setCentralWidget(centralwidget);
        menubar = new QMenuBar(cframe);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1596, 25));
        cframe->setMenuBar(menubar);
        statusbar = new QStatusBar(cframe);
        statusbar->setObjectName("statusbar");
        cframe->setStatusBar(statusbar);

        retranslateUi(cframe);

        stackedWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(cframe);
    } // setupUi

    void retranslateUi(QMainWindow *cframe)
    {
        cframe->setWindowTitle(QCoreApplication::translate("cframe", "cframe", nullptr));
        btnIniciarSesion->setText(QCoreApplication::translate("cframe", "Iniciar Sesi\303\263n", nullptr));
        label->setText(QCoreApplication::translate("cframe", "Usuario:", nullptr));
        label_2->setText(QCoreApplication::translate("cframe", "Contrase\303\261a:", nullptr));
        btnEnlaceLogin->setText(QCoreApplication::translate("cframe", "\302\277No tienes cuenta? Crea una aqu\303\255", nullptr));
        comboBoxTipoUsuario->setItemText(0, QCoreApplication::translate("cframe", "Seleccionar", nullptr));
        comboBoxTipoUsuario->setItemText(1, QCoreApplication::translate("cframe", "Admi (Artista)", nullptr));
        comboBoxTipoUsuario->setItemText(2, QCoreApplication::translate("cframe", "Usuario", nullptr));

        cbActivo->setText(QCoreApplication::translate("cframe", "Activo", nullptr));
        gbAdministrador->setTitle(QString());
        btnCargarImagen->setText(QCoreApplication::translate("cframe", "Cargar", nullptr));
        label_5->setText(QCoreApplication::translate("cframe", "Nombre art\303\255stico:", nullptr));
        label_6->setText(QCoreApplication::translate("cframe", "Pais de origen:", nullptr));
        label_7->setText(QCoreApplication::translate("cframe", "Nombre Real:", nullptr));
        label_8->setText(QCoreApplication::translate("cframe", "Genero", nullptr));
        label_3->setText(QCoreApplication::translate("cframe", "Biograf\303\255a:", nullptr));
        label_4->setText(QCoreApplication::translate("cframe", "Fecha de Reg\303\255stro:", nullptr));
        lblPreviewImagen->setText(QString());
        gbUsuarioComun->setTitle(QString());
        label_11->setText(QCoreApplication::translate("cframe", "Genero Prefer\303\255do:", nullptr));
        label_9->setText(QCoreApplication::translate("cframe", "Fecha de nacimiento:", nullptr));
        label_10->setText(QCoreApplication::translate("cframe", "Nombre:", nullptr));
        btnCrearCuenta->setText(QCoreApplication::translate("cframe", "Crear", nullptr));
        label_13->setText(QCoreApplication::translate("cframe", "Contrase\303\261a:", nullptr));
        btnVolver1->setText(QCoreApplication::translate("cframe", "Volver", nullptr));
        label_12->setText(QCoreApplication::translate("cframe", "Usuario:", nullptr));
        groupBox->setTitle(QString());
        groupBox_2->setTitle(QString());
        stop->setText(QCoreApplication::translate("cframe", "stop", nullptr));
        pause->setText(QCoreApplication::translate("cframe", "pause", nullptr));
        abrir->setText(QCoreApplication::translate("cframe", "Abrir", nullptr));
        labelTiempo->setText(QString());
        play->setText(QCoreApplication::translate("cframe", "play", nullptr));
        mute->setText(QCoreApplication::translate("cframe", "Mute", nullptr));
        groupBox_3->setTitle(QString());
        label_14->setText(QCoreApplication::translate("cframe", "TextLabel", nullptr));
        label_15->setText(QCoreApplication::translate("cframe", "TextLabel", nullptr));
        label_16->setText(QCoreApplication::translate("cframe", "TextLabel", nullptr));
        label_17->setText(QCoreApplication::translate("cframe", "TextLabel", nullptr));
        groupBox_4->setTitle(QString());
        Cerrar_sesion1_2->setText(QCoreApplication::translate("cframe", "Cerrar Sesion", nullptr));
        lblBienvenidaAdmin->setText(QString());
        pushButton->setText(QCoreApplication::translate("cframe", "Agregar", nullptr));
        pushButton_2->setText(QCoreApplication::translate("cframe", "Editar", nullptr));
        pushButton_3->setText(QCoreApplication::translate("cframe", "Eliminar", nullptr));
        pushButton_4->setText(QCoreApplication::translate("cframe", "PushButton", nullptr));
        pushButton_5->setText(QCoreApplication::translate("cframe", "PushButton", nullptr));
        lblImagenAdminInicio->setText(QString());
        Cerrar_sesion1->setText(QCoreApplication::translate("cframe", "Cerrar Sesion", nullptr));
    } // retranslateUi

};

namespace Ui {
    class cframe: public Ui_cframe {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CFRAME_H
