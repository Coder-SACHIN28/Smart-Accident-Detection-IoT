# 🚨 Advanced IoT-Based Vehicle Tracking & Smart Accident Detection System

**A cyber-physical "Black Box" solution designed to track vehicle locations and autonomously report severe physical impacts.**

### 🎯 Overview
This project is an automated, hardware-driven IoT platform that provides real-time vehicle location tracking. It utilizes an edge-to-cloud architecture to detect high-impact collisions and autonomously dispatch verified, multi-channel emergency alerts to save lives during the critical "golden hour."

### 💻 Tech Stack
* **Hardware:** ESP32 Microcontroller, MPU6050 (6-Axis IMU), NEO-6M GPS Module, SSD1306 OLED, Piezo Buzzer.
* **Firmware:** C/C++ (Arduino IDE)
* **Cloud & APIs:** ThingSpeak (Telemetry), Telegram Bot API, CallMeBot (VoIP), Nominatim (Reverse Geocoding).

### ✨ Key Features
* **Intelligent Crash Detection:** Uses MPU6050 to detect sudden deceleration and rollovers.
* **10-Second Failsafe Window:** Local OLED and buzzer warning with a manual override button to cancel false alarms.
* **Multi-Channel SOS:** Automated Telegram text messages (with Google Maps links) and VoIP voice calls.
* **Reverse Geocoding:** Converts raw GPS coordinates into human-readable street addresses.
* **Live Telemetry:** Real-time logging of X, Y, and Z-axis orientation data on a ThingSpeak dashboard.
