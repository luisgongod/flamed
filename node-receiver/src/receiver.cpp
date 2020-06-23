/*
 Receiver using an M5 Atom.
 TODO: 
 - test pixel functions
 - Use board with better antena.
 */

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <FastLED.h>
#include "credentials.h"

#define MIN_HUE 0
#define MIN_SAT 0
#define MIN_VAL 0
#define MAX_HUE 255
#define MAX_SAT 255
#define MAX_VAL 125// to keep under 1.5A
// #define MAX_VAL 255// full power!

// the IP address for the shield:
IPAddress ip(192, 168, 2, 105);    //atom
IPAddress gateway(192, 168, 2, 254);    
IPAddress subnet(255, 255, 255, 0);      

#define LED_PIN 26
#define NUM_LEDS 60
#define LED_TYPE WS2812B //Very important to have the right LED Chip name
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(192,168,2,73); // K7
const unsigned int outPort = 9000;	//Send port
const unsigned int localPort = 8888;	//Receive port

//Global Global vars
OSCErrorCode error;

uint8_t v_bg_hue = 0;
uint8_t v_bg_sat = 0;
uint8_t v_bg_val = 0;

uint8_t v_0_hue = 0;
uint8_t v_0_sat = 0;
uint8_t v_0_val = 0;
uint8_t v_0_pos = 0;

uint8_t v_1_hue = 0;
uint8_t v_1_sat = 0;
uint8_t v_1_val = 0;
uint8_t v_1_pos = 0;

void setup(){

   	Serial.begin(115200);   

    WiFi.config(ip,gateway,subnet);
   	WiFi.begin(ssid, pass);
  	// FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS);

	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
	}
    Udp.begin(localPort);

	//Print Wifi Stuff Serial
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.println("Starting UDP");
	Serial.print("Local port: ");
    Serial.print(localPort);
    Serial.println("");
   
}

//Background Functions
void bg_hue(OSCMessage &msg){
	v_bg_hue = map(msg.getFloat(0)*100, 0, 100, MIN_HUE, MAX_HUE);
}
void bg_saturation(OSCMessage &msg){
	v_bg_sat = map(msg.getFloat(0)*100, 0, 100, MIN_SAT, MAX_SAT);
}
void bg_value(OSCMessage &msg){
	v_bg_val = map(msg.getFloat(0)*100, 0, 100, MIN_VAL, MAX_VAL);
}

//Pixel Functions
void pixel_0_hue(OSCMessage &msg){
	v_0_hue = map(msg.getFloat(0)*100, 0, 100, MIN_HUE, MAX_HUE);
}
void pixel_0_saturation(OSCMessage &msg){
	v_0_sat = map(msg.getFloat(0)*100, 0, 100, MIN_SAT, MAX_SAT);
}
void pixel_0_value(OSCMessage &msg){
	v_0_val = map(msg.getFloat(0)*100, 0, 100, MIN_VAL, MAX_VAL);
}
void pixel_0_position(OSCMessage &msg){
	v_0_pos = map(msg.getFloat(0)*100, 0, 100, 0, NUM_LEDS);
}

void pixel_1_hue(OSCMessage &msg){
	v_1_hue = map(msg.getFloat(0)*100, 0, 100, MIN_HUE, MAX_HUE);
}
void pixel_1_saturation(OSCMessage &msg){
	v_1_sat = map(msg.getFloat(0)*100, 0, 100, MIN_SAT, MAX_SAT);
}
void pixel_1_value(OSCMessage &msg){
	v_1_val = map(msg.getFloat(0)*100, 0, 100, MIN_VAL, MAX_VAL);
}
void pixel_1_position(OSCMessage &msg){
	v_1_pos = map(msg.getFloat(0)*100, 0, 100, 0, NUM_LEDS);
}


/**************************LOOP***********************************/

void loop(){
	OSCMessage msg;
	int size = Udp.parsePacket();

	if (size > 0)	{
		while (size--)		{
			msg.fill(Udp.read());
		}
		if (!msg.hasError()){			

			//Background
			msg.dispatch("/core/bg/hue", bg_hue);
        	msg.dispatch("/core/bg/sat", bg_saturation);
        	msg.dispatch("/core/bg/val", bg_value);
			           
            int i;
            // fill_solid(&(leds[i]), NUM_LEDS, CHSV(0, 0, 0));
            fill_solid(&(leds[i]), NUM_LEDS, CHSV(v_bg_hue,v_bg_sat,v_bg_val));

			//Pixel		    
			msg.dispatch("/core/0/hue", pixel_0_hue);
        	msg.dispatch("/core/0/sat", pixel_0_saturation);
        	msg.dispatch("/core/0/val", pixel_0_value);
			msg.dispatch("/core/0/pos", pixel_0_position);
			msg.dispatch("/core/1/hue", pixel_1_hue);
        	msg.dispatch("/core/1/sat", pixel_1_saturation);
        	msg.dispatch("/core/1/val", pixel_1_value);
			msg.dispatch("/core/1/pos", pixel_1_position);

void fire_consumer(OSCMessage &msg, int addressOffset){			
			msg.dispatch("/fire/val", fire_value);
        	msg.dispatch("/fire/cool", fire_cooling);
			msg.dispatch("/fire/sparks", fire_sparks);
        	msg.dispatch("/fire/nleds", fire_nleds);
}

            Serial.printf("bg:%d %d %d pixel: %d:%d  %d:%d  %d:%d  %d:%d\n",v_bg_hue,v_bg_sat,v_bg_val,v_0_hue,v_1_hue,v_0_sat,v_1_sat,v_0_val,v_1_val,v_0_pos,v_1_pos);
	        FastLED.show();
		}
		else
		{
			error = msg.getError();
			Serial.print("error: ");
			Serial.println(error);
		}
	}

}
