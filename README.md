# 🤖 Arduino Sumo Robot with PS4 Controller

This project is an **autonomous and remote-controlled Sumo Robot** built with **Arduino** and controlled via a **PS4 joystick**.  
The robot is designed for **Sumo competitions**, capable of moving with various speed modes, detecting arena edges, and performing attack maneuvers.

## 🔍 Project Overview
- Controlled using **PS4Controller.h** library  
- Dual motor control with multiple states: **STOP, CW, CCW**  
- Speed modes:
  - **Normal Speed**
  - **Attack Speed**
  - **Turn Speed**
  - **Edge Escape Speed** (to avoid leaving the arena)  
- Gradual speed adjustment for smoother movement  

## 🛠️ Technologies & Components
- Arduino (ESP32 recommended with PS4 controller support)  
- PS4 Controller (Bluetooth connection)  
- Dual DC Motors with motor driver  
- Edge detection mechanism (line sensors)  
- LED indicator  

## 📷 Robot Preview
![Robot Sumo](DSC_3283.JPG)  

## 🎯 Goal
To design and implement a **Sumo Robot** that is both competitive in the arena and fun to control with a PS4 joystick.

---
✨ This project is part of my **Robotics Portfolio**.
