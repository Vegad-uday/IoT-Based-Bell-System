# IoT-Based Automatic Bell System

This project is an IoT-based automatic bell system designed for educational institutions, such as schools and colleges. It utilizes the ESP8266 NodeMCU, an RTC DS1302 module, an OLED display, and a relay module to control the bell timings. The system allows for easy configuration of multiple alarm profiles via a web interface.

## Features

- **Remote Control:** Set and manage bell timings remotely via a web interface.
- **Multiple Alarm Profiles:** Configure up to two groups with multiple alarms each.
- **Real-Time Clock (RTC) Integration:** Accurate timekeeping using the DS1302 RTC module.
- **OLED Display:** Displays the current time and system status.
- **EEPROM Storage:** Alarm settings are stored in EEPROM for persistence across resets.
- **Hotspot Mode:** ESP8266 NodeMCU acts as a Wi-Fi hotspot, allowing easy access to the configuration page.

## Components Required

- ESP8266 NodeMCU
- RTC DS1302 Module
- Relay Module
- OLED Display (128x32)
- Jumper Wires
- Breadboard or PCB

## Libraries Used

- [Wire](https://www.arduino.cc/en/Reference/Wire)
- [Rtc by Makuna](https://github.com/Makuna/Rtc)
- [ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)
- [ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)

## Installation

1. **Open the project in Arduino IDE:**
    - Ensure you have the required libraries installed via Arduino Library Manager.
    - Connect your ESP8266 NodeMCU to your computer.

2. **Upload the code:**
    - Select the correct board and port from the Arduino IDE.
    - Upload the `iot_bell_system.ino` file to the ESP8266 NodeMCU.

3. **Connect the components:**
    - Follow the wiring diagram to connect the RTC module, relay, and OLED display to the ESP8266.

4. **Access the configuration page:**
    - After uploading the code, the ESP8266 will start as a Wi-Fi hotspot with the SSID `Your SSID` and password `Your Password`.
    - Connect to this network and open a web browser.
    - Go to `OLED display shows the IP address` to access the bell system's configuration page.

## Usage

- **Configure Alarms:**
    - Select the group (Group 1 or Group 2) and set the desired alarm times.
    - Click "Update Alarms" to save the settings.

- **Real-Time Monitoring:**
    - The OLED display shows the current time and upcoming alarms.

## Contributing

If you would like to contribute to this project, please open an issue or submit a pull request with your improvements or bug fixes.

## Authors

- Prajapati Manish 
- Arvind Gupta 
- Hiren Dabhi 
- Vegad Uday 

