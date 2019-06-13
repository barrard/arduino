
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>

ESP8266WiFiMulti WiFiMulti;

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0; // 0 is receiving

//RECEIVING DATA OVER RADIO
/**
* Create a data structure for transmitting and receiving data
* This allows many variables to be easily sent and received in a single transmission
* See http://www.cplusplus.com/doc/tutorial/structures/
*/
struct dataStruct
{
    uint8_t id;
    uint8_t temp;
    uint8_t humidity;
} myData;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(2, 15);
/**********************************************************/

byte addresses[][6] = {"0Node", "1Node", "2Node", "3Node", "4Node", "5Node"};

//String iot_url = "http://192.168.0.15:8266";
 String iot_url = "https://iot.dakine.website";

void setup()
{
    Serial.begin(115200);
    Serial.println(F("RF24/examples/GettingStarted"));
    Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

    radio.begin();

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("SanD", "sweetbelly");

    // Set the PA Level low to prevent power supply related issues since this is a
    // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
    //  radio.setPALevel(RF24_PA_LOW);

    // Open a writing and reading pipe on each radio, with opposite addresses

    radio.openReadingPipe(1, addresses[1]);
    radio.openReadingPipe(2, addresses[1]);
    radio.openReadingPipe(3, addresses[1]);
    radio.openReadingPipe(4, addresses[1]);

    // Start the radio listening for data
    radio.startListening();
}

void loop()
{

    if ((WiFiMulti.run() == WL_CONNECTED))
    {
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setInsecure();

        HTTPClient http;

        if (radio.available())
        {
            // Variable for the received timestamp
            while (radio.available())
            {                                        // While there is data ready
                radio.read(&myData, sizeof(myData)); // Get the payload
            }
            Serial.print(F("Got data from "));
            Serial.print(myData.id);
            Serial.print(F(" - Temp is "));
            Serial.print(myData.temp);
            Serial.print(F(" : "));
            Serial.print(F("Humidity is "));

            Serial.println(myData.humidity);

            Serial.print("[HTTP] begin...\n");
            if (http.begin(*client, iot_url + "/temp/" + myData.temp + "/" + myData.humidity + "/" + myData.id))
            { // HTTP
                http.addHeader("secret", "8266iot");

                Serial.print("[HTTP] GET...\n");
                // start connection and send HTTP header
                int httpCode = http.GET();

                http.end();
            }
            else
            {
                Serial.printf("[HTTP} Unable to connect\n");
            }
        }
    }

    delay(10000);

} // Loop
