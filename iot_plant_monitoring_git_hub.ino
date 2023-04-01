//these files should be downloaded prior for oled display
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

//these files are required for thingspeak iot
#include <WiFi.h>
#include "header.h"
#include "ThingSpeak.h" 


char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;


//OLED display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//pins defining for input and output
#define mhPin 34    
#define pumpPin 13  

//variables decleration
int number = 0;
int levelMH=30;
int value;
int waterTime=1000;

//these can be modified accordingly
void display_mh(int sensor_val){
  int val=(sensor_val*100)/4095;
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(0,20);             
  display.println("Soil-Water Level:");
  display.print("    ");
  display.setTextSize(2);
  display.println(String(val)+"%");
  display.display();
}

//checking waterlevel
bool checkMH(int sensor_val){
  int val=(sensor_val*100)/4095;
  if (val>levelMH){
    return true;
  }
  else{
    return false;
  }
}

void setup() {
  Serial.begin(115200);  //Initialize serial
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  //checking display
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);// this is an infinite loop,entered when condition failed
  
  }
  
  //initializing pin
  pinMode(mhPin,INPUT);
  pinMode(pumpPin,OUTPUT);
}



void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeField(myChannelNumber, 1, number, myWriteAPIKey); 

  /*The number 200 is a standard HTTP response code that indicates a successful
  request in the HTTP protocol. When you make an HTTP request to the ThingSpeak 
  server to update a channel field, the server will send a response back to the
  client indicating the status of the request. In this case, if the server
  successfully updated the channel field, it will respond with an HTTP status
  code of 200.

  The HTTP status codes are standardized and maintained by the Internet Assigned
  Numbers Authority (IANA). The code 200 is defined in the HTTP/1.1 specification
  as a "successful response" to a request.
  */
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  
  //reading value from mhsensor
  value=4095-analogRead(mhPin);
  display_mh(value);// not sure if it will close automatically

  if (!checkMH(value)){
    digitalWrite(pumpPin,HIGH);
    //delay(waterTime);
  }
  else{
    digitalWrite(pumpPin,LOW);
  }
  
  delay(9000); // Wait 8 seconds to update the channel again
}
