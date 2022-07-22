#include <TinyGPS++.h>

TinyGPSPlus gps;

void setup(){
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX, TX
}

void loop(){
  if ( Serial2.available() ){
    if (gps.encode(Serial2.read())){
      displayInfo(Serial);
    }
  }
}

void displayInfo(Stream &serial){
  //현재 위치의 위도와 경도 출력
  serial.print(F("Location: ")); 
  if (gps.location.isValid()){
    serial.print(gps.location.lat(), 6);
    serial.print(F(","));
    serial.print(gps.location.lng(), 6);
  }
  else{
    serial.print(F("INVALID"));
  }
  
  //현재 날짜 정보 출력
  serial.print(F("  Date/Time: "));
  if (gps.date.isValid()){
    serial.print(gps.date.month());
    serial.print(F("/"));
    serial.print(gps.date.day());
    serial.print(F("/"));
    serial.print(gps.date.year());
  }
  else{
    serial.print(F("INVALID"));
  }
  serial.print(F(" "));

  //현재 시간 정보 출력
  if (gps.time.isValid()){
    if (gps.time.hour() < 10) serial.print(F("0"));
    serial.print(gps.time.hour());
    serial.print(F(":"));
    if (gps.time.minute() < 10) serial.print(F("0"));
    serial.print(gps.time.minute());
    serial.print(F(":"));
    if (gps.time.second() < 10) serial.print(F("0"));
    serial.print(gps.time.second());
    serial.print(F("."));
    if (gps.time.centisecond() < 10) serial.print(F("0"));
    serial.print(gps.time.centisecond());
  }
  else{
    serial.print(F("INVALID"));
  }
  serial.println();
}
