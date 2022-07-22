#include "defines.h"
#include "Credentials.h"
#include <MySQL_Generic.h>
#include <TinyGPS++.h>

/*****************mysql & wifi***************/
//쿼리
char query[128];
char INSERT_DATA[] = "INSERT INTO %s.%s (time, lat, lng, meta_string, is_finish) VALUES ('%s',%f,%f,%s,%s)";

MySQL_Connection conn((Client *)&client);
MySQL_Query *query_mem;
/*********************gps*******************/

TinyGPSPlus gps;
double Latitude, longtitude;
char Date[50];

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX, TX

  /*******************mysql & wifi******************/
  while (!Serial && millis() < 5000); // wait for serial port to connect
  //보드정보 및 버전 출력
  MYSQL_DISPLAY1("\nStarting Basic_Insert_WiFi on", BOARD_NAME);
  MYSQL_DISPLAY(MYSQL_MARIADB_GENERIC_VERSION);
  MYSQL_DISPLAY1("Connecting to", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){    //네트워크(wifi) 연결되지 않을시 . 출력
    delay(500);
    MYSQL_DISPLAY0(".");
  }

  // 연결정보출력
  MYSQL_DISPLAY1("Connected to network. My IP address is:", WiFi.localIP());
  MYSQL_DISPLAY3("Connecting to SQL Server @", server, ", Port =", server_port);
  MYSQL_DISPLAY5("User =", user, ", PW =", password, ", DB =", default_database);
}

void loop() {
  MYSQL_DISPLAY("Connecting...");

  //if (conn.connect(server, server_port, user, password))
  if (conn.connectNonBlocking(server, server_port, user, password) != RESULT_FAIL){
    delay(500);
    runInsert();
    conn.close();                     // 연결 종료
  } 
  else {
    MYSQL_DISPLAY("\nConnect failed. Trying again on next iteration.");
  }

  MYSQL_DISPLAY("\nSleeping...");
  MYSQL_DISPLAY("================================================");
 
  delay(1000);
}

void runInsert(){
  // Initiate the query class instance
  MySQL_Query query_mem = MySQL_Query(&conn);

  if (conn.connected()){                              //mysql 서버와 연결되었을때
    if(Serial2.available()){
      while(gps.encode(Serial2.read())==0){
        if(gps.encode(Serial2.read())!=0)break;
      }
      Serial.print("22222");
      getGpsInfo(Serial);                          // gps정보 가져와서
      sprintf(query,INSERT_DATA,default_database, default_table,Date,Latitude,longtitude,"0","0");
      Serial.print(query);
      MYSQL_DISPLAY(query);                        // 쿼리 날리기
 
      if ( !query_mem.execute(query) ){
        MYSQL_DISPLAY("Insert error");
       }
      else{
        MYSQL_DISPLAY("Data Inserted.");
      }
    }
  }
  
  else{                                                //mysql 서버와 연결 X 떄
    MYSQL_DISPLAY("Disconnected from Server. Can't insert.");
  }
}

void getGpsInfo(Stream &serial){
  Serial.print("we are getting GPS info");
  /**********위치정보 저장***********/
  if (gps.location.isValid()){
    Latitude = gps.location.lat();
    longtitude = gps.location.lng();

    serial.print(F("Location: "));
    serial.print(Latitude);
    serial.print(longtitude);
  }
  else{
    serial.print(F("INVALID"));
  }

  /********시간정보 저장***********/
  String Mon,Day,Year,Hour,Min,Sec;
  if (gps.date.isValid()){
    Mon = gps.date.month();
    Day = gps.date.day();
    Year = gps.date.year();
    
    if (gps.time.minute() < 10){
      Hour = "0" + gps.time.hour();
    }
    else{
      Hour = gps.time.hour();
    }
    if (gps.time.second() < 10){
      Min = "0" + gps.time.minute();
    }
    else{
      Min = gps.time.minute();
    }
    if (gps.time.second() < 10){
      Sec = "0" + gps.time.second();
    }
    else{
      Sec = gps.time.second();
    }
    sprintf(Date,"%s-%s-%s %s:%s:%s",Year,Mon,Day,Hour,Min,Sec);
    serial.print(Date);
  }
  else{
    serial.print(F("INVALID"));
  }
  serial.print(F(" "));
}
