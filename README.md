# CrowdSafe 🚨

> Arduino-based Stampede Early Warning System using the Crowd Pressure Index (CPI) Algorithm

![Arduino](https://img.shields.io/badge/Arduino-Mega%202560-teal)
![Sensors](https://img.shields.io/badge/Sensors-HC--SR04%20%7C%20MPU--6050-blue)
![License](https://img.shields.io/badge/License-MIT-green)

---

## 📌 Problem Statement

Stampedes at crowded events (religious gatherings, concerts, stadiums) cause hundreds of deaths annually in India. Existing systems react *after* a stampede begins. CrowdSafe provides **proactive, real-time alerts** by detecting dangerous crowd conditions before they escalate.

---

## 🧠 Core Algorithm — Crowd Pressure Index (CPI)

The CPI is a novel composite metric derived from three sensor inputs:

```
CPI = (0.45 × DensityScore) + (0.35 × MovementScore) + (0.20 × ShockScore)
```

| Score | Source | Measures |
|---|---|---|
| DensityScore | 3× HC-SR04 ultrasonic | Crowd proximity / compression |
| MovementScore | Distance deltas | Crowd turbulence / wave effect |
| ShockScore | MPU-6050 accelerometer | Physical impact / barrier stress |

### Alert Levels

| CPI Range | Level | Action |
|---|---|---|
| 0 – 39 | 🟢 NORMAL | Green LED |
| 40 – 69 | 🟡 WARNING | Yellow LED + slow beep |
| 70 – 100 | 🔴 DANGER | Red LED + rapid beep + LCD alert |

---

## 🔌 Hardware Required

| Component | Quantity |
|---|---|
| Arduino Mega 2560 | 1 |
| HC-SR04 Ultrasonic Sensor | 3 |
| MPU-6050 Accelerometer | 1 |
| 16×2 I2C LCD Display | 1 |
| RGB LED (common cathode) | 1 |
| Buzzer (active) | 1 |
| Breadboard + Jumper Wires | — |

---

## 🔧 Circuit Connections

| Component | Arduino Pin |
|---|---|
| Ultrasonic 1 (TRIG/ECHO) | D2 / D3 |
| Ultrasonic 2 (TRIG/ECHO) | D4 / D5 |
| Ultrasonic 3 (TRIG/ECHO) | D6 / D7 |
| LED Red / Green / Blue | D8 / D9 / D10 |
| Buzzer | D11 |
| LCD SDA / SCL | A4 / A5 |
| MPU-6050 SDA / SCL | A4 / A5 |

---

## 📁 Project Structure

```
CrowdSafe/
├── CrowdSafe.ino         # Main Arduino sketch
├── cpi_simulation.py     # Python simulation for algorithm testing
└── README.md
```

---

## 🚀 Getting Started

### Arduino
1. Install libraries via Arduino IDE Library Manager:
   - `LiquidCrystal I2C` by Frank de Brabander
   - `MPU6050` by Electronic Cats
2. Open `CrowdSafe.ino` in Arduino IDE
3. Select **Board**: Arduino Mega 2560 and correct **Port**
4. Upload

### Python Simulation (no hardware needed)
```bash
python cpi_simulation.py
```

---

## 📸 Output Preview (Serial Monitor)

```
Time  D-Score  M-Score  S-Score    CPI  Level
----------------------------------------------------
   0s     82.5      0.0      0.0   37.1  NORMAL  ✅
   5s     55.0     12.5      0.0   52.6  WARNING 🟡
  10s     18.0     30.0     48.0   29.7  DANGER  ⚠️
```

---

## 👨‍💻 Author

**Pranav** — AI & Data Science, VIT Pune  
[GitHub](https://github.com/your-username)

---

## 📄 License

MIT License
