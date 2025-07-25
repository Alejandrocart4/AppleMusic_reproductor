/********************************************************************************
** Form generated from reading UI file 'cframe.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CFRAME_H
#define UI_CFRAME_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cframe
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *login;
    QPushButton *pushButton;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLabel *label;
    QLabel *label_2;
    QPushButton *btnEnlaceLogin;
    QWidget *page;
    QLabel *label_3;
    QFrame *line;
    QWidget *page_3;
    QSlider *volumen;
    QSlider *sliderProgreso;
    QLabel *labelTiempo;
    QPushButton *mute;
    QPushButton *play;
    QPushButton *abrir;
    QPushButton *pause;
    QPushButton *stop;
    QWidget *page_4;
    QWidget *page_crear;
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
        stackedWidget->setGeometry(QRect(-10, -10, 1581, 871));
        stackedWidget->setStyleSheet(QString::fromUtf8(""));
        login = new QWidget();
        login->setObjectName("login");
        login->setStyleSheet(QString::fromUtf8("#login {\n"
"    background-image: url(:/1/Imagenes/Fondo apple.webp);\n"
"    background-repeat: no-repeat;\n"
"    background-position: center;\n"
"    background-attachment: fixed;\n"
"}"));
        pushButton = new QPushButton(login);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(750, 680, 83, 29));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        pushButton->setFont(font);
        lineEdit = new QLineEdit(login);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(860, 580, 221, 28));
        QFont font1;
        font1.setPointSize(10);
        lineEdit->setFont(font1);
        lineEdit_2 = new QLineEdit(login);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setGeometry(QRect(500, 580, 221, 28));
        lineEdit_2->setFont(font1);
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
        label_3 = new QLabel(page);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(770, 680, 41, 20));
        line = new QFrame(page);
        line->setObjectName("line");
        line->setGeometry(QRect(690, -100, 181, 791));
        line->setFrameShape(QFrame::Shape::VLine);
        line->setFrameShadow(QFrame::Shadow::Sunken);
        stackedWidget->addWidget(page);
        page_3 = new QWidget();
        page_3->setObjectName("page_3");
        volumen = new QSlider(page_3);
        volumen->setObjectName("volumen");
        volumen->setGeometry(QRect(560, 380, 160, 18));
        volumen->setValue(20);
        volumen->setOrientation(Qt::Orientation::Horizontal);
        sliderProgreso = new QSlider(page_3);
        sliderProgreso->setObjectName("sliderProgreso");
        sliderProgreso->setGeometry(QRect(580, 230, 271, 16));
        sliderProgreso->setOrientation(Qt::Orientation::Horizontal);
        labelTiempo = new QLabel(page_3);
        labelTiempo->setObjectName("labelTiempo");
        labelTiempo->setGeometry(QRect(860, 230, 101, 20));
        mute = new QPushButton(page_3);
        mute->setObjectName("mute");
        mute->setGeometry(QRect(430, 370, 83, 29));
        play = new QPushButton(page_3);
        play->setObjectName("play");
        play->setGeometry(QRect(440, 220, 83, 29));
        abrir = new QPushButton(page_3);
        abrir->setObjectName("abrir");
        abrir->setGeometry(QRect(440, 180, 83, 29));
        pause = new QPushButton(page_3);
        pause->setObjectName("pause");
        pause->setGeometry(QRect(440, 260, 83, 29));
        stop = new QPushButton(page_3);
        stop->setObjectName("stop");
        stop->setGeometry(QRect(440, 300, 83, 29));
        stackedWidget->addWidget(page_3);
        page_4 = new QWidget();
        page_4->setObjectName("page_4");
        stackedWidget->addWidget(page_4);
        page_crear = new QWidget();
        page_crear->setObjectName("page_crear");
        stackedWidget->addWidget(page_crear);
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
        pushButton->setText(QCoreApplication::translate("cframe", "Entrar", nullptr));
        label->setText(QCoreApplication::translate("cframe", "Usuario:", nullptr));
        label_2->setText(QCoreApplication::translate("cframe", "Contrase\303\261a:", nullptr));
        btnEnlaceLogin->setText(QCoreApplication::translate("cframe", "\302\277No tienes cuenta? Crea una aqu\303\255", nullptr));
        label_3->setText(QCoreApplication::translate("cframe", "Crear", nullptr));
        labelTiempo->setText(QString());
        mute->setText(QCoreApplication::translate("cframe", "Mute", nullptr));
        play->setText(QCoreApplication::translate("cframe", "play", nullptr));
        abrir->setText(QCoreApplication::translate("cframe", "Abrir", nullptr));
        pause->setText(QCoreApplication::translate("cframe", "pause", nullptr));
        stop->setText(QCoreApplication::translate("cframe", "estop", nullptr));
    } // retranslateUi

};

namespace Ui {
    class cframe: public Ui_cframe {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CFRAME_H
