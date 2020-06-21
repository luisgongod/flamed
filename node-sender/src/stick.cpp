/*
To be used with M5Stick with or without the Wiimote.
Use Chataigne file: IMU_Stick.noisette

M5 Stick resolution: 80x160
*/
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
IPAddress ip(192, 168, 2, 102);    
IPAddress gateway(192, 168, 2, 254);    
IPAddress subnet(255, 255, 255, 0);     

const IPAddress outIp(192,168,2,73); // remote IP (not needed for receive)
const unsigned int outPort = 9000;		// remote port (not needed for receive)
const unsigned int localPort = 8000;
OSCErrorCode error;

Nunchuk nchuk;

float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

bool nk_connected = false;

void setup() {
	Serial.begin(115200);
	delay(100);
	Serial.println("On a Stick!");

	M5.begin();
	M5.IMU.Init();
	M5.Lcd.setRotation(1);
	M5.Lcd.fillScreen(BLACK);
	M5.Lcd.setTextSize(2);
	M5.Lcd.setCursor(0, 0);
	M5.Lcd.println("Stick!");

	// M5.Lcd.setCursor(0, 10);
	// M5.Lcd.setCursor(0, 50);
	// M5.Lcd.println("  Pitch   Roll    Yaw");	
	
	nchuk.begin();
	M5.Lcd.print("NChuck");
	for (int i = 0; i < 3; i++)	{
		if (!nchuk.connect() ){
		Serial.println("Connecting to Nunchuk");
		M5.Lcd.print(".");
		delay(400);
		}
		else{
			M5.Lcd.println(". OK!");
			nk_connected = true;
			break;				
		}		
	}
	if (nk_connected==false){
		M5.Lcd.println("off");
	}

	WiFi.config(ip,gateway,subnet);
	WiFi.begin(ssid, pass);	
	M5.Lcd.print("wifi.");
	while (WiFi.status() != WL_CONNECTED){
		delay(400);
		Serial.print(".");
	}
	M5.Lcd.println(". OK!");

	Udp.begin(localPort);
}

void loop() {  
//IMU
	M5.IMU.getAhrsData(&pitch,&roll,&yaw);  
	// M5.Lcd.setCursor(0, 60);
	// M5.Lcd.printf(" %5.2f   %5.2f   %5.2f   ", pitch, roll, yaw);    

//OSC_IMU
	OSCBundle bndl;
	bndl.add("/imu/pitch").add(pitch);
	bndl.add("/imu/roll").add(roll);
	bndl.add("/imu/yaw").add(yaw);
	Udp.beginPacket(outIp, outPort);
	bndl.send(Udp); 
	Udp.endPacket(); 
	bndl.empty();  

	Serial.printf("p=%5.2f r=%5.2f y=%5.2f\n", pitch, roll, yaw);


//Wii Nchuck
	if(nk_connected == true){
		boolean success = nchuk.update();  // Get new data from the controller

		if (success == true) {
		/*Sending IMU*/
		bndl.add("/nkacc/x").add(nchuk.accelX());
		bndl.add("/nkacc/y").add(nchuk.accelY());
		bndl.add("/nkacc/z").add(nchuk.accelZ());
		bndl.add("/nkimu/roll").add(nchuk.rollAngle());
		bndl.add("/nkimu/pitch").add(nchuk.pitchAngle());
		Udp.beginPacket(outIp, outPort);
		bndl.send(Udp); 
		Udp.endPacket(); 
		bndl.empty();  

		/*Sending Joystick*/
		bndl.add("/nkjoy/x").add(nchuk.joyX());
		bndl.add("/nkjoy/y").add(nchuk.joyY());
		Udp.beginPacket(outIp, outPort);
		bndl.send(Udp); 
		Udp.endPacket(); 
		bndl.empty();  	

		/*Sending Buttons*/
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
		}

		// Serial.printf("p=%5.2f r=%5.2f y=%5.2f\n", pitch, roll, yaw);

		else {  // Data is bad :(		
			M5.Lcd.setCursor(0, 0);
			M5.Lcd.println("                         ");
			M5.Lcd.println("                         ");
			M5.Lcd.println("                         ");
			M5.Lcd.println("                         ");
			M5.Lcd.setCursor(0, 0);
			M5.Lcd.print("NChuck \ndisconnected!");

			Serial.println("\nController Disconnected!");

			while(1){
				if (!nchuk.connect() ){		
				M5.Lcd.print(".");
				delay(600);
				}
				else{
					M5.Lcd.println("\nOK!");			
					M5.Lcd.setCursor(0, 0);
					M5.Lcd.println("                         ");
					M5.Lcd.println("                         ");
				
					break;				
				}
			}
			
		}
	}

	delay(LOOPDELAY);
	Serial.println("");
}