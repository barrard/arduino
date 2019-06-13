#include "DHTesp.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>

ESP8266WiFiMulti WiFiMulti;
#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

DHTesp dht;
  // String iot_url = "http://192.168.0.15:8266";
  String iot_url = "https://iot.dakine.website";
void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  
    WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("SanD", "sweetbelly");

  // Autodetect is not working reliable, don't use the following line
  // dht.setup(17);
  // use this instead: 
//  dht.setup(2, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
  dht.setup(2, DHTesp::DHT11); // Connect DHT sensor to GPIO 17


}

void loop()
{


  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print("Humidity on NODEMCU DHT ");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
    Serial.print("and Temp is ");

  Serial.print(temperature, 1);
  Serial.println("\t\t");
//  Serial.print(dht.toFahrenheit(temperature), 1);
//  Serial.print("\t\t");
  Serial.print("Heat index = ");
  Serial.println(dht.computeHeatIndex(temperature, humidity, false), 1);
  Serial.print("head index fer  ");
  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setInsecure();

//    WiFiClientSecure client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(*client, iot_url + "/temp/"+temperature+"/"+humidity+"/0")) {  // HTTP
        http.addHeader("secret", "8266iot");

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
            Serial.print(httpCode);


      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  delay(25000);
}
