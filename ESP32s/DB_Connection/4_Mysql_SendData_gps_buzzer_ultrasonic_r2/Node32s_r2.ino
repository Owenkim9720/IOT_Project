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
/*************Ultrasonic & Buzzer*******************/
const int ECHO = 18;
const int TRIG = 19;

#define BUZ_PIN 15
int channel_flag = 0;
int freq = 5000;
int buzChannel = 0;
int resolution = 0;
long distance;
int gpsflag = 0;
int timeflag =0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX, TX
  /*******************mysql & wifi******************/
  delay(2000);
  //보드정보 및 버전 출력
  MYSQL_DISPLAY1("\nStarting Basic_Insert_WiFi on", BOARD_NAME);
  MYSQL_DISPLAY(MYSQL_MARIADB_GENERIC_VERSION);
  MYSQL_DISPLAY1("Connecting to", ssid);
  
  for(int i=0;i<5;i++){
    WiFi.begin(ssid, pass);
    int wificnt = 0;
    while (WiFi.status() != WL_CONNECTED){    //네트워크(wifi) 연결되지 않을시 . 출력
      delay(500);
      MYSQL_DISPLAY0(".");
      wificnt++;
      if(wificnt>10) break;
    }
    Warn(2);
  }

  // 연결정보출력
  MYSQL_DISPLAY1("Connected to network. My IP address is:", WiFi.localIP());
  MYSQL_DISPLAY3("Connecting to SQL Server @", server, ", Port =", server_port);
  MYSQL_DISPLAY5("User =", user, ", PW =", password, ", DB =", default_database);
  /*****************Ultrasonic & Buzzer****************/
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  ledcSetup(buzChannel, freq, resolution);
  ledcAttachPin(BUZ_PIN, buzChannel);
}

void loop() {
  MYSQL_DISPLAY("Connecting...");

  //if (conn.connect(server, server_port, user, password))
  if (conn.connectNonBlocking(server, server_port, user, password) != RESULT_FAIL){
    delay(300);
    runInsert();
    conn.close();                     // 연결 종료
  } 
  else {
    MYSQL_DISPLAY("\nConnect failed. Trying again on next iteration.");
  }

  MYSQL_DISPLAY("\nSleeping...");
  MYSQL_DISPLAY("================================================");

/*****************Ultrasonic & Buzzer****************/
  long duration;
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = ((float)(340 * duration) / 1000)/2;

  if (distance<100){
    Warn(3);
  }
  delay(100);
}

void runInsert(){
  // Initiate the query class instance
  MySQL_Query query_mem = MySQL_Query(&conn);

  if (conn.connected()){                              //mysql 서버와 연결되었을때
    if(Serial2.available()){

      int errorShiedcnt = 0;
      while(gps.encode(Serial2.read())==0){
        if(gps.encode(Serial2.read())!=0)break;
        errorShiedcnt++;
        if(errorShiedcnt>100000)break;
      }
      
      getGpsInfo(Serial);                          // gps정보 가져와서
      char str_dis[30];
      sprintf(str_dis,"%ld",distance); 
      
      if(gpsflag==1 || timeflag==1){
         MYSQL_DISPLAY("Insert error");
         Warn(1);
      }
      
      else{ 
        sprintf(query,INSERT_DATA,default_database, default_table,Date,Latitude,longtitude,str_dis,"0");
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
  }
  
  else{                                                //mysql 서버와 연결 X 떄
    MYSQL_DISPLAY("Disconnected from Server. Can't insert.");
  }
}

void getGpsInfo(Stream &serial){
  /**********위치정보 저장***********/
  if (gps.location.isValid()){
    Latitude = gps.location.lat();
    longtitude = gps.location.lng();

    serial.print(F("Location: "));
    serial.print(Latitude);
    serial.print(" ");
    serial.print(longtitude);
    timeflag = 0;
  }
  else{
    Latitude = 0;
    longtitude = 0;
    serial.print(F("INVALID"));
    timeflag = 1;
  }

  /********시간정보 저장***********/
  String Mon,Day,Year,Hour,Min,Sec;
  String base = "0";
  if (gps.date.isValid()){
    Mon = gps.date.month();
    Day = gps.date.day();
    Year = gps.date.year();
    
    if (gps.time.minute() < 10){
      Hour = base + gps.time.hour();
    }
    else{
      Hour = gps.time.hour();
    }
    
    if (gps.time.second() < 10){
      Min = base + gps.time.minute();
    }
    else{
      Min = gps.time.minute();
    }
    
    if (gps.time.second() < 10){
      Sec = base + gps.time.second();
    }
    
    else{
      Sec = gps.time.second();
    }
    sprintf(Date,"%s-%s-%s %s:%s:%s",Year,Mon,Day,Hour,Min,Sec);
    gpsflag = 0;
    serial.print(Date);
  }
  else{
    gpsflag = 1;
    sprintf(Date,"0000-00-00 00:00:00");
  }
  serial.print(F(" "));
}
void Warn(int lo){
  for(int i=0;i<lo;i++){
      ledcWriteTone(buzChannel,500);
      delay(500);
      ledcWriteTone(buzChannel,0);
      delay(200);
      //ledcWriteNote(buzChannel,NOTE_D,4);
  }
}
