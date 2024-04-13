import os
import socket
import math

from PIL import Image

# Defining and verifying socket communications paramaters for use with TCP 
hostName = socket.gethostname()
IP = socket.gethostbyname(hostName)
print ("HOSTNAME: " + str(hostName))
print ("IPADDR: " + str(IP))

TCPPORT = 80
TCPIP = "192.168.1.109"    # Make sure to update this 
PACKETSIZE = 1024

# Function is used to parse the inputed image data and reconstruct the 
# image. Each row of the image is delimited with a semicolon and each 
# RGB pixel is delimited by spaces. Then group each RGB into one pixel 
# in instantiate the image object 
def parseRGBAImage(image, width, height):
    parsImage = Image.new("RGBA", (width, height))
    
    imageLines = image.split(';') 
    
    for imgRow in range(height): 
        imgRGBA = imageLines[imgRow].split(' ') 
        for imgColumn in range(width):
            
            pixRED = int(imgRGBA.pop(0))
            pixGREEN = int(imgRGBA.pop(0))
            pixBLUE = int(imgRGBA.pop(0))
            pixALPHA = int(imgRGBA.pop(0))
            parsImage.putpixel((imgColumn, imgRow), (pixRED, pixGREEN, pixBLUE, pixALPHA))
            
    return parsImage

# Setup for communications using TCP
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((TCPIP, TCPPORT))
print ("LISTENING TO " + str(TCPIP) + ". . .")
sock.listen(5)
conn, addr = sock.accept()
print ("CONNECTED TO " + str(conn) + " BY " + str(addr))
print ("READY TO SERVE . . .")

# Server enters an infinite loop to serve. The server will receive string data
# in 1024 byte blocks. Data will either have a _IMAGE_ or _PARAM_ flag to indicate 
# if the data is the image itself or the parameters that define the image. If the 
# "image" type, then the image type (Origianl or Overlayed) is determined based on
# "OR" or "OV" flags. Then the image width and image height is received. Then the image
# itself is recieved. If the "parameter" type is received, than the brightness and contrast 
# string values are received in the form 00-99 for each respectively. The end of each type
# of data is signaled by the _END_. Receieve, parse, and store the applicable data types
# accordingly.
while True:
    print ("STANDING BY . . .")
    
    dataStringFormat = ""
    paramSend = False
    imgSend = False
    # As long as received data string is neither the _PARAM_ or _IMAGE_ flag continue to
    # receive data, otherwise set the appropriate flag to True to exit this inner loop
    while not (imgSend or paramSend):
        data = conn.recv(PACKETSIZE) 
        dataStringFormat += data
        
        if dataStringFormat.startswith(b'_IMAGE_'):
            imgSend = True
            print ("IMAGE BEING RECIEVED . . .")
        elif dataStringFormat.startswith(b'_PARAM_'):
            paramSend = True
            print ("PARAMETERS ARE BEING RECIEVED . . .")
        else:
            dataStringFormat = ""
    # Continue to receive data until the _END_ flag is encountered        
    while not dataStringFormat.endswith(b'_END_'): 
        data = conn.recv(PACKETSIZE) 
        dataStringFormat += data
    
    # Now that data has been received parse the input data and store in the appropriate
    # variables and save as files to be used by the camera assignment5.cpp program
    # A .png file is saved for the image and a .txt file is saved for the parameters
    print ("THE DATA HAS BEEN RECEIVED . . .")
    dataVector = dataStringFormat.split('\n')
    
    if imgSend:
        imgWidth = int(dataVector[1])
        imgHeight = int(dataVector[2])
        imgString = dataVector[3]
        sentImage = parseRGBAImage(imgString, imgWidth, imgHeight)
        sentImage.save("assignment5_image.png")
        
        print ("THE IMAGE HAS BEEN SAVED TO THE BOARD")
        
    if paramSend:
        brightness = int(dataVector[1])
        contrast = int(dataVector[2])
        paramFile = open("assignment5_parameters.txt", "w")
        paramFile.write(str(brightness))
        paramFile.write('\n')
        paramFile.write(str(contrast))
        paramFile.close()