# Arduino social photo camera

After making an USB Audio card work with Arduino Yún, the logical second step was try to use an USB camera instead, and it worked! I was so amazed by this new huge set of possibilitities to create, that I started a little weekend project: to build an Arduino based Facebook photo camera

I was so into this project that I didn't realize that, even if the single steps are very simple, they're quite a lot! So I decided to write this step-by-step tutorial and to split it into ... articles.

Just to be clear: 

**The goal**

*To build photocamera that shoots pictures and publishes them directly to a specific Facebook Album.*

## Ingredients

 - Arduino Yún
 - USB camera (this guide is for UVC compatible cameras)
 - Pushbutton circuit (http://arduino.cc/en/tutorial/button)
 - SD Card
 - Facebook developers account (https://developers.facebook.com/)
 - Temboo account (https://temboo.com/)
 - Working, brand new Facebook Application (we'll create one during the tutorial)
 - [This Arduino Sketch](https://github.com/amicojeko/ArduinoUSBCamera/blob/master/ArduinoUSBCamera.ino)
 - [This Python script](https://github.com/amicojeko/ArduinoUSBCamera/blob/master/sendphoto.py)

## The Steps

1. Install the USB Camera on Arduino Yún
2. Take your first picture
3. Create the Facebook Application
4. Post your first picture with the Temboo API
5. Install the Temboo SDK on Arduino Yún
6. Make the python script and publish your first photo to Facebook from Arduino
7. Create the Arduino sketch
8. Put everything together

### 1. Install the USB Camera on Arduino Yún

I assume that your Arduino Yún is already configured for wi-fi (or connected via the Ethernet port), and that you have SSH access to it, if not, follow [this link](http://arduino.cc/en/Guide/ArduinoYun#toc13) to configure the wifi network and [this link](http://arduino.cc/en/Tutorial/LinuxCLI) to connect via SSH.

The UVC package is already available fot Linino via opkg, so installing the Camera is a very easy step, just connect to your Yún via ssh and type

	$ opkg update
	$ opkg install kmod-video-uvc

We also need a software for taking pictures, I used fswebcam that is supersmall and very easy to use

	$ opkg install fswebcam

Now we can test it! 

### 2. Take your first picture

First of all, be sure to use and SD card for storage or you will quickly fill the Arduino internal storage, and you don't want to do that. If you are using an SD card, you should see it mounted in /mnt/sda1. If it's not, put a FAT32 formatted micro SD card into the SD Card Slot, and reboot your Arduino

Now plug the camera to the USB host port, and type

	$ cd /mnt/sda1
	$ fswebcam test.png

If everything is ok, you took your first picture! 

This means that now we can take pictures from our Arduino sketch, via the Yún's Bridge library, in this way

	Process.runShellCommand("fswebcam /mnt/sda1/test.png");

Now we will learn how to share them on Facebook using Temboo.

### 3. Create the Facebook Application

This is easy, but you must be a Facebook developer to do that. That simply means that you have to accomplish [these easy steps](https://developers.facebook.com/docs/create-developer-account/) before starting. Done? ok! So now let's create a blank Facebook Application

![image creation](http://i.imgur.com/z3SG4sq.png)

and configure it in this way:

![configuration](http://i.imgur.com/QCTJR6T.png)

### Create a Temboo Application

First of all, register on Temboo. When you're in, create a new application, then go to [https://temboo.com/library/Library/Facebook/Publishing/UploadPhoto/](https://temboo.com/library/Library/Facebook/Publishing/UploadPhoto/)

add new credentials for the new Facebook app.

![temboo config](http://i.imgur.com/mxrJtlU.png)

Choose oAuth wizard, go directly to the third step and copy and paste the app id & app secret from your Facebook application.

Go to the next step, click on launch authorisation and grant the authorisation to the Facebook application.

Now you have the access token

Click on use these values and save, copy and paste the access token in the access token form field if needed.

Now you need the target album id. to get an album id, just go on Facebook, open an album (not a single photo) and the URL will look like this.

![albumid](http://imgur.com/RElG4Zg.png)

The album id is the highlighted part. copy and paste it into the AlbumID field on Temboo

I also choose to open the Optional Input section and to put a custom message :)

![message](http://i.imgur.com/HKNU5sX.png)

If you take a look to the sample code, Temboo has made pretty much all the job for you, but we need to make some changes, we will make them later. Now we need some gratification, so let's post our first picture!


### 4. Post your first picture with the Temboo API

You already put your Access Token and the desired AlbumID. The only thing that's missing is the Base64 Encoded image. So go to [http://www.freeformatter.com/base64-encoder.html](http://www.freeformatter.com/base64-encoder.html) and paste the URI of the image you want to encode, then copy the encoded string and paste it into the Temboo interface.

![http://i.imgur.com/DT1Ij8U.png](http://i.imgur.com/DT1Ij8U.png)

Click TRY IT OUT and voilà, the image it's magically posted on your album!

### 5. Install the Temboo SDK on Arduino Yún

We will use the Temboo API from a Python script, so we need to download the SDK. The download page is here [https://temboo.com/download](https://temboo.com/download), all we have to do is download the Python SDK and unzip it into the root folder of your Micro SD card (it will unzip into a /temboo folder).

A very important step is installing OpenSSL support for Python, as the Temboo libraries will try to use SSL to publish the photos.

	$ opkg update
	$ opkg install python-openssl
	
	
### 6. Make the python script and publish your first photo to Facebook from Arduino	
	

As I wrote before, the generated code from Temboo is a good starting point, but we need to make some modifications in order to make it work.

	# Instantiate the Choreo, using a previously instantiated TembooSession object, eg:
	# session = TembooSession('jeko', 'APP_KEY_NAME', 'APP_KEY_VALUE')
	uploadPhotoChoreo = UploadPhoto(session)

	# Get an InputSet object for the choreo
	uploadPhotoInputs = uploadPhotoChoreo.new_input_set()

	# Set credential to use for execution
	uploadPhotoInputs.set_credential('ArduinoCamera')

	# Set inputs
	uploadPhotoInputs.set_Message("This photo has been shot and published with an Arduino Yún")
	uploadPhotoInputs.set_AlbumID("")

	# Execute choreo
	uploadPhotoResults = uploadPhotoChoreo.execute_with_results(uploadPhotoInputs)

Download [my version of the script](https://github.com/amicojeko/ArduinoUSBCamera/blob/master/sendphoto.py) and copy it into the root folder of the SD card. Here's what I did.

Step number one, the encoding. I'm italian and we use a lot of accented characters, so I'll use UTF-8 as the default script encoding, so I added

	# coding=utf-8
	
at the top of my script. Now we need to read the photo from the disk, and to base64 encode it, so we will need the base64 and the sys libraries.

	import base64
	import sys
	
The Temboo libraries are not included in the generated script by Temboo, so we will have to add them

	from temboo.core.session import TembooSession
	from temboo.Library.Facebook.Publishing import UploadPhoto
	
The script will read the first argument, and use it as the name of the image file to encode

	with open(str(sys.argv[1]), "rb") as image_file:
	    encoded_string = base64.b64encode(image_file.read())

In this line you have to put your Temboo's account name, and the app name and key (from [this page](https://temboo.com/account/applications/))

	session = TembooSession('ACCOUNT_NAME', 'APP_KEY_NAME', 'APP_KEY_VALUE')
	
You'll also need to put yout access token, message (if you want one) and Album ID into the script 

	uploadPhotoInputs.set_AccessToken("ACCESS_TOKEN")
	uploadPhotoInputs.set_Message("MESSAGE")
	uploadPhotoInputs.set_AlbumID("ALBUM_ID")	
	
The rest of the script is basically identical to the Temboo's example.

We can now try to publish our first photo with the script! 

	$fswebcam testpic.jpg
	$python sendphoto.py testpic.jpg 
	
At this point I strongly hope that it's working for you as it's working for me (otherwise contact me and we will try to solve the problem together) because you have just published your first picture to Facebook with Arduino! The only missing step is to make an Arduino sketch to take and send the picture without using SSH

### 7. Create the Arduino sketch

I started from the basic "Button" example, as I wanted the camera to be triggered by a simple Pushbutton. The circuit is the same of the [button example](http://arduino.cc/en/tutorial/button). Just download [this arduino sketch](https://github.com/amicojeko/ArduinoUSBCamera/blob/master/ArduinoUSBCamera.ino and open it with the Arduino IDE)

you can customise the LED and Button pin if you want

	const int buttonPin = 2; // pin for the pushbutton 
	const int ledPin =  13; // pin for the status led


as well as the path for the photos and the Python script (important! this folder must contain the Temboo SDK)

	String path = "/mnt/sda1/"; // path of the images and script file 
	
The setup is pretty simple, we will use the Bridge library to invoke the Python script, and the Serial library for a very basic debugging

	void setup() {
  		Bridge.begin(); // The bridge library is used to run processes on the linux side
  		pinMode(ledPin, OUTPUT);      
  		pinMode(buttonPin, INPUT); 
  		Serial.begin(57600); // Serial port for debugging
  		filename = "test";
	}
	
Then, when the pushbutton is pressed, the script generates a new timestamp based filename running the "date +%s"	command
	

  	if (buttonState == LOW) {
    	filename = "";
    	p.runShellCommand("date +%s"); //generate a timestamp
    	while(p.running()); // wait until the command has finished running
    	
    	while (p.available()>0) {
      		char c = p.read(); //reads the first available character
      		filename += c; //and adds it to the filename string
    	} 
    	
    	filename.trim(); //used to avoid trailing spaces or newline characters
    	filename += ".png"; //finally I add the png extension
    	
This part of the code will take the picture and send it to Facebook using the Bridge library
    	
	p.runShellCommand("fswebcam " + path + filename); //let's take the picture
    while(p.running());  //waits till the picture is saved
    Serial.println("picture taken"); 
    
    p.runShellCommand("python " + path + "sendphoto.py " + path + filename); //sends the picture to facebook 
    while(p.running()); 
    Serial.println("picture sent"); //last debug message    
    	

### 8. Put everything together

Ok! Everything is ready! Just upload the code to Arduino, push the button and... Smile!! You're on Facebook! :)

I really hope that you enjoyed this tutorial, it opens a lot of possibilities! Trap cameras, cameras triggered by doors, by sounds or any kind of sensor... I'd really like to see your projects and share them!

Cheers and be happy!


**Stefano**






- opkg install ffserver



VIDEO


opkg install v4l-utils




ffmpeg -v verbose -r 1 -s -f v4l2  -y -i /dev/video0 -sameq -vframes 1 test.jpg

ffmpeg -v verbose -r 1 -s 640x480 -f v4l2  -y -i /dev/video0 -sameq -vframes 1 test2.jpg

ffmpeg -v verbose -r 5 -s 320x240 -f v4l2 -i /dev/video0 -c:v flv http://localhost:8090/videofeed.ffm



install coreutils-base64

