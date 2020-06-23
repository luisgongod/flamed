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
#define MAX_VAL 255
#define FRAMES_PER_SECOND 60
#define LED_PIN 26
#define NUM_LEDS 60
#define LED_TYPE WS2812B //Very important to have the right LED Chip name
#define COLOR_ORDER GRB

bool gReverseDirection = false;

IPAddress ip(192, 168, 2, 105);    //atom
IPAddress gateway(192, 168, 2, 254);    
IPAddress subnet(255, 255, 255, 0);      

CRGB leds[NUM_LEDS];

WiFiUDP Udp;
const IPAddress outIp(192,168,2,73); // K7
const unsigned int outPort = 9000;	//Send port
const unsigned int localPort = 8888;	//Receive port

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

uint8_t v_fire_value = 180;
uint8_t v_fire_cooling = 50;
uint8_t v_fire_sparks = 120;
uint8_t v_fire_nleds = NUM_LEDS;

uint8_t mode = 1; //manual

void setup(){
   	Serial.begin(115200);   

    WiFi.config(ip,gateway,subnet);
   	WiFi.begin(ssid, pass);
  	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	
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

//Fire funtions
void fire_value(OSCMessage &msg){
	v_fire_value = msg.getInt(0);
}
void fire_cooling(OSCMessage &msg){
	v_fire_cooling = msg.getInt(0);
}
void fire_sparks(OSCMessage &msg){
	v_fire_sparks = msg.getInt(0);
}
void fire_nleds(OSCMessage &msg){
	v_fire_nleds = msg.getInt(0);
}

void mode_mode(OSCMessage &msg){
	mode = msg.getInt(0); 	
}

//Modes
void Fire2012(){
	FastLED.setBrightness( v_fire_value );
	
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {		
      heat[i] = qsub8( heat[i],  random8(0, ((v_fire_cooling * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < v_fire_sparks ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

void core(){
	// int i;
	// fill_solid(&(leds[i]), NUM_LEDS, CHSV(0, 0, 0));
	fill_solid(leds, NUM_LEDS, CHSV(v_bg_hue,v_bg_sat,v_bg_val));
	fill_gradient(leds, v_0_pos,CHSV(v_0_hue,v_0_sat,v_0_val), v_1_pos,CHSV(v_1_hue,v_1_sat,v_1_val));            

	Serial.printf("bg:%d %d %d pixel: %d:%d  %d:%d  %d:%d  %d:%d\n",v_bg_hue,v_bg_sat,v_bg_val,v_0_hue,v_1_hue,v_0_sat,v_1_sat,v_0_val,v_1_val,v_0_pos,v_1_pos);			
}

//Consumers
void core_consumer(OSCMessage &msg, int addressOffset){
			//Background
			msg.dispatch("/core/bg/hue", bg_hue);
        	msg.dispatch("/core/bg/sat", bg_saturation);
        	msg.dispatch("/core/bg/val", bg_value);
			           
			//Pixel		    
			msg.dispatch("/core/0/hue", pixel_0_hue);
        	msg.dispatch("/core/0/sat", pixel_0_saturation);
        	msg.dispatch("/core/0/val", pixel_0_value);
			msg.dispatch("/core/0/pos", pixel_0_position);
			msg.dispatch("/core/1/hue", pixel_1_hue);
        	msg.dispatch("/core/1/sat", pixel_1_saturation);
        	msg.dispatch("/core/1/val", pixel_1_value);
			msg.dispatch("/core/1/pos", pixel_1_position);
}


void fire_consumer(OSCMessage &msg, int addressOffset){			
			msg.dispatch("/fire/val", fire_value);
        	msg.dispatch("/fire/cool", fire_cooling);
			msg.dispatch("/fire/sparks", fire_sparks);
        	msg.dispatch("/fire/nleds", fire_nleds);
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
			msg.route("/core", core_consumer);	
			msg.route("/fire", fire_consumer);	
			msg.dispatch("/mode", mode_mode);
		}
		else
		{
			error = msg.getError();
			Serial.print("error: ");
			Serial.println(error);
		}
	}

	switch (mode)
	{
	case 1:
		core();
		break;
	case 2:
		Fire2012();
		break;
	default:
		break;
	}	

	FastLED.show();
	FastLED.delay(1000 / FRAMES_PER_SECOND);
}
