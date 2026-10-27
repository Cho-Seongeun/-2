#include <Servo.h>

// Assignment 11P07 - based on 11_example_1.ino
// Arduino pin assignment
#define PIN_LED   9   // LED active-low
#define PIN_TRIG  12  // sonar sensor TRIGGER
#define PIN_ECHO  13  // sonar sensor ECHO
#define PIN_SERVO 10  // servo motor

// configurable parameters for sonar
#define SND_VEL 346.0
#define INTERVAL 25       // sampling interval (ms), same as example
#define PULSE_DURATION 10 // trigger pulse duration (us)
#define _DIST_MIN 180.0   // mm
#define _DIST_MAX 360.0   // mm
#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)
#define _EMA_ALPHA 0.3    // starting value; tune on the actual device

// Starting values only: calibrate these for your own servo.
#define _DUTY_MIN 900
#define _DUTY_MAX 2550
#define _DUTY_NEU ((_DUTY_MIN + _DUTY_MAX) / 2)

// global variables (mm)
float dist_ema, dist_prev = _DIST_MAX;
unsigned long last_sampling_time;
bool ema_initialized = false;
Servo myservo;

float USS_measure(int TRIG, int ECHO);

void setup() {
  digitalWrite(PIN_LED, HIGH); // active-low: initially OFF
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // Calibrated pulse limits also make read() use the same angle mapping.
  myservo.attach(PIN_SERVO, _DUTY_MIN, _DUTY_MAX);
  myservo.writeMicroseconds(_DUTY_NEU);

  dist_prev = (_DIST_MIN + _DIST_MAX) / 2.0;
  dist_ema = dist_prev;
  last_sampling_time = millis();
  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;

  // Unsigned subtraction also handles millis() rollover.
  unsigned long now = millis();
  if (now - last_sampling_time < INTERVAL)
    return;
  last_sampling_time = now;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  bool valid = (dist_raw > 0.0);
  bool in_range = valid && dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX;
  digitalWrite(PIN_LED, in_range ? LOW : HIGH);

  // Range filter: distinguish missing echoes from real out-of-range echoes.
  if (!valid) {
    dist_filtered = dist_prev; // timeout: keep previous filtered distance
  } else {
    if (dist_raw < _DIST_MIN) {
      dist_filtered = _DIST_MIN;
    } else if (dist_raw > _DIST_MAX) {
      dist_filtered = _DIST_MAX;
    } else {
      dist_filtered = dist_raw;
    }
    dist_prev = dist_filtered;
  }

  if (valid) {
    // EMA: initialize with first valid sample, then smooth new samples.
    if (!ema_initialized) {
      dist_ema = dist_filtered;
      ema_initialized = true;
    } else {
      dist_ema = _EMA_ALPHA * dist_filtered
               + (1.0 - _EMA_ALPHA) * dist_ema;
    }

    // 180..360 mm -> 0..180 degrees -> calibrated pulse width.
    float ratio = (dist_ema - _DIST_MIN) / (_DIST_MAX - _DIST_MIN);
    int duty = (int)(_DUTY_MIN + ratio * (_DUTY_MAX - _DUTY_MIN) + 0.5);
    duty = constrain(duty, _DUTY_MIN, _DUTY_MAX);
    myservo.writeMicroseconds(duty);
  }
  // Timeout: EMA and servo command stay unchanged; LED is OFF.

  Serial.print("Min:");    Serial.print(_DIST_MIN);
  Serial.print(",dist:");  Serial.print(dist_raw);
  Serial.print(",ema:");   Serial.print(dist_ema);
  Serial.print(",Servo:"); Serial.print(myservo.read());
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.println("");
}

// get a distance reading from USS. Return value is in millimeters.
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
