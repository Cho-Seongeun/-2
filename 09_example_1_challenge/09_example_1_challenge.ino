
#define PIN_LED 9
#define PIN_TRIG 12
#define PIN_ECHO 13

#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100
#define _DIST_MAX 300

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.5

#define N 3
// 여기서 N을 3, 10, 30로 각각 변화시키면 된다.

unsigned long last_sampling_time = 0;

float dist_raw = 0;
float dist_ema = 0;
float dist_median = 0;

// Store the latest N samples
float samples[N];
int sample_index = 0;
int sample_count = 0;

float USS_measure(int TRIG, int ECHO);

// Calculate median
float getMedian()
{
  float temp[N];

  // Copy samples
  for (int i = 0; i < sample_count; i++) {
    temp[i] = samples[i];
  }

  // Sort in ascending order
  for (int i = 0; i < sample_count - 1; i++) {
    for (int j = i + 1; j < sample_count; j++) {
      if (temp[i] > temp[j]) {
        float t = temp[i];
        temp[i] = temp[j];
        temp[j] = t;
      }
    }
  }

  // Return median
  if (sample_count % 2 == 1) {
    return temp[sample_count / 2];
  }
  else {
    return (temp[sample_count / 2 - 1]
          + temp[sample_count / 2]) / 2.0;
  }
}

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop()
{
  if (millis() - last_sampling_time < INTERVAL)
    return;

  last_sampling_time = millis();

  // Measure raw distance
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // EMA filter
  dist_ema = _EMA_ALPHA * dist_raw
           + (1.0 - _EMA_ALPHA) * dist_ema;

  // Store raw sample (including invalid values)
  samples[sample_index] = dist_raw;

  sample_index = (sample_index + 1) % N;

  if (sample_count < N) {
    sample_count++;
  }

  // Median filter
  dist_median = getMedian();

  // Serial plotter output
  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",raw:");
  Serial.print(dist_raw);

  Serial.print(",ema:");
  Serial.print(dist_ema);

  Serial.print(",median:");
  Serial.print(dist_median);

  Serial.print(",Max:");
  Serial.print(_DIST_MAX);

  Serial.println();
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
