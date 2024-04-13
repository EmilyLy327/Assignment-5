#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QPainter>
#include <QtNetwork>

///Assigning port and ipaddress for communications
/// IP address must match server and needs to be verified every session
/// and updated accordingly, set up using UDP but tried TCP
QTcpSocket* TCPSOCKET;
QHostAddress TCPHOST = QHostAddress("192.168.1.109");
const static int TCPPORT = 80;

std::string FPGAboardDisplayValues = "5050"; ///string to represent the seven segment displays initizlized

//slider values
double brightness = 100.0;
double contrast = 255.0;

// overlay flag and constrastSlider and brightnessSlider strings
std::string overlay = "F";
std::string contrastSliderDisplayString = "50";
std::string brightnessSliderDisplayString = "50";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Establish the connection
    ui->setupUi(this);
    TCPSOCKET = new QTcpSocket(this); //Create new socket on startup
    connect( TCPSOCKET, SIGNAL(readyRead()), SLOT(readTcpData()));
    TCPSOCKET -> connectToHost(TCPHOST, TCPPORT);
}

MainWindow::~MainWindow()
{
    // Collecting garbage
    delete ui;
    delete TCPSOCKET;
}

void MainWindow::on_saveButton_clicked()
{
    //acquires the image from widget
    QImage savedImage = ui->outputImage->pixmap()->toImage();
    //then gets the file name as is assigned accordingly by default
    QString sFile = QFileDialog::getSaveFileName(this, tr("Save Output Image"), "ECE484W_Assignment4.bmp", tr("Images (*.bmp)"));
    //as ling as the file name is not empty saves the image in the location specified by the user
    if (sFile != "")
    {
        savedImage.save(sFile);
    }
}

void MainWindow::on_overlayButton_clicked()
{
    //initializing the output image object and selecting the file
    QImage outputImage = ui->outputImage->pixmap()->toImage();
    QString selectFile = QFileDialog::getOpenFileName(this, tr("Select"), "", tr("Images (*.bmp)"));

    //compare the files
    if (QString::compare(selectFile, QString()))
    {
        // load the selected image and scale and identify the over and under images and execute the overlay
        QImage nImage;
        if (nImage.load(selectFile))
        {
            //scaling the image
            nImage = nImage.scaledToWidth(ui->outputImage->width(), Qt::SmoothTransformation);

            // Identifiying the over and under image and executing the overlay
            overlayImage = QPixmap::fromImage(nImage);
            QPixmap underImage = QPixmap::fromImage(outputImage);

            if (!underImage.isNull() && !overlayImage.isNull()) {

                QImage bottomImage = underImage.toImage();
                QImage topImage = overlayImage.toImage();
                QImage resultingImage = bottomImage;
                QPainter painter(&resultingImage);
                // image is painted over
                painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                painter.drawImage(0, 0, topImage);

                // Display the resulting image
                ui->outputImage->setPixmap(QPixmap::fromImage(resultingImage));
            }
        }
        else
        {
            // Print an error if the image will not load
            ui->inputImage->setText("File Invalid");
        }
    }
    else
    {
        ui->inputImage->setText("File Invalid");
    }
}

void MainWindow::on_imageButton_clicked()
{
    //gets file name
    QString selectFile = QFileDialog::getOpenFileName(this, tr("Select"), "", tr("Images (*.bmp)"));

    //this block checks to make sure the appropriate file has been selected and if not
    //displays the appropriate error message
    if (QString::compare(selectFile, QString()))
    {
        QImage nImage;
        //check to make sure the file can be loaded as an image
        if (nImage.load(selectFile))
        {
            // The image is scaled
            const int imageWidth = ui->inputImage->width();
            nImage = nImage.scaledToWidth(imageWidth, Qt::SmoothTransformation);

            // The image is displayed
            ui->inputImage->setPixmap(QPixmap::fromImage(nImage));
            ui->outputImage->setPixmap(QPixmap::fromImage(nImage));

            sendImage(nImage);
            ////maybe need to send image here ....?
        }
        else
        {
            // An error message is displayed if the file is not valid
            ui->inputImage->setText("File Invalid");
        }
    }
    else
    {
        ui->inputImage->setText("File Invalid");
    }
}

