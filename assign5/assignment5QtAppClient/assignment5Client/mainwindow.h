#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cmath>//for using floor()

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QPixmap overlayImage;
    QPixmap modifiedImage;

private slots:
    void on_saveButton_clicked();
    void on_overlayButton_clicked();
    void on_imageButton_clicked();
    void on_brightnessSlider_valueChanged(int value);
    void on_contrastSlider_valueChanged(int value);

    void applySliders(); //apply slider brightness and contrast to output image

    void on_brightnessSlider_sliderReleased();

    void on_contrastSlider_sliderReleased();

    void sendParameters();

    void sendImage(QImage sentImage);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
