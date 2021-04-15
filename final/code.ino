
#include <SoftwareSerial.h>

#include "SIM800L.h"
#include <TinyGPS++.h>
#include <String.h>
#include <limits.h>
#define SIM800_RX_PIN 13
#define SIM800_TX_PIN 12
#define SIM800_RST_PIN 11

const char APN[] = "airtelgprs.com";
const char URL[] = "http://31.171.250.88:8080/";
const char CONTENT_TYPE[] = "application/json";

SIM800L* sim800l;


// Choose two Arduino pins to use for software serial
int RXPin = 4;
int TXPin = 3;

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(9600);
  // Initialize a SoftwareSerial


  // Equivalent line with the debug enabled on the Serial
 //  sim800l = new SIM800L((Stream *)&Serial, SIM800_RST_PIN, 200, 512, (Stream *)&Serial);

  // Start the software serial port at the GPS's default baud

  
  // Setup module for GPRS communication
  setupModule();

}
 
void loop() {

//
    
Serial.println("Start loop");

  String gpsData = getGpsData();
  const char PAYLOAD1[gpsData.length()+1];

  gpsData.toCharArray(PAYLOAD1, gpsData.length()+1);
  Serial.println("sent gps data");
  Serial.println(PAYLOAD1);

  SoftwareSerial* serial = new SoftwareSerial(SIM800_RX_PIN, SIM800_TX_PIN);
  serial->begin(9600);
  delay(1000);
  sim800l = new SIM800L((Stream *)serial, SIM800_RST_PIN, 200, 512);
  
  

  // Establish GPRS connectivity (5 trials)
  bool connected = false;
  for(uint8_t i = 0; i < 5 && !connected; i++) {
    delay(1000);
    connected = sim800l->connectGPRS();
    Serial.println(connected);
  }

  // Check if connected, if not reset the module and setup the config again
  if(connected) {
    Serial.println(F("GPRS connected !"));
  } else {
    Serial.println(F("GPRS not connected !"));
    Serial.println(F("Reset the module."));
    sim800l->reset();
    setupModule();
    return;
  }

  Serial.println(F("Start HTTP POST..."));

  // Do HTTP POST communication with 10s for the timeout (read and write)
  uint16_t rc = sim800l->doPost(URL, CONTENT_TYPE, PAYLOAD1, 10000, 10000);
   if(rc == 200) {
    // Success, output the data received on the serial
    Serial.print(F("HTTP POST successful ("));
    Serial.print(sim800l->getDataSizeReceived());
    Serial.println(F(" bytes)"));
    Serial.print(F("Received : "));
    Serial.println(sim800l->getDataReceived());
  } else {
    // Failed...
    Serial.print(F("HTTP POST error "));
    Serial.println(rc);
  }

  // Close GPRS connectivity (5 trials)
  bool disconnected = sim800l->disconnectGPRS();
  for(uint8_t i = 0; i < 5 && !connected; i++) {
    delay(1000);
    disconnected = sim800l->disconnectGPRS();
  }
  
  if(disconnected) {
    Serial.println(F("GPRS disconnected !"));
  } else {
    Serial.println(F("GPRS still connected !"));
  }

  // Go into low power mode
  bool lowPowerMode = sim800l->setPowerMode(MINIMUM);
  if(lowPowerMode) {
    Serial.println(F("Module in low power mode"));
  } else {
    Serial.println(F("Failed to switch module to low power mode"));
  }

   delete sim800l;
  // End of program... wait...
  delay(1000);
  
    delay(2000);
}

void setupModule() {


  SoftwareSerial* serial = new SoftwareSerial(SIM800_RX_PIN, SIM800_TX_PIN);
  serial->begin(9600);
  delay(1000);
  sim800l = new SIM800L((Stream *)serial, SIM800_RST_PIN, 200, 512);
  
    // Wait until the module is ready to accept AT commands
  while(!sim800l->isReady()) {
    Serial.println(F("Problem to initialize AT command, retry in 1 sec"));
    delay(1000);
  }
  Serial.println(F("Setup Complete!"));

  // Wait for the GSM signal
  uint8_t signal = sim800l->getSignal();
  while(signal <= 0) {
    delay(1000);
    signal = sim800l->getSignal();
  }
  Serial.print(F("Signal OK (strenght: "));
  Serial.print(signal);
  Serial.println(F(")"));
  delay(1000);

  // Wait for operator network registration (national or roaming network)
  NetworkRegistration network = sim800l->getRegistrationStatus();
  while(network != REGISTERED_HOME && network != REGISTERED_ROAMING) {
    delay(1000);
    network = sim800l->getRegistrationStatus();
    Serial.println(network);
  }
  Serial.println(F("Network registration OK"));
  delay(1000);

  // Setup APN for GPRS configuration
  bool success = sim800l->setupGPRS(APN);
  while(!success) {
    success = sim800l->setupGPRS(APN);
    Serial.println(success);
    delay(5000);
  }
  Serial.println(F("GPRS config OK"));

    delete sim800l;

      gpsSerial.begin(9600);


}

String getGpsData(){

gpsSerial.begin(9600);
  float latitude = LONG_MIN;
  float longitude = LONG_MIN;
  float alti = LONG_MIN;
  
  while (latitude < -500){
  while (gpsSerial.available() > 0){
    if (gps.encode(gpsSerial.read())){
        Serial.println("gps readable");



  if (gps.location.isValid())
  {
    Serial.println("getting gps loc");
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    alti = gps.altitude.meters();
  }

  static int date_month = INT_MIN;
  static int date_day = INT_MIN;
  static int date_year = INT_MIN;
  
  if (gps.date.isValid())
  {
  date_month = gps.date.month();
  date_day = gps.date.day();
  date_year = gps.date.year();
  }


  int sat = gps.satellites.value();
  static int time_hour = INT_MIN;
  static int time_minute = INT_MIN;
  static int time_second = INT_MIN;
  
  if (gps.time.isValid())
  {
    time_hour = gps.time.hour();
    time_minute = gps.time.minute();
    time_second = gps.time.second();
  }


//="{\"name\": \"morpheus\", \"job\": \"leader\"}";
 String data("{\"lat\":\"" + String(latitude) + "\",\"lon\":\"" + longitude + "\"}");
  Serial.println(data);

 if (latitude > -500){
  return data;
  }
    }}}
    
}



