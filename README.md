## 🧠 Design Decisions

Based on the knowledge and practical skills gained during the learning phase, the robot was designed using a structured engineering approach.

Key areas considered:
- Motor control  
- Sensor integration  
- Voltage regulation  
- Control algorithms  

Different design options were evaluated, and components were selected to match the required performance of the robot in terms of speed, stability, and accuracy.

## ⚙️ Microcontroller & Motor Selection

### 🔹 ESP32 vs Arduino
The ESP32 development board was selected instead of a standard Arduino due to:

- Faster processing speed  
- More memory  
- Built-in Wi-Fi and Bluetooth  

These features allow faster sensor readings and more precise motor control.

---

### 🔹 Motor Selection (600 RPM N20)

Motor speed plays a critical role in robot performance:

- High RPM → Faster movement but less stability (may overshoot turns)  
- Low RPM → Better control but slower performance  

After testing and evaluating track conditions, **600 RPM N20 gear motors** were selected to balance speed and stability.

If required, speed can be further adjusted through software (PWM control).

## 🔌 Components Used

- ESP32 Development Board  
- TB6612FNG Motor Driver  
- 600 RPM N20 Gear Motors (x2)  
- LM2596 Buck Converter  
- 8-Channel IR Sensor Array  
- 3.7V Batteries (x2)  
- Jumper Wires  
- Robot Chassis + Wheels + Ball Caster


## 🏗️ System Design & Hardware

The robot is built on a simple and stable mechanical structure:

- Two N20 motors are mounted on a chassis using brackets  
- Rubber wheels provide traction  
- A ball caster wheel is added for balance  

### 🔋 Power System
- Powered by two 3.7V batteries  
- Voltage is regulated using an **LM2596 buck converter** to safely supply the ESP32  

### ⚡ Motor Control
- The **TB6612FNG motor driver** acts as the interface between ESP32 and motors  
- It receives control signals and drives the motors accordingly  

### 👁️ Line Detection
- An **8-channel IR sensor array** is mounted at the front bottom  
- It detects the line using reflected infrared light  
- Sensor data is processed by ESP32 to adjust movement in real time

During the development and testing of the Line Following Robot, several challenges were faced. Each issue was resolved systematically to improve stability and performance.

### 1. Chassis Damage
**Problem:** The chassis cracked or broke during assembly due to pressure while fixing components.  
**Solution:** Used super glue to repair and reinforced the chassis for stability.

### 2. Battery Short Circuit
**Problem:** Battery pack wires short-circuited, causing the wires to melt.  
**Solution:** Replaced the damaged battery pack and implemented a safety practice:
- Only connect the battery during testing  
- Disconnect the battery when not in use to prevent overheating or short circuits

### 3. Improper Initial Setup
**Problem:** Components were connected without a clear arrangement, making the circuit messy and causing multiple re-connections.  
**Solution:** Organized components neatly on the chassis and arranged wires carefully for stability and easier troubleshooting.

### 4. IR Sensor Malfunction
**Problem:** The robot had difficulty detecting the path over time; sensor readings were inconsistent.  
**Solution:** Replaced the faulty IR sensor and adjusted the distance between the sensor and the floor to optimize detection.

### Control & PID Logic

The robot uses an ESP32 to read sensor data and control two motors using a PID loop. Bluetooth commands can start/stop the robot and adjust PID parameters (`Kp`, `Kd`) and speed dynamically.

**Key Features:**
- **Bluetooth Control** – Start/Stop the robot, adjust parameters on the fly  
- **PID Line Following** – Uses 8 IR sensors to detect the line and calculate motor speed corrections  
- **Motor Driver Control** – TB6612FNG controls left and right N20 motors based on PID output  

**Snippet Example:**

```cpp
// Read IR sensors and calculate position
qtr.readCalibrated(sensorValues);
long weighted_sum = 0; int sum = 0;
for (uint8_t i=0; i<SensorCount; i++){
  int digitalValue = (sensorValues[i] > THRESHOLD) ? 1 : 0;
  weighted_sum += digitalValue * (i*1000);
  sum += digitalValue;
}
uint16_t position = (sum == 0) ? lastPosition : weighted_sum/sum;
lastPosition = position;

// PID calculation
int error = 3500 - position;
int PIDvalue = (Kp*error) + (Kd*(error - previousError));
previousError = error;

// Apply to motors
setMotors(baseSpeed - PIDvalue, baseSpeed + PIDvalue);
```
The day of the competition was the moment our team had been preparing for. Until the day before, our robot faced several issues, but after making key improvements, it worked correctly.

### Round Strategy:
- **First Round:**  
  - Objective: Complete one lap  
  - Base Speed: 150  
  - Max Speed: 180  
  - Outcome: Robot completed the lap successfully, allowing us to skip the wildcard round

- **Second Round:**  
  - Objective: Achieve the lowest lap time  
  - Base Speed: 200  
  - Max Speed: 255  
  - PID Adjustments: Kp = 0.07, Kd = 0.13  
  - Outcome: The robot completed the track smoothly, achieving **7th place** with a lap time of **13.79 seconds**

This experience demonstrated the importance of careful **tuning, testing, and strategy** for competition success.


