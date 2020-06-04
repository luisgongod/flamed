#include <WiFi.h>

IPAddress ip(192, 168, 2, 104);    
IPAddress gateway(192, 168, 2, 254);    
IPAddress subnet(255, 255, 255, 0);       

char ssid[] = "batcave";    // your network SSID (name)
char pass[] = "iambatman!"; // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

void setup()
{  
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
delay(500);
    Serial.println("Serial ok");

  
  WiFi.config(ip,gateway,subnet);
  WiFi.begin(ssid, pass);
  
	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
	}
  
}

void loop () {
   Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

// #include <WiFi.h>
// int status = WL_IDLE_STATUS;     // the Wifi radio's status

// //SSID of your network
// char ssid[] = "batcave";
// //password of your WPA Network
// char pass[] = "imbatman!";

// IPAddress ip;
// IPAddress subnet;
// IPAddress gateway;

// void setup()
// {
//   WiFi.begin(ssid, pass);

//   if ( status != WL_CONNECTED) {
//     Serial.println("Couldn't get a wifi connection");
//     while(true);
//   }
//   // if you are connected, print out info about the connection:
//   else {

//     // print your subnet mask:
//     subnet = WiFi.subnetMask();
//     Serial.print("NETMASK: ");
//     Serial.println(subnet);

//   }
// }

// void loop () {
// }
 
