void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);           //  setup serial
}

void loop() {
  // put your main code here, to run repeatedly:
    unsigned int x = analogRead(0);
    unsigned int t = millis();
    Serial.print(t);
    Serial.print(',');

    Serial.println(x);



}
