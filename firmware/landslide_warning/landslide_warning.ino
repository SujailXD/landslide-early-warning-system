/*
 * Low-Cost Landslide Early Warning System
 * Sensors : capacitive soil moisture sensor (A0), SW-520D tilt switch (D2)
 * Outputs : green / yellow / red LEDs (D8-D10), active buzzer (D11)
 *
 * SAFE    = soil not saturated AND pole upright      -> green
 * WARNING = soil saturated OR pole tilted            -> yellow + short beeps
 * DANGER  = soil saturated AND pole tilted           -> red + continuous alarm
 */

// ---------- Pin map ----------
const int MOISTURE_PIN = A0;
const int TILT_PIN     = 2;
const int LED_GREEN    = 8;
const int LED_YELLOW   = 9;
const int LED_RED      = 10;
const int BUZZER       = 11;

// ---------- Calibration ----------
// Open the Serial Monitor (9600 baud) and note "Raw moisture":
//   probe in dry air        -> put that number in DRY_VALUE
//   probe in glass of water -> put that number in WET_VALUE
const int DRY_VALUE = 520;
const int WET_VALUE = 260;

const int WET_LIMIT    = 70;  // % moisture treated as saturated soil
const int TILT_SAMPLES = 10;  // readings per check (debounce)
const int TILT_VOTES   = 8;   // how many must say "tilted"

enum RiskLevel { SAFE, WARNING, DANGER };

void setup() {
  Serial.begin(9600);
  pinMode(TILT_PIN, INPUT_PULLUP);   // upright = LOW, tilted = HIGH
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.println(F("Landslide monitor started"));
}

int readMoisturePercent() {
  int raw = analogRead(MOISTURE_PIN);
  Serial.print(F("Raw moisture: "));
  Serial.print(raw);
  int pct = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
  return constrain(pct, 0, 100);
}

bool isTilted() {
  int votes = 0;
  for (int i = 0; i < TILT_SAMPLES; i++) {
    // Ball off the contacts -> circuit open -> pull-up reads HIGH.
    // If your sensor reports the opposite, change HIGH to LOW here.
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
    digitalWrite(BUZZER, HIGH);          // continuous alarm
  } else if (level == WARNING) {
    digitalWrite(BUZZER, HIGH);          // short beep
    delay(150);
    digitalWrite(BUZZER, LOW);
  } else {
    digitalWrite(BUZZER, LOW);
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
  Serial.print(F("  Moisture: ")); Serial.print(moisture);
  Serial.print(F("%  Tilt: "));    Serial.print(tilted ? F("YES") : F("no"));
  Serial.print(F("  ->  "));       Serial.println(names[level]);

  showLevel(level);
  delay(500);
}