void MainWindow::on_brightnessSlider_valueChanged(int value)
{
    //convert slider value from between 0 and 200 to between 0 and 2
    brightness = (double)value/100.0;

    //normalize slider value to convert to range 00-99
    int normalizedSliderDisplayValue = round(((value*1.0)/200.0)*99);
    //convert the normalized brightness slider value (00-99) to a string
    brightnessSliderDisplayString = std::to_string(normalizedSliderDisplayValue);
    //concatenate leading zero if normalized brightness slider value is less than 10
    if (normalizedSliderDisplayValue < 10)
        brightnessSliderDisplayString = "0" + brightnessSliderDisplayString;

    applySliders(); ///for processing of images
}

void MainWindow::on_contrastSlider_valueChanged(int value)
{
    //adjust contrast
    //max is 511 min is 0
    contrast = value;
    //contrast = 511 - value;

    //normalize slider value to convert to range 00-99
    //int normalizedSliderDisplayValue = round(((value*1.0)/511.0)*99);
    int normalizedSliderDisplayValue = round(((value*1.0)/200.0)*99);
    //convert the normalized contrast slider value (00-99) to a string
    contrastSliderDisplayString = std::to_string(normalizedSliderDisplayValue);
    //concatenate leading zero if normalized contrast slider value is less than 10
    if (normalizedSliderDisplayValue < 10)
        contrastSliderDisplayString = "0" + contrastSliderDisplayString;

    applySliders(); ///for processing of images
}

void MainWindow::applySliders()
{
    //apply slider value brightness and contrast to image
    QImage inputImage = ui->inputImage->pixmap()->toImage();
    QImage outputImage = ui->outputImage->pixmap()->toImage();
    QRgb oldPix, newPix;
    uint tColor; //single color channel 0-255
    for (int pixelY = 0; pixelY < inputImage.height(); pixelY++)
    {
        for (int pixelX = 0; pixelX < inputImage.width(); pixelX++)
        {
            //get the current pixel
            oldPix = inputImage.pixel(pixelX,pixelY);
            tColor = qRed(oldPix); // Since each pixel is gray just pick a random color component like red
            //apply transformation formula with brightness and contrast to the pixel
            tColor = floor(brightness*(tColor+contrast));
            if (tColor > 255) //check that the pixel value is between 0 and 255
            {
                tColor = 255;
            }
            else if (tColor < 0)
            {
                tColor = 0;
            }
            newPix = qRgb(tColor,tColor,tColor);
            // Assign new pixel value to the output image
            outputImage.setPixel(pixelX, pixelY, newPix);
        }
    }
    // Display final image as output
    ui->outputImage->setPixmap(QPixmap::fromImage(outputImage));
}

void MainWindow::on_brightnessSlider_sliderReleased()
{
    // Upon completion of slider movement and after slider has been released
    // the reulting slider value is obtained and then converted to a range
    // 00-99 (from a range of 0-200) to send to the board. The values are
    // then converted to a string. If the resulting value is less than 10 a 0
    // is concatenated to have two digits. The updated paramaters are then sent
    // the the VEEK board via the sendParameters function
    int value = ui -> brightnessSlider -> sliderPosition();
    int sliderDisplayValue = round(value * 1.0 / 200.0 * 99);
    brightnessSliderDisplayString = std::to_string(sliderDisplayValue);
    if (sliderDisplayValue < 10 )
        brightnessSliderDisplayString = "0" + brightnessSliderDisplayString;

    sendParameters();

    qInfo() << "Brightness value: " << QString::fromStdString(brightnessSliderDisplayString) <<
        "Contrast value" << QString::fromStdString(contrastSliderDisplayString) << '\n';
}


void MainWindow::on_contrastSlider_sliderReleased()
{
    // Upon completion of slider movement and after slider has been released
    // the reulting slider value is obtained and then converted to a range
    // 00-99 (from a range of 511-0) to send to the board. The values are
    // then converted to a string. If the resulting value is less than 10 a 0
    // is concatenated to have two digits. The updated paramaters are then sent
    // the the VEEK board via the sendParameters function
    int value = ui -> contrastSlider -> sliderPosition();
    //int sliderDisplayValue = round(value * 1.0 / 511.0 * 99);
    int sliderDisplayValue = round(((value*1.0)/200.0)*99);
    contrastSliderDisplayString = std::to_string(sliderDisplayValue);
    if (sliderDisplayValue < 10 )
        contrastSliderDisplayString = "0" + contrastSliderDisplayString;

    sendParameters();

    qInfo() << "Brightness value: " << QString::fromStdString(brightnessSliderDisplayString) <<
        "Contrast value" << QString::fromStdString(contrastSliderDisplayString) << '\n';
}

