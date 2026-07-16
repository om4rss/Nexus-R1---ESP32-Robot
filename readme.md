# Nexus R1 - ESP32 Gemini-Powered 2WD Robot Car

An intelligent, dual-wheel drive (2WD) robot controlled via a Python-based Wi-Fi server integrated with the Gemini API. This project offloads computational processing and natural language/vision reasoning to the cloud, enabling the low-power ESP32 microcontroller to focus entirely on low-latency hardware execution.

## System Architecture

---

## Hardware Configuration

### Component Specifications

| Component | Specification / Model | Purpose |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32 Development Board | Handles Wi-Fi connection and PWM generation |
| **Motor Driver** | L298N Dual H-Bridge | Regulates DC motor direction and speed |
| **Actuators** | 2x TT DC Gear Motors | Drives the 2WD chassis |
| **Power Supply** | 4x 18650 Li-ion Batteries | Provides stable high-drain current |

### Pin Mapping (Default)

| ESP32 Pin | L298N Pin | Function |
| :--- | :--- | :--- |
| **GPIO 12** | IN1 | Motor A Forward |
| **GPIO 13** | IN2 | Motor A Backward |
| **GPIO 14** | IN3 | Motor B Forward |
| **GPIO 27** | IN4 | Motor B Backward |
| **GPIO 25** | ENA | Speed Control Motor A (PWM) |
| **GPIO 26** | ENB | Speed Control Motor B (PWM) |

---

## Repository Structure

```
├── ai.py                    # Gemini API orchestration and Wi-Fi communication
└── robot.ino                # Hardware pin mapping and Bluetooth execution
└── README.md
```

1. Firmware Deployment (ESP32)
   
   Open robot.ino in VS Code with Arduino IDE.
   
   Edit robot.ino to include your network credentials and server IP address:
   ```
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   const char* server_ip = "YOUR_SERVER_IP";
   const uint16_t server_port = 8080;
   Build and flash the firmware to your ESP32 board.
   ```

3. Gemini API Key
   Visit https://aistudio.google.com/app/apikey and click "Create API key"
   
   export GEMINI_API_KEY="your_api_key_here"
   
   (On Windows PowerShell, use: $env:GEMINI_API_KEY="your_api_key_here")

---
Usage

Start the Python server backend:

```
python ai.py
```

Power on the ESP32 robot car. It will automatically connect to the localized Wi-Fi network and establish a persistent TCP handshake with the server.
Issue high-level prompt commands via the server console to test the Gemini-interpreted directional logic.
