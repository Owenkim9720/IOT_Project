/*********************************************************************************************************************************
  Basic_Insert_WiFi.ino

  Library for communicating with a MySQL or MariaDB Server

  Based on and modified from Dr. Charles A. Bell's MySQL_Connector_Arduino Library https://github.com/ChuckBell/MySQL_Connector_Arduino
  to support nRF52, SAMD21/SAMD51, SAM DUE, STM32F/L/H/G/WB/MP1, ESP8266, ESP32, etc. boards using W5x00, ENC28J60, LAM8742A Ethernet,
  WiFiNINA, ESP-AT, built-in ESP8266/ESP32 WiFi.

  The library provides simple and easy Client interface to MySQL or MariaDB Server.

  Built by Khoi Hoang https://github.com/khoih-prog/MySQL_MariaDB_Generic
  Licensed under MIT license
 **********************************************************************************************************************************/
/*
  MySQL Connector/Arduino Example : basic insert

  This example demonstrates how to issue an INSERT query to store data in a
  table. For this, we will create a special database and table for testing.
  The following are the SQL commands you will need to run in order to setup
  your database for running this sketch.

  CREATE DATABASE test_arduino;
  CREATE TABLE test_arduino.hello_arduino (
    num integer primary key auto_increment,
    message char(40),
    recorded timestamp
  );

  Here we see one database and a table with three fields; a primary key that
  is an auto_increment, a string, and a timestamp. This will demonstrate how
  to save a date and time of when the row was inserted, which can help you
  determine when data was recorded or updated.

  For more information and documentation, visit the wiki:
  https://github.com/ChuckBell/MySQL_Connector_Arduino/wiki.

  INSTRUCTIONS FOR USE

  1) Create the database and table as shown above.
  2) Change the address of the server to the IP address of the MySQL server
  3) Change the user and password to a valid MySQL user and password
  4) Connect a USB cable to your Arduino
  5) Select the correct board and port
  6) Compile and upload the sketch to your Arduino
  7) Once uploaded, open Serial Monitor (use 115200 speed) and observe
  8) After the sketch has run for some time, open a mysql client and issue
     the command: "SELECT * FROM test_arduino.hello_arduino" to see the data
     recorded. Note the field values and how the database handles both the
     auto_increment and timestamp fields for us. You can clear the data with
     "DELETE FROM test_arduino.hello_arduino".

  Note: The MAC address can be anything so long as it is unique on your network.

  Created by: Dr. Charles A. Bell
*/
#include "defines.h"
#include "Credentials.h"
#include <MySQL_Generic.h>

#define USING_HOST_NAME     false  //HOST NAME이 있을 시 True (ex : www.naver.com)

#if USING_HOST_NAME
  char server[] = "your_account.ddns.net"; //서버의 호스트 네임 ex : www.naver.com
#else
  IPAddress server(-, -, -, -);     //mysql 서버 기입 (ex : 23,234,23,234)
#endif

uint16_t server_port = 3306;    //mysql 포트기입 (default : 3306)

char default_database[] = "-";           //데이타베이스(스키마) 이름;
char default_table[]    = "-";          //테이블 이름;
String default_value    = "Hello, Arduino!";     //저장할 샘플 메세지

// 샘플 쿼리
String INSERT_SQL = String("INSERT INTO ") + default_database + "." + default_table 
                 + " (message) VALUES ('" + default_value + "')";

MySQL_Connection conn((Client *)&client);
MySQL_Query *query_mem;

void setup(){
  Serial.begin(115200);
  while (!Serial && millis() < 5000); // wait for serial port to connect
  
  //보드정보 및 버전 출력
  MYSQL_DISPLAY1("\nStarting Basic_Insert_WiFi on", BOARD_NAME);
  MYSQL_DISPLAY(MYSQL_MARIADB_GENERIC_VERSION);

//***************Remember to initialize your WiFi module***********************
#if ( USING_WIFI_ESP8266_AT  || USING_WIFIESPAT_LIB ) 
  #if ( USING_WIFI_ESP8266_AT )
    MYSQL_DISPLAY("Using ESP8266_AT/ESP8266_AT_WebServer Library");
  #elif ( USING_WIFIESPAT_LIB )
    MYSQL_DISPLAY("Using WiFiEspAT Library");
  #endif
  
  // initialize serial for ESP module
  EspSerial.begin(115200);
  // initialize ESP module
  WiFi.init(&EspSerial);

  MYSQL_DISPLAY(F("WiFi shield init done"));

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD)
  {
    MYSQL_DISPLAY(F("WiFi shield not present"));
    // don't continue
    while (true);
  }
#endif
//******************************************************************************

  // Begin WiFi section
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

void runInsert(){
  // Initiate the query class instance
  MySQL_Query query_mem = MySQL_Query(&conn);

  if (conn.connected()){                              //mysql 서버와 연결되었을때
    MYSQL_DISPLAY(INSERT_SQL);
    
    // Execute the query
    // KH, check if valid before fetching
    if ( !query_mem.execute(INSERT_SQL.c_str()) ){
      MYSQL_DISPLAY("Insert error");
    }
    else{
      MYSQL_DISPLAY("Data Inserted.");
    }
  }
  
  else{                                                //mysql 서버와 연결 X 떄
    MYSQL_DISPLAY("Disconnected from Server. Can't insert.");
  }
}

void loop(){
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
 
  delay(60000);
}
