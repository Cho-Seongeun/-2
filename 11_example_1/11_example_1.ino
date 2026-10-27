#include <Servo.h>


#define PIN_LED   9  
#define PIN_TRIG  12 
#define PIN_ECHO  13 
#define PIN_SERVO 10  


#define SND_VEL 346.0
#define INTERVAL 25      
#define PULSE_DURATION 10 
#define _DIST_MIN 180.0   
#define _DIST_MAX 360.0   
#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)
#define _EMA_ALPHA 0.3  

#define _DUTY_MIN 900
#define _DUTY_MAX 2550
#define _DUTY_NEU ((_DUTY_MIN + _DUTY_MAX) / 2)

float dist_ema, dist_prev = _DIST_MAX;
unsigned long last_sampling_time;
bool ema_initialized = false;
Servo myservo;

float USS_measure(int TRIG, int ECHO);

void setup() {
  digitalWrite(PIN_LED, HIGH); 
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO, _DUTY_MIN, _DUTY_MAX);
  myservo.writeMicroseconds(_DUTY_NEU);

  dist_prev = (_DIST_MIN + _DIST_MAX) / 2.0;
  dist_ema = dist_prev;
  last_sampling_time = millis();
  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;

  unsigned long now = millis();
  if (now - last_sampling_time < INTERVAL)
    return;
  last_sampling_time = now;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  bool valid = (dist_raw > 0.0);
  bool in_range = valid && dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX;
  digitalWrite(PIN_LED, in_range ? LOW : HIGH);

  if (!valid) {
    dist_filtered = dist_prev;
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
    if (!ema_initialized) {
      dist_ema = dist_filtered;
      ema_initialized = true;
    } else {
      dist_ema = _EMA_ALPHA * dist_filtered
               + (1.0 - _EMA_ALPHA) * dist_ema;
    }

    float ratio = (dist_ema - _DIST_MIN) / (_DIST_MAX - _DIST_MIN);
    int duty = (int)(_DUTY_MIN + ratio * (_DUTY_MAX - _DUTY_MIN) + 0.5);
    duty = constrain(duty, _DUTY_MIN, _DUTY_MAX);
    myservo.writeMicroseconds(duty);
  }

  Serial.print("Min:");    Serial.print(_DIST_MIN);
  Serial.print(",dist:");  Serial.print(dist_raw);
  Serial.print(",ema:");   Serial.print(dist_ema);
  Serial.print(",Servo:"); Serial.print(myservo.read());
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.println("");
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
