#include <ESP8266WiFi.h>
#include "DHT.h"
#include <ESP8266mDNS.h>
#include <PubSubClient.h>


const char* WIFI_SSID = "MyHome"; 
const char* WIFI_PWD = "korcsogek";


IPAddress MQTTserver(192, 168, 1, 92);
WiFiClient wclient;
PubSubClient client(wclient, MQTTserver);

#define DHTPIN D1 // Wemos D1 Mini's D1 pin ....DHT PIN 2 (data)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


const int UPDATE_INTERVAL_SECS = 9; // Update every 9 second...modify if you want :-)


// Initialize the temperature/ humidity sensor
DHT dht(DHTPIN, DHTTYPE);
float humidity = 0.;
float temperature = 0.;

void setup() {
  // Setup console
  Serial.begin(115200);
  delay(10);
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void HomeKit(){  
    if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect("ESP8266: DHT Sensor")) {
        client.publish("HomeKit","Temperature Sensor Online!");
        client.publish("HomeKit","Humidity Sensor Online!");
      }
    }

    if (client.connected()){
      Serial.println("publishing " +  String(dht.readTemperature()) + "°");
        client.publish("RoomTemperature",String(dht.readTemperature()));   
        Serial.println("publishing " +  String(dht.readHumidity()) + "%");
        client.publish("RoomHumidity",String(dht.readHumidity()));   
        client.loop();
        delay(6000); // reading every 6 sec
    }
      
  } 
}

void loop() {
 HomeKit();
 Serial.println("Sending completed");
 delay(UPDATE_INTERVAL_SECS * 1000);
}
