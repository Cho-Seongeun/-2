#define LED_PIN 7

int period = 1000;
int duty = 0;

void set_period(int p)
{
  if (p < 100) {
    p = 100;
  }

  if (p > 10000) {
    p = 10000;
  }

  period = p;
}

void set_duty(int d)
{
  if (d < 0) {
    d = 0;
  }

  if (d > 100) {
    d = 100;
  }

  duty = d;

  unsigned long onTime;
  unsigned long offTime;

  onTime = (unsigned long)period * duty / 100;
  offTime = period - onTime;


  if (onTime > 0) {
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(onTime);
  }

  if (offTime > 0) {
    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(offTime);
  }
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);

  set_period(100);      // 1 ms
  // 100 -> 1000, 10000으로 바꾸어 각각 확인하면 된다!

}

void loop()
{
  int repeatCount;

  repeatCount = 5000 / period;

  if (repeatCount < 1) {
    repeatCount = 1;
  }

  // 최소 밝기 -> 최대 밝기
  for (int d = 0; d <= 100; d++) {
    for (int i = 0; i < repeatCount; i++) {
      set_duty(d);
    }
  }

  // 최대 밝기 -> 최소 밝기
  for (int d = 99; d >= 0; d--) {
    for (int i = 0; i < repeatCount; i++) {
      set_duty(d);
    }
  }
}
