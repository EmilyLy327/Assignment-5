Useful references:
https://docs.opencv.org/3.4/d3/d63/classcv_1_1Mat.html

How to set-up:

1. Place the provided assignment5.py (navigate to /root/opencv/camera_in) with putty and paste the assignment5.py code there and be sure the file name is "assignment5.py" (using VIM). Actually could name the python server whatever you want but good to be consistent. It must be in this directory though since it retrieves the data that the camera needs. 

2. Using VIM edit the camera_in.cpp file with the provided assignment5.cpp file and ensure it has this name.

3. The provided Makefile has the required modifications to the existing Makefile. Only a few lines need to be changed. Make sure the provided Makefile is updated in this directory.

4. Create a Qt project like you normally do maybe call it assignment5 or whatever.., close your project, delete the build file, delete the *.cpp files, delete the .h file, and delete the mainwindow.ui. Copy and paste the provided *.cpp, .h, and .ui files into 
your Qt project. Don't forget --> add the "QT 	+=   network" line to your .pro file 
(will make it the second line in the file) since the Qt application will use the network features.  

How to run:
(Note: Make sure mouse and keyboard, ethernet connected, and RS-232 cable to pc for putty 
is connected)
1. Open putty, Give power to the VEEK board, Startup as usual --> ifconfig to obtain IP address --> then update server and client with the IP address.

2. Run the python server --> cd to ~/opencv/camera_in/ --> python assignment5.py 
(of course could also run by providing relative path)

3. Run the Qt client --> select the image --> move brightness and contrast sliders slightly. Wait for message to appear in Qt terminal stating that the image has been saved to the board. (If running "make" and no image is on the board, the build of "assignment5" executable can't be completed, thats why have to wait for image to be saved on board before next step)

4. On the VEEKS board, (using keyboard and mouse), click the bottom left most corner icon --> system Tools --> "LXTerminal", cd to /root/opencv/camera_in, run "make" if any changes were made, run the resulting executable for the camera --> ./assignment5 (I like to "make clean" and then "make" each time but not necessary)  

5. When the brightness and contrast are varied on the Qt client, you should see variations in the camera video feed. The overlay image in upper left corner is part of the build and is static (thats why it needed to be on the board before running make). 

Shutting down:

Don't think the order matters as long as not trying to shut down while data being sent
(image data only send when you select an image) or moving the sliders

1. In LXterminal on board the VEEK, control C
2. Python server, control C
3. Stop client 
