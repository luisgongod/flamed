#include <Arduino.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <NintendoExtensionCtrl.h>
#include "credentials.h"

#define LOOPDELAY 50

WiFiUDP Udp;
const IPAddress outIp(192,168,2,73); // remote IP (not needed for receive)
const unsigned int outPort = 9000;		// remote port (not needed for receive)
const unsigned int localPort = 8000;
OSCErrorCode error;

Nunchuk nchuk;

//To work with: IMU_Stick.noisette
float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

void setup() {
	Serial.begin(115200);
	delay(100);
	Serial.println("On a Stick!");
	
	nchuk.begin();
	while (!nchuk.connect()) {
		Serial.println("Nunchuk not detected!");
		delay(500);
	}
	
	WiFi.begin(ssid, pass);	
	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
	}

	Udp.begin(localPort);
	
	M5.begin();
	M5.IMU.Init();
	M5.Lcd.setRotation(3);
	M5.Lcd.fillScreen(BLACK);
	M5.Lcd.setTextSize(1);
	M5.Lcd.setCursor(40, 0);
	M5.Lcd.println("IMU TEST");
	M5.Lcd.setCursor(0, 10);
	M5.Lcd.setCursor(0, 50);
	M5.Lcd.println("  Pitch   Roll    Yaw");	
}

float temp = 0;
void loop() {  
//IMU
	M5.IMU.getAhrsData(&pitch,&roll,&yaw);  
	M5.Lcd.setCursor(0, 60);
	M5.Lcd.printf(" %5.2f   %5.2f   %5.2f   ", pitch, roll, yaw);    

//OSC_IMU
	OSCBundle bndl;
	bndl.add("/imu/pitch").add(pitch);
	bndl.add("/imu/roll").add(roll);
	bndl.add("/imu/yaw").add(yaw);
	Udp.beginPacket(outIp, outPort);
	bndl.send(Udp); 
	Udp.endPacket(); 
	bndl.empty();  


//Wii Nchuck
	boolean success = nchuk.update();  // Get new data from the controller

//.. to debug if there are connection issues.
	// if (success == true) {  // We've got data!
	// 	nchuk.printDebug();  // Print all of the values!
	// }
	// else {  // Data is bad :(
	// 	Serial.println("Controller Disconnected!");
	// 	delay(1000);
	// 	nchuk.connect();		
	// }

	// bndl.add("/nkacc/x").add(nchuk.accelX());
	// bndl.add("/nkacc/y").add(nchuk.accelY());
	// bndl.add("/nkacc/z").add(nchuk.accelZ());
	bndl.add("/nkimu/roll").add(nchuk.rollAngle());
	bndl.add("/nkimu/pitch").add(nchuk.pitchAngle());
	Udp.beginPacket(outIp, outPort);
	bndl.send(Udp); 
	Udp.endPacket(); 
	bndl.empty();  

	bndl.add("/nkjoy/x").add(nchuk.joyX());
	bndl.add("/nkjoy/y").add(nchuk.joyY());
	Udp.beginPacket(outIp, outPort);
	bndl.send(Udp); 
	Udp.endPacket(); 
	bndl.empty();  	

	boolean zButton = nchuk.buttonZ();
	boolean cButton = nchuk.buttonC();
	
	if (zButton == true) {
		bndl.add("/nkbtt/z").add(1);	
	}
	else{
		bndl.add("/nkbtt/z").add(0);	
	}

	if (cButton == true) {
		bndl.add("/nkbtt/c").add(1);	
	}
	else{
		bndl.add("/nkbtt/c").add(0);	
	}
	Udp.beginPacket(outIp, outPort);
	bndl.send(Udp); 
	Udp.endPacket(); 
	bndl.empty();  


	delay(LOOPDELAY);
}