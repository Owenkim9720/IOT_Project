const int ECHO = 18;
const int TRIG = 19;

void setup() {
  Serial.begin(115200);

  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
}

void loop() {
  long duration, distance;
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = ((float)(340 * duration) / 1000)/2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("mm");
  delay(100);
}
