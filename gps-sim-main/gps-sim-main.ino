
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <String.h>
#include <limits.h>

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

//String data;
//String latitude;
//String longitude;
//String alti;
//String curDate;
//String sat;
//String curTime;


void simSetup(){

  Serial.println("setting up your sim");
//  enter GPRS configuration mode
  Sim800l.write("a\r\n");
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


String getGPSData()
{

  static double latitude = LONG_MIN;
  static double longitude = LONG_MIN;
  static double alti = LONG_MIN;
  
  if (gps.location.isValid())
  {
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

  

  String data = String("{latitude:") + latitude + ",longitude" + longitude + ",altitude:" + alti + 
  ",date_month:" + date_month + ",date_day:" + date_day + ",date_year:" + date_year  + ",satellites:" + sat + 
  ",time_hour:" + time_hour + ",time_minute:" + time_minute + ",time_second:" + time_second +"}";

  Serial.print("gps data concatenated");
  return data;
 }


void sendData(String data){

  String datanew("{\"data\": " + data + "}");
    
  Serial.println(datanew);
  Serial.println("posting request\n\n");

  Sim800l.write("AT+CIPSTART=\"TCP\",\"31.171.250.88\",8080");
  Sim800l.write("WAIT=3");


  String request("\nPOST / HTTP/1.1\nHost: 31.171.250.88:8080\nContent-Type: application/json\nContent-Length:" + datanew.length() + String("\n"));
  Serial.println(request.length());

  String frequest("CIPSEND=" + request.length() + request);
  Serial.println(request);
  Serial.println(frequest);

  
  Sim800l.write(frequest.c_str());

  Serial.println("********************************");

}



void simSleepModeOff(){
  Sim800l.write("AT\r\n");
  Sim800l.write("AT+CSCLK=0'\r\n");
  delay(5000);
}

void simSleepModeOn(){
  Sim800l.write("AT+CSCLK=2'\r\n");
  delay(5000);
}




 
void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 Sim800l.begin(9600);
 gpsSerial.begin(9600);
 
 Serial.println("Start Program");

 //simSetup();
 
}


void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0){
    if (gps.encode(gpsSerial.read()))
      Serial.println(getGPSData());
  }

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }

    delay(10000);

}
