void setup(){
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX, TX
}

void loop(){
  if ( Serial2.available() ){
    Serial.write(Serial2.read());
  }
}
