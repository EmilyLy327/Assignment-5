#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include "D8MCapture.h"
#include "hps_0.h"
#include "string"
#include "cmath"
#include "fstream"

using namespace cv;
using namespace std;

#ifndef CAPTURE_RAM_DEVICE
#define CAPTURE_RAM_DEVICE "/dev/f2h-dma-memory"
#endif /* ifndef CAPTURE_RAM_DEVICE */

int main()
{
    Mat src; // Unmodified Camera Feed
    D8MCapture *cap = new D8MCapture(TV_DECODER_TERASIC_STREAM_CAPTURE_BASE, CAPTURE_RAM_DEVICE);
    if (!cap->isOpened()) {
        return -1;
    }

    char str[100];

	// Create windows for displaying the input and output images
    namedWindow("Live Camera Input");
    namedWindow("Live Output Image");

    while (1) {
        // Read a frame from the camera
        if (!cap->read(src))
            return -1;

        // Get the overlayed image from the saved image file
        // that the python server obtained from the QT client
        // output error if the image failed to load
        Mat QTImage = imread("assignment5_image.png");
        if (QTImage.empty()) {
            cout << "Failed to load the overlay image." << endl;
            return -1;
        }

        // Declare fstream variable to parse paramater data
        // and declare the brightness and contrast variables
        fstream parametersFromFile;
        int brightness;
        int contrast;

        // Paramater brightness and contrast string data is 
        // parsed from the saved paramater file that the python
        // server obtained from the QT client and is converted
        // back to integers
        parametersFromFile.open("assignment5_parameters.txt");
        string line;
        getline(parametersFromFile, line);
        brightness = stoi(line);
        getline(parametersFromFile, line);
        contrast = stoi(line);
        parametersFromFile.close();

        // Create a clone of the overlaid image for adjustments
        Mat adjustedQTImage = QTImage.clone();

        // Convert the contrast and brightness back to original values
        // (Was in the range 00-99). Create the necessary variables to be used to overlay
        // and update brightness and contrast. Then overlay onto the incoming camera
        // feed, and change the overlayed image to be the same frame format
        // as the video stream. Then create a look up table for the pixel values.
        // Apply brightness and contrast to the RGB channels of each pixel. Then
        // update the look up table for the output to apply the brightness and contrast.
        int minimumValue = 0;
        int maximumValue = 511 - 511.0 / 99.0 * contrast; 
        int brightnessMultiply = 2.0 / 99.0 * brightness;
        int color; 

        // Create a lookup table for adjusting pixel values
        Mat QTImagePixelLookUp(1, 256, adjustedQTImage.type());

        // Adjust brightness and contrast of the overlaid image
        for (int i = 0; i < 256; i++) {
            color = floor(brightnessMultiply * 255 * (i - minimumValue) / (maximumValue - minimumValue));
            if (color > 255)
                color = 255;

            QTImagePixelLookUp.at<Vec3b>(i)[0] = color; // R
            QTImagePixelLookUp.at<Vec3b>(i)[1] = color; // G
            QTImagePixelLookUp.at<Vec3b>(i)[2] = color; // B
        }
        LUT(adjustedQTImage, QTImagePixelLookUp, adjustedQTImage);

        // Create a clone of the camera feed for adjustments
        Mat outputImage = src.clone();

        // Variables for brightness and contrast adjustment of the camera feed
        int cameraMinimumValue = 0;
        int cameraMaximumValue = 511 - 511.0 / 99.0 * contrast;
        int cameraBrightnessMultiply = 2.0 / 99.0 * brightness;

        // Create a lookup table for adjusting pixel values of the camera feed
        Mat cameraPixelLookUp(1, 256, outputImage.type());

        // Adjust brightness and contrast of the camera feed
        for (int i = 0; i < 256; i++) {
            color = floor(cameraBrightnessMultiply * 255 * (i - cameraMinimumValue) / (cameraMaximumValue - cameraMinimumValue));
            if (color > 255)
                color = 255;

            cameraPixelLookUp.at<Vec3b>(i)[0] = color; // R
            cameraPixelLookUp.at<Vec3b>(i)[1] = color; // G
            cameraPixelLookUp.at<Vec3b>(i)[2] = color; // B
        }
        LUT(outputImage, cameraPixelLookUp, outputImage);


        // Resize the adjusted overlaid image
        Mat resize1, resize2;
        resize(adjustedQTImage, resize1, Size(src.cols / 2, src.rows / 2), INTER_LINEAR);
        cvtColor(resize1, resize2, COLOR_BGR2BGRA);

        // Calculate the region of interest (ROI) for overlaying
        int qtrWidth = src.cols / 2;
        int qtrHeight = src.rows / 2;
        Rect roi(0, 0, qtrWidth, qtrHeight);

        // Overlay the adjusted image onto the camera feed
        Mat overlayedRoi = src(roi);
        resize2.copyTo(overlayedRoi);
        addWeighted(overlayedRoi, 1.0, resize2, 0.5, 0.0, overlayedRoi);

        // Display the input and output images
        imshow("Live Camera Input", src);
        imshow("Live Output Image", outputImage);

        // Display brightness and contrast values on the output image
        char brightnessContrastStr[50];
        sprintf(brightnessContrastStr, "Brightness: %d, Contrast: %d", brightness, contrast);
        putText(src, brightnessContrastStr, Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);

        // Exit if 'Esc' key is pressed
        int c = waitKey(10);
        if ((char)c == 27) {
            break;
        }
    }

    // Clean up
    delete cap;
    destroyAllWindows();

    return 0;
}