void MainWindow::sendParameters() {
    // Function is used to send the brightness and contrast values to the board
    // A string for the paramater data string is created begininning with a "_PARAM_"
    // flag to be used by the server that receives the data, thn the subsequent brightness
    // and contrast string values already converted to the 2 digit range 00-99 are concatenated,
    // respectfully, finishing with and "_END_" flag to signal to the receiving server that it
    // is the end of the parameter data stream. The paramater data string is then sent.
    QString dataStringFormat = "_PARAM_\n";
    dataStringFormat.append(QString::fromStdString(brightnessSliderDisplayString + '\n'));
    dataStringFormat.append(QString::fromStdString(contrastSliderDisplayString + '\n'));
    dataStringFormat.append("_END_");
    TCPSOCKET -> write(dataStringFormat.toUtf8());
    qInfo() << "Parameter Data Sending Complete";
}

void MainWindow::sendImage(QImage sentImage) {
    // Function used to send the image width, height, and pixel data to the VEEK board using TCP.
    // The dimensions of the image are resized for sending in grayscale byte format. The image data
    // string is constructed starting with the "_IMAGE_" flag to let the server that the data stream
    // consists of the image data. The width and height are then appended to the image string with
    // a new line character seperating each string. Next the 2-dimensionsal for loop appends data
    // of each pixel to the image data string and seperated with a semicolon. The end of the image
    // data string is signaled by the "_END_" flag
    const static int imgWidth = 400;
    const static int imgHeight = 400;
    QImage resizedImage(imgWidth, imgHeight, QImage::Format_Grayscale8);
    resizedImage = sentImage.scaled(imgWidth, imgHeight);
    QString imageDataString = "_IMAGE_\n";
    imageDataString.append(QString::fromStdString(std::to_string(imgWidth) + '\n'));
    imageDataString.append(QString::fromStdString(std::to_string(imgHeight) + '\n'));

    // Append data of each pixel to data string and seperate with a semicolon
    for (int height = 0; height < imgHeight; height++) {
        for (int width = 0; width < imgWidth; width++) {
            // Get RGBA for each pixel
            QRgb pixel = resizedImage.pixel(width, height);
            int pixelRED = qRed(pixel);
            int pixelGREEN = qGreen(pixel);
            int pixelBLUE = qBlue(pixel);
            int pixelALPHA = qAlpha(pixel);
            // Place the pixel data into the image data string
            imageDataString.append(QString::number(pixelRED) + ' ');
            imageDataString.append(QString::number(pixelGREEN) + ' ');
            imageDataString.append(QString::number(pixelBLUE) + ' ');
            imageDataString.append(QString::number(pixelALPHA) + ' ');
        }
        imageDataString.append(";");
    }

    imageDataString.chop(1);
    imageDataString.append('\n');
    imageDataString.append("_END_");

    // Now that we have the complete image data string prepared, since
    // we are using TCP, we are sending the image data in streams of 1024 byte
    // blocks. So, this image data becomes really cool. Using a range based for-loop,
    // we iterate through each block of image data and send it using TCP.
    // Image data is considered sent when the "Data transmitted" message appears in
    // the terminal

    QString coolData = "";

    for (auto byteIndex = imageDataString.begin(); byteIndex != imageDataString.end(); byteIndex++)
    {
        // Iterate through each (byte) in the data being sent
        // and append the next character where "byte" acts as a counter
        coolData += *byteIndex;

        // Send the data in blocks of 1024 bytes
        if ((coolData.size() == 1024) || (byteIndex+1 == imageDataString.end()))
        {
            TCPSOCKET -> write(coolData.toUtf8());
            coolData = "";
        }
    }
    qInfo() << "Data transmitted\n";
}
