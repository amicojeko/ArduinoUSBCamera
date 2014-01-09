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
    p.runShellCommand("date +%s");
    while(p.running()); 
    
    while (p.available()>0) {
      char c = p.read();
      filename += c;
    } 
    
    filename.trim();
    filename += ".png";
    
    Serial.println(filename);
    
    digitalWrite(ledPin, HIGH);  
    
    p.runShellCommand("fswebcam " + path + filename);
    while(p.running());  
    Serial.println("picture taken");
    
    p.runShellCommand("python " + path + "sendphoto.py " + path + filename);
    while(p.running());  
    Serial.println("picture sent");
  } 
  else {
    digitalWrite(ledPin, LOW); 
  }
}




