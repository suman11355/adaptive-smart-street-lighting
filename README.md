## Live Simulation

[Click here to view and simulate this circuit on Wokwi](https://wokwi.com/projects/478928900547729409)
# Adaptive Smart Street Lighting System
<img width="955" height="633" alt="Screenshot 2026-10-28 003004" src="https://github.com/user-attachments/assets/97459fc7-4448-46ec-b4ab-387112799740" />
<img width="960" height="624" alt="Screenshot 2026-10-28 002945" src="https://github.com/user-attachments/assets/27c04f5c-9c4a-4feb-8cde-3fff67818f81" />



An IoT-based street lighting project using ESP32, an LDR light sensor, and MQTT cloud messaging.

## Features

- Automatic lighting based on ambient light.
- Manual and automatic operating modes.
- Individual control of three streetlight poles.
- MQTT publishing of LDR readings and pole status.
- Designed for testing in Wokwi with PlatformIO.

## Hardware

- ESP32 development board
- LDR light sensor module
- Three LEDs for simulated streetlight poles
- Current-limiting resistors and jumper wires

## Pin Configuration

| Component | ESP32 GPIO |
|---|---:|
| LDR analog output | 34 |
| Streetlight pole 1 | 21 |
| Streetlight pole 2 | 19 |
| Streetlight pole 3 | 18 |

## MQTT Configuration

Broker: broker.hivemq.com  
Port: 1883  
Topic prefix: streetlight/suman2026

| Topic | Purpose |
|---|---|
| streetlight/suman2026/mode | Send `auto` or `manual` |
| streetlight/suman2026/pole1 | Send `on` or `off` |
| streetlight/suman2026/pole2 | Send `on` or `off` |
| streetlight/suman2026/pole3 | Send `on` or `off` |
| streetlight/suman2026/sensor/ldr | Published LDR reading |
| streetlight/suman2026/currentMode | Published operating mode |
| streetlight/suman2026/pole1/status | Published pole 1 status |
| streetlight/suman2026/pole2/status | Published pole 2 status |
| streetlight/suman2026/pole3/status | Published pole 3 status |

## Build

1. Install Visual Studio Code and the PlatformIO IDE extension.
2. Open this project folder.
3. Allow PlatformIO to install the ESP32 platform and PubSubClient library.
4. Build using PlatformIO's Build command.
5. Upload to a connected ESP32, or transfer the logic and hardware setup to a compatible Wokwi simulation.

## Notes

- Adjust `DARK_THRESHOLD` in `src/main.cpp` after observing LDR readings.
- The public MQTT broker is for testing only. Messages are not private and port 1883 does not encrypt traffic.
- Use authenticated, encrypted MQTT and unique credentials for a real deployment.
