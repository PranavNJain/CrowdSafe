/*
  CrowdSafe — Stampede Early Warning System
  Arduino Mega 2560

  Sensors  : 3× HC-SR04 (ultrasonic), 1× MPU-6050 (accelerometer/gyro)
  Output   : Buzzer, RGB LED, 16×2 LCD, Serial monitor
  Algorithm: Crowd Pressure Index (CPI)

  CPI Formula:
    CPI = (w1 × DensityScore) + (w2 × MovementScore) + (w3 × ShockScore)
    where w1=0.45, w2=0.35, w3=0.20

  Alert Levels:
    CPI < 40   → NORMAL  (Green LED)
    40 ≤ CPI < 70 → WARNING (Yellow LED + slow beep)
    CPI ≥ 70   → DANGER  (Red LED + rapid beep + LCD alert)
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>

// ── Pin Definitions ────────────────────────────────────────────────────
#define TRIG1 2
#define ECHO1 3
#define TRIG2 4
#define ECHO2 5
#define TRIG3 6
#define ECHO3 7

#define LED_RED   8
#define LED_GREEN 9
#define LED_BLUE  10
#define BUZZER    11

// ── Constants ──────────────────────────────────────────────────────────
#define SENSOR_SPACING_CM  50.0   // Physical spacing between sensors (cm)
#define MAX_DENSITY_DIST   80.0   // Distance (cm) below which area is "dense"
#define SHOCK_THRESHOLD    1.8    // Accel magnitude (g) to trigger shock detection

// CPI weights
const float W_DENSITY  = 0.45;
const float W_MOVEMENT = 0.35;
const float W_SHOCK    = 0.20;

// ── Globals ────────────────────────────────────────────────────────────
LiquidCrystal_I2C lcd(0x27, 16, 2);
MPU6050 mpu;

float prevDistance1 = 0, prevDistance2 = 0, prevDistance3 = 0;
unsigned long lastReadTime = 0;

// ── Setup ──────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // Ultrasonic pins
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT); pinMode(ECHO3, INPUT);

  // Output pins
  pinMode(LED_RED,   OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE,  OUTPUT);
  pinMode(BUZZER,    OUTPUT);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  CrowdSafe v1  ");
  lcd.setCursor(0, 1);
  lcd.print(" Initializing.. ");
  delay(2000);
  lcd.clear();

  // MPU-6050
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("[ERROR] MPU-6050 not found.");
  } else {
    Serial.println("[INFO] MPU-6050 connected.");
  }

  Serial.println("[INFO] CrowdSafe system started.");
}

// ── Main Loop ──────────────────────────────────────────────────────────
void loop() {
  // Read sensors
  float d1 = readUltrasonic(TRIG1, ECHO1);
  float d2 = readUltrasonic(TRIG2, ECHO2);
  float d3 = readUltrasonic(TRIG3, ECHO3);
  float accelMag = readAccelMagnitude();

  // Compute individual scores (0–100 scale)
  float densityScore  = computeDensityScore(d1, d2, d3);
  float movementScore = computeMovementScore(d1, d2, d3);
  float shockScore    = computeShockScore(accelMag);

  // Compute CPI
  float cpi = (W_DENSITY  * densityScore)
            + (W_MOVEMENT * movementScore)
            + (W_SHOCK    * shockScore);
  cpi = constrain(cpi, 0, 100);

  // Determine alert level
  String level = getAlertLevel(cpi);

  // Update outputs
  updateLED(level);
  updateBuzzer(level);
  updateLCD(cpi, level);

  // Log to serial
  Serial.print("[CPI] ");  Serial.print(cpi, 1);
  Serial.print("  D:");   Serial.print(densityScore, 1);
  Serial.print("  M:");   Serial.print(movementScore, 1);
  Serial.print("  S:");   Serial.print(shockScore, 1);
  Serial.print("  => ");  Serial.println(level);

  // Store distances for movement delta next cycle
  prevDistance1 = d1;
  prevDistance2 = d2;
  prevDistance3 = d3;

  delay(500);
}

// ── Sensor Readers ─────────────────────────────────────────────────────
float readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  return (duration == 0) ? 200.0 : (duration * 0.034 / 2.0);
}

float readAccelMagnitude() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float axG = ax / 16384.0;
  float ayG = ay / 16384.0;
  float azG = az / 16384.0;
  return sqrt(axG*axG + ayG*ayG + azG*azG);
}

// ── Score Computations ─────────────────────────────────────────────────
float computeDensityScore(float d1, float d2, float d3) {
  float avg = (d1 + d2 + d3) / 3.0;
  return map(constrain(avg, 0, MAX_DENSITY_DIST), MAX_DENSITY_DIST, 0, 0, 100);
}

float computeMovementScore(float d1, float d2, float d3) {
  float delta = abs(d1 - prevDistance1) + abs(d2 - prevDistance2) + abs(d3 - prevDistance3);
  return constrain(delta * 5.0, 0, 100);
}

float computeShockScore(float mag) {
  if (mag < SHOCK_THRESHOLD) return 0.0;
  return constrain((mag - SHOCK_THRESHOLD) * 60.0, 0, 100);
}

// ── Alert Level ────────────────────────────────────────────────────────
String getAlertLevel(float cpi) {
  if (cpi >= 70) return "DANGER";
  if (cpi >= 40) return "WARNING";
  return "NORMAL";
}

// ── Output Handlers ────────────────────────────────────────────────────
void updateLED(String level) {
  digitalWrite(LED_RED,   level == "DANGER");
  digitalWrite(LED_GREEN, level == "NORMAL");
  digitalWrite(LED_BLUE,  level == "WARNING");
}

void updateBuzzer(String level) {
  if (level == "DANGER") {
    tone(BUZZER, 1000, 200);
    delay(300);
    tone(BUZZER, 1000, 200);
  } else if (level == "WARNING") {
    tone(BUZZER, 600, 400);
  } else {
    noTone(BUZZER);
  }
}

void updateLCD(float cpi, String level) {
  lcd.setCursor(0, 0);
  lcd.print("CPI: ");
  lcd.print(cpi, 1);
  lcd.print("      ");

  lcd.setCursor(0, 1);
  if (level == "DANGER")  lcd.print("!! DANGER !!    ");
  else if (level == "WARNING") lcd.print("CAUTION: WARNING");
  else                         lcd.print("Status: NORMAL  ");
}
