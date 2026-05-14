# 💡 Smart Lamp IoT using ESP32 and Firebase

## 📌 Project Overview

This project is an Internet of Things (IoT)-based smart lamp control system developed using an ESP32 microcontroller and Firebase Realtime Database. The system allows users to control a lamp remotely through a mobile/web application, automatic scheduling, and a physical switch.

The project integrates:

* ESP32 WiFi Module
* Firebase Realtime Database
* Relay Module
* Manual Physical Switch
* Real-Time Monitoring System
* Automatic Scheduling System

This system is designed to provide efficient, flexible, and real-time smart lighting control for smart home applications.

---

# ⚙️ Main Features

## ✅ Real-Time Lamp Control

Users can turn the lamp ON or OFF instantly using Firebase-connected applications.

## ✅ Automatic Schedule System

The lamp can automatically turn ON and OFF based on configured schedules.

Example:

* ON at 17:45 WIB
* OFF at 03:30 WIB

## ✅ Manual Physical Switch

A hardware switch is integrated as a priority control system. Even if the internet connection is unstable, the lamp can still be controlled manually.

## ✅ Firebase Cloud Synchronization

All lamp status updates are synchronized in real time to Firebase Realtime Database.

## ✅ Event Logging System

Every lamp activity is recorded into Firebase logs, including:

* Lamp ON/OFF status
* Control mode
* Timestamp information

## ✅ Time Synchronization using NTP

The system automatically synchronizes Indonesian local time (WIB) using an NTP server.

---

# 🛠️ Hardware Components

| Component            | Description               |
| -------------------- | ------------------------- |
| ESP32                | Main microcontroller      |
| Relay Module         | Lamp switching controller |
| Push Button / Switch | Manual control            |
| WiFi Network         | Internet connection       |
| Lamp                 | Output device             |

---

# 🧠 System Workflow

1. ESP32 connects to WiFi.
2. ESP32 synchronizes time using NTP.
3. Firebase configuration is initialized.
4. System reads:

   * Lamp status
   * Schedule configuration
5. ESP32 continuously:

   * Monitors Firebase commands
   * Checks automatic schedules
   * Reads physical switch input
6. Relay controls the lamp state.
7. All activities are stored in Firebase logs.

---

# 🔥 Firebase Structure

```json
{
  "lampu": {
    "status": true,
    "mode": "AUTO",
    "time": "Monday, 13/05/2026 18:00 WIB"
  },

  "settings": {
    "startTime": "17:45",
    "endTime": "03:30"
  },

  "command": {
    "value": "ON"
  },

  "logs": {
    "log1": {
      "event": "Light ON",
      "mode": "AUTO",
      "time": "..."
    }
  }
}
```

---

# 📡 Technologies Used

* C++
* Arduino IDE
* ESP32
* Firebase Realtime Database
* WiFi Communication
* NTP Time Protocol
* IoT Automation System

---

# 📂 Project Structure

```bash
Smart-Lamp-IoT/
│
├── smart_lamp.ino
├── README.md
└── assets/
```

---

# 🚀 Installation Guide

## 1. Install Arduino IDE

Download and install Arduino IDE.

## 2. Install ESP32 Board

Add ESP32 board support through Board Manager.

## 3. Install Required Libraries

Install:

* FirebaseESP32
* WiFi
* NTP / Time Library

## 4. Configure WiFi & Firebase

Edit:

```cpp
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
```

And:

```cpp
#define FIREBASE_HOST "YOUR_FIREBASE_URL"
#define FIREBASE_AUTH "YOUR_FIREBASE_SECRET"
```

## 5. Upload Code

Select:

* Board: ESP32 Dev Module
* Correct COM Port

Then upload the program.

---

# 📈 Advantages of the System

* Real-time communication
* Smart automation
* Cloud-based monitoring
* Hybrid manual & automatic control
* Stable scheduling system
* Event logging support

---

# 📖 Conclusion

This project demonstrates the implementation of an IoT-based smart lighting system using ESP32 and Firebase. The system successfully combines real-time cloud communication, automation scheduling, and manual hardware control into one integrated smart home solution.

The project can be further developed into:

* Smart Home Systems
* Energy Monitoring
* Voice Assistant Integration
* Mobile Applications
* AI-based Automation

---

# 👨‍💻 Developer

Developed for educational and IoT research purposes.
