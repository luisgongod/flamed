/*
Atom lite, working with buttons.
MPU6886 not working
sadly
*/
#include <Arduino.h>
// #include <M5StickC.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <NintendoExtensionCtrl.h>
#include "credentials.h"
// #include "M5Atom.h"

#define LOOPDELAY 50

WiFiUDP Udp;
IPAddress ip(192, 168, 2, 105);    
IPAddress gateway(192, 168, 2, 254);    
IPAddress subnet(255, 255, 255, 0);     

const IPAddress outIp(192,168,2,73); // remote IP (not needed for receive)
const unsigned int outPort = 9000;		// remote port (not needed for receive)
const unsigned int localPort = 8000;
OSCErrorCode error;

int red_button =0;
int blue_button =0;
int lite_button = 0;

void setup() {
	Serial.begin(115200);
	delay(100);
	Serial.println("Lite it up!");

  //Serial,i2c,display
  // M5.begin(true, false, false);
  // delay(10);
	
	WiFi.config(ip,gateway,subnet);
	WiFi.begin(ssid, pass);	
	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
	}

	Udp.begin(localPort);

  pinMode(26, INPUT);
  pinMode(32, INPUT);	
  pinMode(39, INPUT);	
}

float temp = 0;
void loop() {  

  red_button = digitalRead(26);
  blue_button = digitalRead(32);
  lite_button = digitalRead(39);

	OSCBundle bndl;	

  // M5.Btn.wasPressed()
  if (lite_button == 1){
    bndl.add("/lite").add(1);	    
  }
  else
  {
    bndl.add("/lite").add(0);	
  }

  if (red_button == 1){
    bndl.add("/red").add(1);	    
  }
  else
  {
    bndl.add("/red").add(0);	
  }

  if (red_button == 1){
    bndl.add("/red").add(1);	    
  }
  else
  {
    bndl.add("/red").add(0);	
  }
  
  if (blue_button == 1){
    bndl.add("/blue").add(1);	    
  }
  else
  {
    bndl.add("/blue").add(0);	
  }

	Udp.beginPacket(outIp, outPort);
	bndl.send(Udp); 
	Udp.endPacket(); 
	bndl.empty();  



	delay(LOOPDELAY);
}