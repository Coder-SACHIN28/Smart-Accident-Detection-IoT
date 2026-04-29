# Advanced IoT-Based Vehicle Tracking & Smart Accident Detection System

A robust Cyber-Physical system designed to detect vehicular crashes in real-time and automate emergency response using ESP32, GPS, and multiple cloud APIs.

## 🌟 Key Features
- **Crash Detection:** Uses MPU6050 (Accelerometer + Gyroscope) with intelligent thresholding.
- **Verification Logic:** 10-second countdown with a loud siren to prevent false alarms.
- **Multi-Channel SOS:** Sends Telegram text alerts with Google Maps links and triggers an automated VoIP Voice Call.
- **Reverse Geocoding:** Converts GPS coordinates into human-readable addresses via Nominatim API.
- **Cloud Telemetry:** Live data logging (Acceleration X, Y, Z and Speed) on ThingSpeak Dashboard.
- **Indoor Fallback:** Intelligent static coordinate fallback for indoor testing environments.

## 🛠️ Hardware Stack
- **ESP32** DevKit V1
- **MPU6050** IMU Sensor
- **NEO-6M** GPS Module
- **SSD1306** OLED Display (128x64)
- **95dB Piezo Buzzer**
- **Tactile Push Button**

## 💻 Software & Libraries
- Arduino IDE
- [UniversalTelegramBot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)
- [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus)
- [Adafruit MPU6050 & SSD1306](https://github.com/adafruit)
- ThingSpeak API
- CallMeBot API

## 🛡️ Cyber Security Context
This project focuses on secure IoT communication protocols, endpoint authorization using API tokens, and ensuring data integrity between edge devices and the cloud.

---
Developed as part of the ICT Workshop II at Gautam Buddha University.
