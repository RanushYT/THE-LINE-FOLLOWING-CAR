#include <QTRSensors.h>
#include <BluetoothSerial.h> // Bluetooth library for ESP32

// --- BLUETOOTH OBJECT & VARIABLES ---
BluetoothSerial SerialBT;
String btCommand = "";
bool isRunning = false; // Controls Start/Stop state

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
int THRESHOLD = 500;           // Midpoint for line detection

// --- PID VARIABLES ---
float Kp = 0.04;
float Kd = 0.1;
int previousError = 0;
int P, D;

// --- MOTOR SETTINGS ---
int baseSpeed = 80;      // Can be updated via Bluetooth
const int maxSpeed = 170;

void setup() {
  Serial.begin(115200);

  // Initialize Bluetooth
  SerialBT.begin("ESP32_LineFollower");
  Serial.println("Bluetooth Started! Ready to pair.");
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
  Serial.println("Calibrating... Sweep the robot over the line!");
  pinMode(2, OUTPUT); // Built-in LED to indicate calibration
  digitalWrite(2, HIGH);

  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(20);
  }

  digitalWrite(2, LOW);
  Serial.println("Calibration complete! Waiting for START command via Bluetooth...");
}

// --- MOTOR CONTROL FUNCTION ---
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

// --- BLUETOOTH COMMAND PROCESSING ---
void processBluetoothCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "START") {
    isRunning = true;
    SerialBT.println("Robot STARTED");
  } else if (cmd == "STOP") {
    isRunning = false;
    SerialBT.println("Robot STOPPED");
  } else if (cmd.startsWith("KP:")) {
    Kp = cmd.substring(3).toFloat();
    SerialBT.print("Kp updated to: ");
    SerialBT.println(Kp, 4);
  } else if (cmd.startsWith("KD:")) {
    Kd = cmd.substring(3).toFloat();
    SerialBT.print("Kd updated to: ");
    SerialBT.println(Kd, 4);
  } else if (cmd.startsWith("SPEED:")) {
    baseSpeed = cmd.substring(6).toInt();
    SerialBT.print("Base Speed updated to: ");
    SerialBT.println(baseSpeed);
  }
}

void loop() {
  // --- Read Bluetooth commands ---
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n' || c == '\r') {
      if (btCommand.length() > 0) {
        processBluetoothCommand(btCommand);
        btCommand = "";
      }
    } else {
      btCommand += c;
    }
  }

  // --- Run PID logic only if robot is started ---
  if (isRunning) {
    // Read sensors
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

    // PID calculation
    int error = 3500 - position;
    P = error;
    D = error - previousError;
    int PIDvalue = (Kp * P) + (Kd * D);
    previousError = error;

    // Apply to motors
    int leftMotorSpeed = constrain(baseSpeed - PIDvalue, 0, maxSpeed);
    int rightMotorSpeed = constrain(baseSpeed + PIDvalue, 0, maxSpeed);
    setMotors(leftMotorSpeed, rightMotorSpeed);

  } else {
    // Stop motors if not running
    setMotors(0, 0);
  }

  delay(5); // Stability
}
