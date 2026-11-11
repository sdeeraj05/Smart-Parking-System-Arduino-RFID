# Smart-Parking-System-Arduino-RFID
An automated parking management system built with Arduino Uno, utilizing RFID (Fast Tag) for access control and IR sensors for real-time parking slot availability.  README.md

# Overview
This project aims to improve parking lot management through an automated system that streamlines the entry and exit of vehicles. The system scans RFID tags to verify user access, ensuring only authorized vehicles can enter reserved spaces. Once an RFID tag is validated, the system checks for available parking spaces using IR sensors. If a free space is available, the servo-controlled gate mechanism activates and the available slot is displayed on an LCD. If the parking lot is full or the RFID tag is invalid, the gate remains closed and an access denied message is displayed.
#Features
Reduces parking search time by displaying available slots at the entrance
Automates entry and exit gate mechanisms
Enhances security by restricting access to authorized RFID tags
Adaptable for use in malls, offices, and public parking lots
#Hardware Components
Arduino UNO
16x2 LCD Display (with I2C module)
RFID Scanner (RC522 Module)
Fast Tag (RFID key / fob / card)
IR Sensors (for entry/exit gates and individual parking slots)
Servo Motor (for gate operation)
#Pin Connections & Circuit Mapping
RFID Scanner (RC522): Connects to pins 9 (RST) and 10 (SS/SDA), plus standard SPI pins (11, 12, 13), 3.3V, and GND.
LCD Display (I2C): Connects to 5V, GND, and analog pins (A4/A5 or A2/A3 depending on the board configuration).
Gate Servo Motor: Connects to Digital Pin 4, 5V, and GND.
IR Sensors:
Entry Sensor: Digital Pin 2
Exit Sensor: Digital Pin 3
Parking Slot Sensors: Pins 6, 8, A0, A1
Mode Button: Analog Pin A2
#Limitations
Lost or damaged RFID cards can cause access issues for users
IR sensors may be affected by environmental factors like dust, sunlight, or unrelated obstacles
Implementing this on a larger scale requires additional networking and sensors
The current iteration lacks IoT or cloud integration
