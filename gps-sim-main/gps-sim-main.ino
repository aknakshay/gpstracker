#include <Ftp.h>
#include <Geo.h>
#include <GPRS.h>
#include <Http.h>
#include <Parser.h>
#include <Result.h>
#include <Sim800.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <String.h>



// Choose two Arduino pins to use for software serial
int RXPin = 9;
int TXPin = 8;
int GPSBaud = 9600;
SoftwareSerial gpsSerial(RXPin, TXPin);

int RXPinSIM = 10;
int TXPinSIM = 11;
int RSTPinSIM = 13;
SoftwareSerial Sim800l(RXPinSIM,TXPinSIM);

// Create a TinyGPS++ object
TinyGPSPlus gps;

String data;
String latitude;
String longitude;
String alti;
String curDate;
String sat;
String curTime;


void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 Sim800l.begin(9600);
 gpsSerial.begin(9600);
 
 Serial.println("Start Program");

}


void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read())){
      data = getGPSData();
    }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }

  sendData(data);
  
}


void sendData(String data){

  String datanew = String("{\"data\": ") + data + String("}");
  
  char * tab2 = new char [datanew.length()+1];
  strcpy (tab2, datanew.c_str());
  
  const char BEARER[] PROGMEM = "airtelgprs.com";

  HTTP http(9600, RXPinSIM, TXPinSIM, RSTPinSIM);
  http.connect(BEARER);
  
  char response[256];
  Result result = http.post("31.171.250.88", tab2, response);
  
  Serial.println(response);
  
  http.disconnect();

  
}

void simSleepOff(){
  Sim800l.write("AT\r\n");
  Sim800l.write("AT+CSCLK=0'\r\n");
  delay(5000);
}

void simSleepMode(){
  Sim800l.write("AT+CSCLK=2'\r\n");
  delay(5000);
}

String getGPSData()
{
  if (gps.location.isValid())
  {
    latitude = static_cast<String>(gps.location.lat());
    longitude = static_cast<String>(gps.location.lng());
    alti = static_cast<String>(gps.altitude.meters());
  }
  else
  {
    latitude = "NA";
    longitude = "NA";
    alti = "NA";
  }
  
  if (gps.date.isValid())
  {
    curDate = static_cast<String>(gps.date.month())  +  "/"  + static_cast<String>(gps.date.day()) + "/" + static_cast<String>(gps.date.year());
  }
  else
  {
    curDate = "NA" ;
  }

  sat = static_cast<String>(gps.satellites.value());
  
  if (gps.time.isValid())
  {
    curTime = static_cast<String>(gps.time.hour()) + ":" + static_cast<String>(gps.time.minute()) + ":" + static_cast<String>(gps.time.second());
  }
  else
  {
    curTime = "NA";
  }

  String data = String("{latitude:") + latitude + ",longitude" + longitude + ",altitude:" + alti + ",date:" + curDate + ",satellites:" + sat + ",time:" + curTime + "}";

  return data;
 }
