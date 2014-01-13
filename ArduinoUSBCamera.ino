// Copyright (C) 2014  Stefano Guglielmetti

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <Process.h>

const int buttonPin = 2; // pin for the pushbutton 
const int ledPin =  13; // pin for the status led

String path = "/mnt/sda1/"; // path of the images and script file
String filename;
int buttonState = 0;

Process p;

void setup() {
  Bridge.begin(); // The bridge library is used to run processes on the linux side
  pinMode(ledPin, OUTPUT);      
  pinMode(buttonPin, INPUT); 
  Serial.begin(57600); // Serial port for debugging
  filename = "test";
}

void loop(){
  buttonState = digitalRead(buttonPin);
  
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
    
    Serial.println(filename); // I send the generated filename to the Serial port for debugging
    
    digitalWrite(ledPin, HIGH); // The led indicates that I'm taking the picture 
    
    p.runShellCommand("fswebcam " + path + filename); //let's take the picture
    while(p.running());  //waits till the picture is saved
    Serial.println("picture taken"); 
    
    p.runShellCommand("python " + path + "sendphoto.py " + path + filename); //sends the picture to facebook 
    while(p.running()); 
    Serial.println("picture sent"); //last debug message
  } 
  else {
    digitalWrite(ledPin, LOW); 
  }
}




