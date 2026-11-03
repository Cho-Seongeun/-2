#include <Servo.h>

#define PIN_SERVO 10

#define _DUTY_MIN 1000
#define _DUTY_NEU 1500
#define _DUTY_MAX 2000

#define _SERVO_SPEED 0.3
#define INTERVAL 20

Servo myservo;
unsigned long start_time;
unsigned long last_sampling_time;
float duty_curr;

void setup() {
  myservo.attach(PIN_SERVO);
  Serial.begin(57600);

  duty_curr = _DUTY_MAX;
  myservo.writeMicroseconds((int)duty_curr);
  delay(2000);
  start_time = millis();
  last_sampling_time = start_time;
}

void loop() {
  unsigned long time_curr = millis();

  if (time_curr - last_sampling_time < INTERVAL)
    return;
  last_sampling_time += INTERVAL;

  unsigned long elapsed = time_curr - start_time;
  if (elapsed >= 300000UL) {
    duty_curr = _DUTY_NEU;
  } else {
    duty_curr = _DUTY_MAX - (_DUTY_MAX - _DUTY_MIN)
                * (_SERVO_SPEED / 180.0) * (elapsed / 1000.0);
  }

  myservo.writeMicroseconds((int)duty_curr);
}
