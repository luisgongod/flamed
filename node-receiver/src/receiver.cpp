#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <M5Stack.h>
#include <FastLED.h>
#include "credentials.h"

//TODO: sender/receiver flag within code: No FastLED in Sender 

#define LED_PIN 21 //G21: groove port
#define NUM_LEDS 60
#define LED_TYPE WS2812B //Very important to have the right LED Chip name
// #define LED_TYPE NEOPIXEL //Neopixel RGBW not supported
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

IPAddress ip(192, 168, 2, 101);    
IPAddress gateway(192, 168, 2, 254);    
IPAddress subnet(255, 255, 255, 0);       

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(192,168,2,73); // remote IP (not needed for receive)
const unsigned int outPort = 9000;		// remote port (not needed for receive)
const unsigned int localPort = 8000;	// local port to listen for UDP packets (here's where we send the packets)

//Global Global vars
OSCErrorCode error;

void setup(){

	M5.begin();
   	Serial.begin(115200);
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
    
    M5.Lcd.setTextSize(2);
	M5.Lcd.print(WiFi.localIP());
	M5.Lcd.print(" UDP ");	
	M5.Lcd.println(localPort);
    
}


#define MIN_HUE 0
#define MIN_SAT 0
#define MIN_VAL 0
#define MAX_HUE 255
#define MAX_SAT 255
#define MAX_VAL 255

uint8_t v_hue = 0;
uint8_t v_saturation = 0;
uint8_t v_value = 0;

uint8_t v_hue1 = 0;
uint8_t v_saturation1 = 0;
uint8_t v_value1 = 0;

int v_ledposition0 = 0;
int v_ledposition1 = 0;

/**
Messages directly from OSC
/1/fader1
/1/toggle1
/1/xy
/accxyz
/4/multitoggle/8/1
/push1
/multifader/1
/rotary
*/

void led_hue(OSCMessage &msg){
	v_hue = map(msg.getFloat(0)*100, 0, 100, MIN_HUE, MAX_HUE);
}
void led_saturation(OSCMessage &msg){
	v_saturation = map(msg.getFloat(0)*100, 0, 100, MIN_SAT, MAX_SAT);
}
void led_brigthness(OSCMessage &msg){
	v_value = map(msg.getFloat(0)*100, 0, 100, MIN_VAL, MAX_VAL);
}
void led_number(OSCMessage &msg){
	v_ledposition0 = map(msg.getFloat(0)*100, 0, 100, 0, NUM_LEDS);
}


void led_hue1(OSCMessage &msg){
	v_hue1 = map(msg.getFloat(0)*100, 0, 100, MIN_HUE, MAX_HUE);
}
void led_saturation1(OSCMessage &msg){
	v_saturation1 = map(msg.getFloat(0)*100, 0, 100, MIN_SAT, MAX_SAT);
}
void led_brigthness1(OSCMessage &msg){
	v_value1 = map(msg.getFloat(0)*100, 0, 100, MIN_VAL, MAX_VAL);
}

void led_number1(OSCMessage &msg){
	v_ledposition1 = map(msg.getFloat(0)*100, 0, 100, 0, NUM_LEDS-1);
}

/**************************LOOP***********************************/

void loop(){
	OSCMessage msg;
	int size = Udp.parsePacket();

	if (size > 0)
	{
		while (size--)
		{
			msg.fill(Udp.read());
		}
		if (!msg.hasError())
		{
			//Use OSCTouch "Mix 16"
			msg.dispatch("/2/fader1", led_hue);
        	msg.dispatch("/2/fader2", led_saturation);
        	msg.dispatch("/2/fader3", led_brigthness);
        	msg.dispatch("/2/fader4", led_number);            
            msg.dispatch("/2/fader5", led_hue1);
        	msg.dispatch("/2/fader6", led_saturation1);
        	msg.dispatch("/2/fader7", led_brigthness1);
        	msg.dispatch("/2/fader8", led_number1);
            
            int i;
            fill_solid(&(leds[i]), NUM_LEDS, CHSV(0, 0, 0));
	        
			fill_gradient(&(leds[i]),v_ledposition0,CHSV(v_hue,v_saturation,v_value), v_ledposition1, CHSV(v_hue1, v_saturation1, v_value1));
            fill_gradient(&(leds[i]),v_ledposition0,CHSV(v_hue,v_saturation,v_value), v_ledposition1, CHSV(v_hue, v_saturation, v_value));

            Serial.printf("%d,%d,%d,%d\n",v_hue,v_saturation,v_value,v_ledposition0);
	        FastLED.show();

		}
		else
		{
			error = msg.getError();
			Serial.print("error: ");
			Serial.println(error);
		}
	}

	M5.update();
}
