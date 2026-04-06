#include <QTRSensors.h>

// --- MOTOR PINS (ESP32 + TB6612FNG) ---
// Motor A (Left Motor)
const int PWMA = 18;
const int AIN1 = 19;
const int AIN2 = 21;

// Motor B (Right Motor)
const int PWMB = 5;
const int BIN1 = 16;
const int BIN2 = 17;

// System Control
const int STBY = 23;

// --- SENSOR OBJECT & VARIABLES ---
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
uint16_t digitalValues[SensorCount];
uint16_t lastPosition = 3500; // Center position
int THRESHOLD = 500;          // Auto-threshold for line detection

// --- PID VARIABLES ---
float Kp = 0.04;
float Kd = 0.1;
int P, D;
int previousError = 0;

// --- MOTOR SETTINGS ---
const int baseSpeed = 80;
const int maxSpeed = 170;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize Motor Driver Pins
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Enable motor driver
  digitalWrite(STBY, HIGH);

  // Initialize Sensor Pins
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){36, 39, 34, 35, 32, 33, 25, 26}, SensorCount);

  // Calibration Sequence
  Serial.println("Calibrating... SWEEP THE ROBOT OVER THE LINE!");
  pinMode(2, OUTPUT);   // Built-in LED for calibration indicator
  digitalWrite(2, HIGH);

  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(20); // Small delay to allow sweeping
  }

  digitalWrite(2, LOW); // Turn off LED when calibration is done
  Serial.println("Calibration complete! Starting in 3 seconds...");
  delay(3000);
}

// --- HELPER FUNCTION FOR MOTOR CONTROL ---
void setMotors(int leftSpeed, int rightSpeed) {
  // Motor A (Left)
  if (leftSpeed >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, leftSpeed);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(PWMA, -leftSpeed);
  }

  // Motor B (Right)
  if (rightSpeed >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, rightSpeed);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    analogWrite(PWMB, -rightSpeed);
  }
}

void loop() {
  // --- 1. READ SENSORS & CALCULATE POSITION ---
  qtr.readCalibrated(sensorValues);

  long weighted_sum = 0;
  int sum = 0;

  for (uint8_t i = 0; i < SensorCount; i++) {
    int digitalValue = (sensorValues[i] > THRESHOLD) ? 1 : 0;
    digitalValues[i] = digitalValue;
    weighted_sum += (long)digitalValue * (i * 1000);
    sum += digitalValue;
  }

  uint16_t position = (sum == 0) ? lastPosition : weighted_sum / sum;
  lastPosition = position;

  // --- 2. CALCULATE PID ---
  int error = 3500 - position;
  P = error;
  D = error - previousError;
  int PIDvalue = (Kp * P) + (Kd * D);
  previousError = error;

  // --- 3. APPLY TO MOTORS ---
  int leftMotorSpeed = constrain(baseSpeed - PIDvalue, 0, maxSpeed);
  int rightMotorSpeed = constrain(baseSpeed + PIDvalue, 0, maxSpeed);

  // Optional debugging output
  // Serial.print(position); Serial.print(" | L: "); Serial.print(leftMotorSpeed);
  // Serial.print(" R: "); Serial.println(rightMotorSpeed);

  setMotors(leftMotorSpeed, rightMotorSpeed);

  // Loop delay for stability
  delay(5);
}
