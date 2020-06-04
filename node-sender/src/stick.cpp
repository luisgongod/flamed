#include <Arduino.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include "credentials.h"

//To work with: IMU_Stick.noisette
/
float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

WiFiUDP Udp;
OSCErrorCode error;

float pitch_mx = 0;
float roll_mx = 0;
float yaw_mx = 0;

float pitch_mn = 0;
float roll_mn = 0;
float yaw_mn = 0;

void setup() {
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
  
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
	}
  Udp.begin(localPort);

}

float temp = 0;
void loop() {  
  M5.IMU.getAhrsData(&pitch,&roll,&yaw);  
  M5.Lcd.setCursor(0, 60);
  M5.Lcd.printf(" %5.2f   %5.2f   %5.2f   ", pitch, roll, yaw);
    delay(100);


//OSC
  OSCBundle bndl;
  bndl.add("/imu").add(pitch).add(roll).add(yaw);
  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  bndl.empty(); // empty the bundle to free room for a new one
  
}