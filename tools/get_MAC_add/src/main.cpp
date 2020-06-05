//Simple Sketch to put device into the network to find MAC/IP and so
#include <SPI.h>
#include <WiFi.h>
#include "credentials.h"

byte mac[6]; // the MAC address of your Wifi shield

void setup(){
	Serial.begin(115200);

	WiFi.begin(ssid,pass);
	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
	}
	// if you are connected, print your MAC address:
		Serial.print("IP Address: ");
	  	Serial.println(WiFi.localIP());
	
	// MAC might be flip, confirm with the router or command:
	// sudo nmap -sP 192.168.2.0/24 | awk '/Nmap scan report for/{printf $5;}/MAC Address:/{print " => "$3;}' | sort
		WiFi.macAddress(mac);
		Serial.print("MAC: ");
		Serial.print(mac[5], HEX);
		Serial.print(":");
		Serial.print(mac[4], HEX);
		Serial.print(":");
		Serial.print(mac[3], HEX);
		Serial.print(":");
		Serial.print(mac[2], HEX);
		Serial.print(":");
		Serial.print(mac[1], HEX);
		Serial.print(":");
		Serial.println(mac[0], HEX);
	
}

void loop() {}
