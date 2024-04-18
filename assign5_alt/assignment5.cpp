/*This program performs real-time image processing by capturing frames from a camera, 
applying brightness and contrast adjustments to BOTH the camera feed and an overlaid image, 
and displaying the processed output. 
It uses OpenCV functions for image manipulation and GUI display.
Uses LUT
*/

#include <opencv2/opencv.hpp>  // Include OpenCV library
#include <sys/time.h>          // Include sys/time.h for time-related functions
#include "D8MCapture.h"        // Include D8MCapture header
#include "hps_0.h"             // Include hps_0 header
#include "string"              // Include string header
#include "cmath"               // Include cmath header
#include "fstream"             // Include fstream header for file operations

#include <chrono>              // For time related functions
#include <iostream>            // used to read parameter values from a file

using namespace cv;            // Using OpenCV namespace
using namespace std;           // Using standard namespace

#ifndef CAPTURE_RAM_DEVICE
#define CAPTURE_RAM_DEVICE "/dev/f2h-dma-memory"    // Default RAM device if not defined
#endif /* ifndef CAPTURE_RAM_DEVICE */

int main()
{  
    Mat src;                    // Unmodified Camera Feed
    D8MCapture *cap = new D8MCapture(TV_DECODER_TERASIC_STREAM_CAPTURE_BASE, CAPTURE_RAM_DEVICE);
    // Check if the capture object is successfully opened
    if (!cap->isOpened()) {
        return -1;              // Return -1 if capture object failed to open
    }

    // Declare variables for FPS calculation using built in OpenCV funtion
    double fps = 0.0;
    int64 prevTick = 0;

    while (1) {
        // Read a frame from the camera
        if (!cap->read(src))
            return -1;

        // Read overlay image from the saved file "assignment5_image.png"
        // that the python server obtained from the QT client
        Mat QTImage = imread("assignment5_image.png");
        if (QTImage.empty()) {
            cout << "Failed to load the overlay image." << endl;
            return -1;
        }

        // Declare fstream variable to parse parameter data
        fstream parametersFromFile;

        // Declare variables to store brightness and contrast parameters
        int brightness;
        int contrast;

        // Paramater brightness and contrast string data is 
        // parsed from the saved paramater file that the python
        // server obtained from the QT client and is converted
        // back to integers
        parametersFromFile.open("assignment5_parameters.txt");
        string line;

        // Read brightness value from the file and convert to integer
        getline(parametersFromFile, line);
        brightness = stoi(line);
        // Read contrast value from the file and convert to integer
        getline(parametersFromFile, line);
        contrast = stoi(line);
        // Close the parameter file
        parametersFromFile.close();

        // Create a clone of the overlaid image for adjustments
        Mat adjustedQTImage = QTImage.clone();

        // Convert contrast and brightness values back to original range (00-99)
        // Calculate necessary variables for overlay and update brightness and contrast
        double brightnessAdjust = (brightness - 50) / 50.0;
        double contrastAdjust = contrast / 255.0;

        // Create a lookup table for adjusting pixel values
        Mat QTImagePixelLookUp(1, 256, CV_8UC1);

        // Adjust brightness and contrast of the overlaid image
        for (int i = 0; i < 256; i++) {
            int color = static_cast<int>((i + (brightnessAdjust * 128)) * contrastAdjust);
            color = saturate_cast<uchar>(color);
            QTImagePixelLookUp.at<uchar>(i) = color;
        }

        // Apply the lookup table to the overlaid image
        LUT(adjustedQTImage, QTImagePixelLookUp, adjustedQTImage);

        // Create a clone of the camera feed for adjustments
        Mat outputImage = src.clone();

        // Variables for brightness and contrast adjustment of the camera feed
        double cameraBrightnessAdjust = (brightness - 50) / 50.0;
        double cameraContrastAdjust = contrast / 255.0;

        // Create a lookup table for adjusting pixel values of the camera feed
        Mat cameraPixelLookUp(1, 256, CV_8UC1);

        // Adjust brightness and contrast of the camera feed
        for (int i = 0; i < 256; i++) {
            int color = static_cast<int>((i + (cameraBrightnessAdjust * 128)) * cameraContrastAdjust);
            color = saturate_cast<uchar>(color);
            cameraPixelLookUp.at<uchar>(i) = color;
        }

        // Apply the lookup table to the camera feed
        LUT(outputImage, cameraPixelLookUp, outputImage);       

        // Resize the adjusted overlaid image
        Mat resize1, resize2;
        resize(adjustedQTImage, resize1, Size(src.cols / 2, src.rows / 2), INTER_LINEAR);
        cvtColor(resize1, resize2, COLOR_BGR2BGRA);

        // Calculate the region of interest (ROI) for overlaying
        int qtrWidth = outputImage.cols / 2;
        int qtrHeight = outputImage.rows / 2;
        Rect roi(0, 0, qtrWidth, qtrHeight);
        resize(resize2, resize2, Size(qtrWidth, qtrHeight), INTER_LINEAR);

        // Overlay the adjusted image onto the camera feed
        Mat overlayedRoi = outputImage(roi);
        resize2(Rect(0, 0, qtrWidth, qtrHeight)).copyTo(overlayedRoi);
        addWeighted(overlayedRoi, 1.0, resize2, 0.5, 0.0, overlayedRoi);

        // Calculate FPS
        double tickFrequency = getTickFrequency();
        int64 currentTick = getTickCount();
        double timeDiff = (currentTick - prevTick) / tickFrequency;
        fps = 1.0 / timeDiff;
        prevTick = currentTick;

        // Display FPS values on the output image (top right corner)
        std::string fpsString = "FPS: " + std::to_string(fps);
        fpsString = fpsString.substr(0, fpsString.find(".") + 3);
        putText(outputImage, fpsString, Point(outputImage.cols - 150, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);

        // Display brightness, contrast values, and on the output image
        string brightnessText = "Brightness: " + to_string(brightness);
        string contrastText = "Contrast: " + to_string(contrast);
        putText(outputImage, brightnessText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        putText(outputImage, contrastText, Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
 
        // Display the input and output images
        imshow("Both Live Video and Overlayed Images Change", outputImage);

        // Exit if 'Esc' key is pressed
        int c = waitKey(10);
        if ((char)c == 27) {
            break;
        }
    }

    // Release capture object and close windows
    delete cap;
    destroyAllWindows();

    return 0;  // Return 0 to indicate successful execution
}