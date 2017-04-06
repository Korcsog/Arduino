//AMS iAQ Core C
// http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-C

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "brzo_i2c.h"  // just google it!

IPAddress MQTTserver(192, 168, 1, 40); // MQTT server IP address
WiFiClient wclient;
PubSubClient client(wclient, MQTTserver);

const char* ssid = "ssid"; // Your ssid
const char* pass = "password"; // Your Password
const char* clientName = "ESP8266-AirQualitySensor1"; // must be unique

uint8_t SCL_PIN = D1; // wemos SCL PIN
uint8_t SDA_PIN = D2; // wemos SDA PIN
uint8_t iaq_adr = 0x5A;


uint8_t buffer[10];

uint8_t error = 0;
uint16_t co2;
uint16_t tvoc;

void setup() 
{
   Serial.begin(115200);
  brzo_i2c_setup(SDA_PIN, SCL_PIN, 3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void readAllBytes() {
   brzo_i2c_start_transaction(iaq_adr, 100);
   brzo_i2c_read(buffer, 9, true);

   if (buffer[2] == 0x10 ){
       Serial.println("Warming up...");
       delay(1000); // ezt én írtam be
   } else
   {

    co2 = buffer[0] * pow(2, 8) + buffer[1];
    tvoc = buffer[7] * pow(2, 8) + buffer[8];
    
    Serial.println("CO2: " +  String(co2));
    Serial.println("TVOC: " +  String(tvoc));
   }
   error = brzo_i2c_end_transaction();

   if (error == 0) {
     //Serial.println("No i2c communication errors");
  }
  else {
    Serial.print("Brzo error : ");
    Serial.println(error);
  }

   delay(1000);
} 


void HomeKit(){  
    if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect(clientName)) {
        client.publish("outTopic","Air Quality Sensor Online!");
      }
    }

    if (client.connected())
  
    {
   
      Serial.println("Publishing CO2: " +  String(co2));
        client.publish("airquality1/co2",String(co2));   // publishing to airquality1/co2 topic
        Serial.println("Publishing VOC: " +  String(tvoc));
        client.publish("airquality1/voc",String(tvoc));  // publishing to airquality1/voc topic
        client.loop();
      
    }
  } 
}


void loop()
{     
  readAllBytes();
  HomeKit();
  Serial.println("Sending completed");
  delay(4000);  // read the sensor and publish the data every 4 sec

}



