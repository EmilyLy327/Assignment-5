#include <opencv2/opencv.hpp>  // Include OpenCV library
#include <sys/time.h>          // Include sys/time.h for time-related functions
#include "D8MCapture.h"        // Include D8MCapture header
#include "hps_0.h"             // Include hps_0 header
#include "string"              // Include string header
#include "cmath"               // Include cmath header
#include "fstream"             // Include fstream header for file operations

using namespace cv;            // Using OpenCV namespace
using namespace std;           // Using standard namespace

#ifndef CAPTURE_RAM_DEVICE
#define CAPTURE_RAM_DEVICE "/dev/f2h-dma-memory"    // Default RAM device if not defined
#endif /* ifndef CAPTURE_RAM_DEVICE */

int main()
{
    Mat src; // Unmodified Camera Feed
    D8MCapture *cap = new D8MCapture(TV_DECODER_TERASIC_STREAM_CAPTURE_BASE, CAPTURE_RAM_DEVICE);
    // Check if the capture object is successfully opened
    if (!cap->isOpened()) {
        return -1;              // Return -1 if capture object failed to open
    }

    char str[100];                     // Character array for string manipulation

	// Create windows for displaying the input and output images
    namedWindow("Live Camera Input");
    namedWindow("Live Output Image");

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

        // Display brightness and contrast values on the output image
        string brightnessText = "Brightness: " + to_string(brightness);
        string contrastText = "Contrast: " + to_string(contrast);
        putText(outputImage, brightnessText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        putText(outputImage, contrastText, Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        
        // Display the input and output images
        imshow("Live Camera Input", src);
        imshow("Live Output Image", outputImage);

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