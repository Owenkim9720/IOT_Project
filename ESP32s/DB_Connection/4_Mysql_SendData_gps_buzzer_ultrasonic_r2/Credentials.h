#ifndef Credentials_h
#define Credentials_h

char ssid[] = "-";        // your network SSID (name) 연결할 wifi 이름
char pass[] = "-";        // your network password   연결할 wifi 비밀번호

IPAddress server(-, -, -, -);     //mysql 서버 기입
char user[]         = "-";              // MySQL user login username  연결할 mysql 유저이름
char password[]     = "-";          // MySQL user login password 연결할 mysql 비밀번호
uint16_t server_port = -;    //mysql 포트기입 (default : 3306)

char default_database[] = "-";           //데이타베이스(스키마) 이름;
char default_table[]    = "-";          //테이블 이름;

#endif    //Credentials_h
