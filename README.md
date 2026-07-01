# Arduino Control

This repo contains the Arduino code for the MHS MATE ROV. 

Specifically, this is reponsible for PWM signals (thruster and servo controls), and transmitting controller data to the ROV. For more details on how everything connects together, see the 2024-2025 and 2025-2026 MATE technical documentation (which we submitted to the competition), and also the READMEs / documentation in the Ventgarden repo.

The reciever code, "receieves" controller data, and is for the Arduino on the ROV. If you want to change what buttons do what, thruster speed, thruster / servo ports, claw range, etc - you should probably use that folder. 

The sender code, "sends" controller data to the ROV via the RS-485 boards. This is the code for the Arduino that runs in the control box. As a result, this code also has the implementation for recieving (or potentially transmitting) radio data to the float. 