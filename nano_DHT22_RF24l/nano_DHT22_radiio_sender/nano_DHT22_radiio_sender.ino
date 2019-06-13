#include <SimpleDHT.h>
#include <SPI.h>
#include "RF24.h"

// for DHT22, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT22 = 2;
SimpleDHT22 dht22(pinDHT22);

//radio addresses
byte addresses[][6] = {"0Node","1Node","2Node", "3Node", "4Node","5Node"};

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/

//SENDING DATA OVER RADIO
/**
* Create a data structure for transmitting and receiving data
* This allows many variables to be easily sent and received in a single transmission
* See http://www.cplusplus.com/doc/tutorial/structures/
*/
struct dataStruct{
  uint8_t id;
  uint8_t temp;
  uint8_t humidity;
}myData;


void setup() {
  Serial.begin(115200);
  
    radio.begin();

    myData.id = 2;

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
//  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses

    radio.openWritingPipe(addresses[radioNumber]);

  
  // Start the radio listening for data
  radio.startListening();
  

}

void loop() {
  bool new_data = false;
    // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT22...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.println(err);delay(2000);
    return;
    //TODO send this in radio
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, ");
  Serial.print((int)humidity); Serial.println(" RH%");
//  myData.temp = (uint8_t)0;
//  myData.humidity = (uint8_t)8;
    myData.temp = (uint8_t)temperature;
    myData.humidity = (uint8_t)humidity;
  
  // DHT22 sampling rate is 0.5HZ.
    new_data = true;


  //***************RADIO CODE *********************************

    radio.stopListening();                                    // First, stop listening so we can talk.
    
    
    Serial.println(F("Now sending"));

     if (!radio.write( &myData, sizeof(myData) )){
       Serial.println(F("failed"));
     }
        
    radio.startListening();                                    // Now, continue listening
    
    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
    while ( ! radio.available() ){                             // While nothing is received
      if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
          timeout = true;
          break;
      }      
    }
        
    if ( timeout ){                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
    }else{
                                                                // Grab the response, compare, and send to debugging spew
        radio.read( &myData, sizeof(myData) );
        
 
    }

    // Try again 1s later
    delay(10000);
     new_data = false;

  
  //**********************************************************

}
