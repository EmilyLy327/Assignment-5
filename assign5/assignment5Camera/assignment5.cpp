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
    static struct timeval last_time;   // Static struct for storing last time
    struct timeval current_time;       // Struct for storing current time
    static float last_fps;             // Static variable for storing last frames per second
    float t;                           // Variable for storing time
    float fps;                         // Variable for storing frames per second

	// Window to display the camera feed and overlay image
    namedWindow("ECE484W_Assignment_5");

    while (1) {
        // Read frame from the camera feed
        if (!cap->read(src))
            return -1;

        // Read overlay image from the saved file "assignment5_image.png"
        // that the python server obtained from the QT client
        Mat overlayedImage = imread("assignment5_image.png");

        // Declare variables to store brightness and contrast parameters
        int brightness;
        int contrast;

        // Paramater brightness and contrast string data is 
        // parsed from the saved paramater file that the python
        // server obtained from the QT client and is converted
        // back to integers
        
        // Declare fstream variable to parse paramater data
        fstream parametersFromFile;
         // Open parameter file to read brightness and contrast values
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

        // Convert contrast and brightness values back to original range (00-99)
        // Calculate necessary variables for overlay and update brightness and contrast
        int minimumValue = 0;
        int maximumValue = 511 - 511.0 / 99.0 * contrast; 
        int brightnessMultiply = 2.0 / 99.0 * brightness;
        int color; 

        // Create necessary Mats for overlay and brightness/contrast adjustment
        Mat resize1;
        Mat resize2;
        Mat outputImage = src.clone();
        
        // Resize and convert overlay image to match camera feed formatThen create a look up table for the pixel values.
        resize(overlayedImage, resize1, Size(800, 480), INTER_LINEAR);
        cvtColor(resize1, resize2, COLOR_BGR2BGRA);
        Mat pixelLookUp(1, 256, outputImage.type()); 

        // Apply brightness and contrast adjustment to each pixel using LUT
        for (int i = 0; i < 256; i++) {
            color = floor(brightnessMultiply * 255 * (i - minimumValue) / (maximumValue - minimumValue));
            if (color > 255) 
                color = 255;
            
            // Update pixel lookup table for RGB channels
            pixelLookUp.at<Vec3b>(i)[0] = color; // R
            pixelLookUp.at<Vec3b>(i)[1] = color; // G
            pixelLookUp.at<Vec3b>(i)[2] = color; // B
        }

        // Apply the lookup table to adjust brightness and contrast of the output image
        LUT(outputImage, pixelLookUp, outputImage);

        // Calculate region of interest (ROI) and overlay the resized image onto it
        // Overlayed image to cover 25% of output (ROI) in the upper left of output
        int qtrWidth = outputImage.cols/2 ;
        int qtrHeight = outputImage.rows/2;
        Rect roi(0, 0, qtrWidth, qtrHeight);
		resize(resize2, resize2, Size(qtrWidth, qtrHeight), INTER_LINEAR);
        Mat overlayedRoi = outputImage(roi);
        resize2(Rect(0, 0, qtrWidth, qtrHeight)).copyTo(overlayedRoi);
		addWeighted(overlayedRoi, 1.0, resize2, 0.5, 0.0, overlayedRoi);
        
        // Display the modified camera feed and overlay image
        imshow("Live Camera Input", src);
        imshow("Live Output Image", outputImage);

        // Bail out if escape was pressed
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