/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *imageButton;
    QLabel *inputLabel;
    QSlider *brightnessSlider;
    QSlider *contrastSlider;
    QPushButton *saveButton;
    QLabel *brightnessLabel;
    QLabel *contrastLabel;
    QLabel *outputImage;
    QPushButton *overlayButton;
    QLabel *inputImage;
    QLabel *outputLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        imageButton = new QPushButton(centralwidget);
        imageButton->setObjectName(QString::fromUtf8("imageButton"));
        imageButton->setGeometry(QRect(380, 0, 121, 31));
        inputLabel = new QLabel(centralwidget);
        inputLabel->setObjectName(QString::fromUtf8("inputLabel"));
        inputLabel->setGeometry(QRect(140, 540, 141, 20));
        brightnessSlider = new QSlider(centralwidget);
        brightnessSlider->setObjectName(QString::fromUtf8("brightnessSlider"));
        brightnessSlider->setGeometry(QRect(10, 0, 160, 16));
        brightnessSlider->setMinimum(0);
        brightnessSlider->setMaximum(200);
        brightnessSlider->setValue(100);
        brightnessSlider->setOrientation(Qt::Horizontal);
        contrastSlider = new QSlider(centralwidget);
        contrastSlider->setObjectName(QString::fromUtf8("contrastSlider"));
        contrastSlider->setGeometry(QRect(190, 0, 160, 16));
        contrastSlider->setMinimum(0);
        contrastSlider->setMaximum(511);
        contrastSlider->setValue(255);
        contrastSlider->setOrientation(Qt::Horizontal);
        saveButton = new QPushButton(centralwidget);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        saveButton->setGeometry(QRect(670, 0, 101, 31));
        brightnessLabel = new QLabel(centralwidget);
        brightnessLabel->setObjectName(QString::fromUtf8("brightnessLabel"));
        brightnessLabel->setGeometry(QRect(60, 20, 101, 16));
        contrastLabel = new QLabel(centralwidget);
        contrastLabel->setObjectName(QString::fromUtf8("contrastLabel"));
        contrastLabel->setGeometry(QRect(236, 20, 101, 20));
        outputImage = new QLabel(centralwidget);
        outputImage->setObjectName(QString::fromUtf8("outputImage"));
        outputImage->setGeometry(QRect(400, 70, 381, 451));
        overlayButton = new QPushButton(centralwidget);
        overlayButton->setObjectName(QString::fromUtf8("overlayButton"));
        overlayButton->setGeometry(QRect(530, -3, 101, 31));
        inputImage = new QLabel(centralwidget);
        inputImage->setObjectName(QString::fromUtf8("inputImage"));
        inputImage->setGeometry(QRect(10, 69, 381, 451));
        outputLabel = new QLabel(centralwidget);
        outputLabel->setObjectName(QString::fromUtf8("outputLabel"));
        outputLabel->setGeometry(QRect(590, 540, 151, 20));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        imageButton->setText(QCoreApplication::translate("MainWindow", "Select Image", nullptr));
        inputLabel->setText(QCoreApplication::translate("MainWindow", "  Selected Image", nullptr));
        saveButton->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        brightnessLabel->setText(QCoreApplication::translate("MainWindow", "    Brightness", nullptr));
        contrastLabel->setText(QCoreApplication::translate("MainWindow", "      Contrast", nullptr));
        outputImage->setText(QString());
        overlayButton->setText(QCoreApplication::translate("MainWindow", "Overlay", nullptr));
        inputImage->setText(QString());
        outputLabel->setText(QCoreApplication::translate("MainWindow", "    Resulting Image", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
