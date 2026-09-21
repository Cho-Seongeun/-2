const int ledPin = 7;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 1. 처음 1초 동안 LED 켜기 (Pin 7을 LOW로 설정해야 켜짐)
  digitalWrite(ledPin, LOW);
  delay(1000);

  // 2. 다음 1초 동안 LED 5회 깜빡이기
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, LOW);   // 켜짐
    delay(100);
    digitalWrite(ledPin, HIGH);  // 꺼짐
    delay(100);
  }

  // 3. LED 끄기 및 무한 루프 (Pin 7을 HIGH로 설정해야 꺼짐)
  digitalWrite(ledPin, HIGH);
  while (1) {
  }
}