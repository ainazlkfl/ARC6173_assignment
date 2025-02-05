# Smart Bin System

A smart waste bin monitoring system built with ESP32 that provides real-time bin status monitoring through Blynk IoT platform.

## Features

- Real-time bin capacity monitoring (Empty, Half Full, Full)
- Person detection using ultrasonic sensor
- LCD display showing bin status and user instructions
- LED indicators (Blue: Empty, Yellow: Half Full, Red: Full)
- Mobile app monitoring through Blynk
- System on/off control via Blynk app

## Hardware Requirements

- ESP32 Development Board
- 2x IR Sensors
- 1x Ultrasonic Sensor (HC-SR04)
- 16x2 I2C LCD Display
- 3x LEDs (Blue, Yellow, Red)
- Resistors for LEDs
- Power Supply (Powerbank)
- Breadboard and jumper wires

## Pin Configuration

Half Full IR Sensor: GPIO25
Full IR Sensor: GPIO26
Blue LED: GPIO33
Yellow LED: GPIO32
Red LED: GPIO23
LCD SDA: GPIO21
LCD SCL: GPIO22
Ultrasonic TRIG: GPIO18
Ultrasonic ECHO: GPIO19
System Switch: GPIO5

## Libraries Required

- WiFi
- BlynkSimpleEsp32
- Wire
- LiquidCrystal_I2C

## Setup Instructions

1. Install required libraries in Arduino IDE
   
3. Configure your Blynk credentials:
   ```cpp
   #define BLYNK_TEMPLATE_ID "Your_Template_ID"
   #define BLYNK_TEMPLATE_NAME "Your_Template_Name"
   #define BLYNK_AUTH_TOKEN "Your_Auth_Token"
   
4. Configure WiFi credentials:
   #define WIFI_SSID "WiFi_SSID"
  #define WIFI_PASS "WiFi_password"
   
