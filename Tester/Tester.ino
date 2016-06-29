void setup() {
  // put your setup code here, to run once:
pinMode(6,OUTPUT);
pinMode(7,OUTPUT);
}

void loop() {
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
  delay(2000);
  digitalWrite(7,LOW);
  delay(2000);

}
