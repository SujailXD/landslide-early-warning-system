/*
 * Landslide Early Warning System - WOKWI SIMULATOR VERSION
 *
 * Wokwi has no soil moisture or ball-tilt part, so:
 *   - the POTENTIOMETER stands in for the moisture sensor
 *     (turn it right = wetter soil)
 *   - the SLIDE SWITCH stands in for the tilt sensor
 *     (left = pole upright, right = pole tilted)
 * Wokwi's buzzer needs tone() to make sound; on real hardware with an
 * active buzzer, use the main landslide_warning.ino instead.
 */

const int MOISTURE_PIN = A0;
const int TILT_PIN     = 2;
const int LED_GREEN    = 8;
const int LED_YELLOW   = 9;
const int LED_RED      = 10;
const int BUZZER       = 11;

// Potentiometer: 0 = fully left (dry), 1023 = fully right (soaked)
const int DRY_VALUE = 0;
const int WET_VALUE = 1023;

const int WET_LIMIT    = 70;
const int TILT_SAMPLES = 10;
const int TILT_VOTES   = 8;

enum RiskLevel { SAFE, WARNING, DANGER };

void setup() {
  Serial.begin(9600);
  pinMode(TILT_PIN, INPUT_PULLUP);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.println(F("Landslide monitor started"));
}

int readMoisturePercent() {
  int raw = analogRead(MOISTURE_PIN);
  int pct = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
  return constrain(pct, 0, 100);
}

bool isTilted() {
  int votes = 0;
  for (int i = 0; i < TILT_SAMPLES; i++) {
    if (digitalRead(TILT_PIN) == HIGH) votes++;
    delay(20);
  }
  return votes >= TILT_VOTES;
}

void showLevel(RiskLevel level) {
  digitalWrite(LED_GREEN,  level == SAFE);
  digitalWrite(LED_YELLOW, level == WARNING);
  digitalWrite(LED_RED,    level == DANGER);

  if (level == DANGER) {
    tone(BUZZER, 1200);                 // continuous alarm
  } else if (level == WARNING) {
    tone(BUZZER, 800, 150);             // short beep
  } else {
    noTone(BUZZER);
  }
}

void loop() {
  int  moisture = readMoisturePercent();
  bool tilted   = isTilted();
  bool wet      = moisture >= WET_LIMIT;

  RiskLevel level = SAFE;
  if (wet && tilted)      level = DANGER;
  else if (wet || tilted) level = WARNING;

  const char* names[] = { "SAFE", "WARNING", "DANGER" };
  Serial.print(F("Moisture: "));  Serial.print(moisture);
  Serial.print(F("%  Tilt: "));   Serial.print(tilted ? F("YES") : F("no"));
  Serial.print(F("  ->  "));      Serial.println(names[level]);

  showLevel(level);
  delay(500);
}
