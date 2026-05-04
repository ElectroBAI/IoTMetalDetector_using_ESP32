# ESP32 IoT Metal Detector System

This project is a sophisticated **IoT & Low-Power** application that transforms an ESP32 into a web-connected security monitor. It utilizes a physical metal detection sensor and hosts a high-end, responsive web dashboard to provide real-time alerts and detection statistics.

---

## Features
*   **Real-Time Web Dashboard**: A modern, CSS-animated interface hosted directly on the ESP32 featuring "SAFE" and "ALERT" status indicators.
*   **Visual & Audio Feedback**: On-board LED and optional buzzer activation upon detection.
*   **JSON API Endpoints**: Provides `/api/status` and `/api/detection` routes, allowing the device to integrate with larger **Industrial & Embedded** monitoring systems.
*   **Automatic Statistics**: Tracks total detection counts and system uptime (formatted as HH:MM:SS on the frontend).
*   **CORS Enabled**: The server supports Cross-Origin Resource Sharing, making it compatible with external web applications.

---

## Hardware Requirements
Following the **Maker & Education** hardware standards, this project is designed for accessible yet powerful microcontrollers.

*   **Microcontroller**: ESP32 (selected for its integrated WiFi and high-speed processing).
*   **Sensor**: Inductive Metal Detector Module (Digital Output).
*   **Indicators**: 
    *   1x LED (Internal Pin 25).
    *   1x Active/Passive Buzzer (Pin 26).

### **Pin Configuration**
| Component | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **Metal Detector** | GPIO 2 | Input Signal (HIGH = Detected) |
| **Alert LED** | GPIO 25 | Visual Indicator |
| **Buzzer** | GPIO 26 | Audio Alarm |

---

## Software Architecture
1.  **WiFi Connectivity**: Connects to a local network to serve the web interface.
2.  **Sensor Polling**: The `loop()` function monitors GPIO 2 every 50ms for a state change.
3.  **JSON Processing**: Uses the `ArduinoJson` library to package system health data for the web client.
4.  **Frontend Logic**: The HTML/JavaScript dashboard uses the `fetch` API to poll the ESP32 every 500ms, ensuring the UI remains "live" without requiring a page refresh.

---
