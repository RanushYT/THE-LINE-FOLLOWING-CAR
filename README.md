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






