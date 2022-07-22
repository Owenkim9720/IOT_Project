const int ECHO = 18;
const int TRIG = 19;

#define BUZ_PIN 15

int channel_flag = 0;
int freq = 5000;
int buzChannel = 0;
int resolution = 0;

void setup(){
  Serial.begin(115200);
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  
  ledcSetup(buzChannel, freq, resolution);
  ledcAttachPin(BUZ_PIN, buzChannel);
}

void loop(){
  long duration, distance;
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = ((float)(340 * duration) / 1000)/2;

  if (distance<100){
    for(int i=0;i<3;i++){
      ledcWriteTone(buzChannel,500);
      delay(500);
      ledcWriteTone(buzChannel,0);
      delay(200);
      //ledcWriteNote(buzChannel,NOTE_D,4);
    }
  }
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("mm");
  delay(100);
}
