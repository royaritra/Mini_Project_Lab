///The Code for Automated Health Monitoring System

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define BLYNK_PRINT Serial
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_MLX90614.h>
#include "Wire.h"
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
double AmbC, BodC, AmbF, BodF;
#define REPORTING_PERIOD_MS 1000

 
char auth[] = "us0iOvQ-4JV-gQS4QYSwIg8QR6PspmNT";             // Authentication Token Sent by Blynk
char ssid[] = "aritra_4G";        //WiFi SSID
char pass[] = "Home@4321";        //WiFi Password
 
// Connections : SCL PIN - D1 , SDA PIN - D2 , INT PIN - D0
PulseOximeter pox;
double ecg;
double BPM, SpO2,Temp, ECG ;
uint32_t tsLastReport = 0;
 

//Function to get temparature data from Sensor
 double get_temp(){
  AmbC= mlx.readAmbientTempC(); 
  BodC= mlx.readObjectTempC();
  AmbF= mlx.readAmbientTempF(); 
  BodF= mlx.readObjectTempF(); 
  delay(500);
  return BodC;
  }

//Function to get ECG data from Sensor
double get_ecg(){
  if((digitalRead(10) == 1)||(digitalRead(11) == 1)){
    Serial.println('!');
  }
  else{
    // send the value of analog input 1:
      ecg=analogRead(A0);
  }
  //Wait for a bit to keep serial data from saturating
  delay(1);
  return ecg;
}

void onBeatDetected()
{
    Serial.println("Beat Detected!");
    
}
const unsigned char bitmap [] PROGMEM=
{
0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
void setup()
{
    Serial.begin(9600);
    pinMode(16, OUTPUT);
    //pinMode(A1,INPUT);
    Blynk.begin(auth, ssid, pass);
    pinMode(9, INPUT); // Setup for leads off detection LO +
    pinMode(10, INPUT); // Setup for leads off detection LO -
    //Serial.print("Initializing Pulse Oximeter..");
 
    if (!pox.begin())
    {
         Serial.println("FAILED");
         for(;;);
    }
    else
    {
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }
 
    // The default current for the IR LED is 50mA and it could be changed by uncommenting the following line.
     pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    mlx.begin();  
}
 
void loop()
{
    pox.update();
    Blynk.run();
 
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    Temp=get_temp();
    ECG=get_ecg();
    
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Blynk.virtualWrite(V7, BPM);
        Blynk.virtualWrite(V8, SpO2);
        Blynk.virtualWrite(V9, Temp);
        Blynk.virtualWrite(V6, ECG); 
        
        tsLastReport = millis();
    }
    Serial.println("new");
}
