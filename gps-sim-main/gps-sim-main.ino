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
int RXPin = 12;
int TXPin = 11;
SoftwareSerial gpsSerial(RXPin, TXPin);

int RXPinSIM = 7;
int TXPinSIM = 8;
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

 simSetup();
}


void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      sendData(getGPSData());

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }

    delay(10000);

}


void sendData(String data){

  String datanew = String("{\"data\": ") + data + String("}");
  Serial.println(datanew);
  char * tab2 = new char [datanew.length()+1];
  strcpy (tab2, datanew.c_str());
  Serial.println("posting request\n\n");

  Sim800l.write("AT+CIPSTART=\"TCP\",\"31.171.250.88\",8080");
  Sim800l.write("WAIT=3");

  String request = String("\nPOST / HTTP/1.1\nHost: 31.171.250.88:8080\nContent-Type: application/json\nContent-Length:") + static_cast<String>(datanew.length()) + String("\n");
  Serial.println(request.length());
  String frequest = String("CIPSEND=") + static_cast<String>(request.length()+11) + String(request);
  Serial.println(request);
  Serial.println(frequest);
  Serial.println(tab2);
  char * req = new char [frequest.length()+1];
  strcpy (req, frequest.c_str());
  
  Sim800l.write(req);
  Sim800l.write(tab2);

  Serial.println("********************************");
}

void simSetup(){

  Serial.println("setting up your sim");
//  enter GPRS configuration mode
  Sim800l.write("AT+CGATT=1\r\n");
// Disable multi IP connection mode.  Single IP cnxn only
  Sim800l.write("AT+CIPMUX=0\r\n");
// set up apn
  Sim800l.write("AT+CSTT=\"airtelgprs.com\",\"\",\"\"\r\n");
// bring up wireless connection with GPRS and CSD 
  Sim800l.write("AT+CIICR\r\n");
// ip up - gprs working
  Sim800l.write("AT+CIFSR\r\n");
// Exit GPRS configuration mode
  Sim800l.write("AT+CIPSHUT\r\n");
  Serial.println("sim set up is complete");

  
// ref: https://stackoverflow.com/questions/33346425/sim800-at-command-post-data-to-server
  
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

  Serial.print("gps data concatenated");
  return data;
 }
