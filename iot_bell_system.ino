#include <Wire.h>
#include <RtcDS1302.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define EEPROM_SIZE 512  // Size of EEPROM for storing alarm data

#define WIFI_SSID "NodeMcu"  // Wi-Fi SSID
#define WIFI_PASS "1234" // Wi-Fi Password

const int relay = D5; // Pin connected to relay
const int maxAlarms = 10; // Maximum number of alarms per group
RtcDateTime alarmTimes[maxAlarms * 2]; // Array to store alarm times for both groups
int eepromAddress = 0; // EEPROM address for storing alarm data
bool relayActive = false; // Relay status
unsigned long lastActivationTime = 0; // Last activation time of the relay

void saveAlarmData(int groupIndex, const RtcDateTime *groupAlarms) {
  // Calculate EEPROM address for the given group
  int eepromAddressGroup = groupIndex * maxAlarms * sizeof(RtcDateTime);

  // Save alarm times to EEPROM
  for (int i = 0; i < maxAlarms; i++) {
    EEPROM.put(eepromAddressGroup + i * sizeof(RtcDateTime), groupAlarms[i]);
  }

  EEPROM.commit(); // Commit changes to EEPROM
}

void loadAlarmData(int groupIndex, RtcDateTime *groupAlarms) {
  // Calculate EEPROM address for the given group
  int eepromAddressGroup = groupIndex * maxAlarms * sizeof(RtcDateTime);

  // Load alarm times from EEPROM
  for (int i = 0; i < maxAlarms; i++) {
    EEPROM.get(eepromAddressGroup + i * sizeof(RtcDateTime), groupAlarms[i]);
  }
}

bool showIPAddress = true; // Flag to toggle display of IP address

ThreeWire myWire(D7, D8, D6); // Define RTC communication pins
RtcDS1302<ThreeWire> Rtc(myWire); // Create RTC object

ESP8266WebServer server(80); // Create web server on port 80

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Create OLED display object

void setup() {
  Serial.begin(57600); // Initialize serial communication
  EEPROM.begin(EEPROM_SIZE); // Initialize EEPROM

  Serial.println("Starting NodeMcu..");
  WiFi.softAP(WIFI_SSID, WIFI_PASS); // Start Wi-Fi Access Point

  IPAddress ip = WiFi.softAPIP(); // Get IP address of the Access Point
  Serial.println("IPAddress..");
  Serial.println(ip);

  Rtc.Begin(); // Initialize RTC

  pinMode(relay, OUTPUT); // Set relay pin as output

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;)
      ; // Infinite loop if OLED initialization fails
  }

  loadAlarmData(0, alarmTimes); // Load alarms for Group 1
  loadAlarmData(1, alarmTimes + maxAlarms); // Load alarms for Group 2

  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/setalarm", HTTP_POST, handleSetAlarm);
  server.on("/uploadalarms", HTTP_POST, handleUploadAlarms); // Handle upload alarms request
  server.begin(); // Start the web server
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime(); // Get the current date and time from RTC
  checkAlarms(now); // Check if any alarms need to be triggered

  // Display IP address or time and alarms on OLED
  if (showIPAddress) {
    displayIPAddress();
    delay(5000); // Show IP address for 5 seconds
  } else {
    displayTimeAndAlarms(now);
    delay(1000); // Show time and alarms every second
  }

  server.handleClient(); // Handle incoming web server requests
}

void displayIPAddress() {
  display.clearDisplay(); // Clear the display
  display.setTextSize(1); // Set text size
  display.setTextColor(SSD1306_WHITE); // Set text color
  IPAddress ip = WiFi.softAPIP(); // Get IP address
  String ipAddress = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
  display.setCursor(0, 0); // Set cursor position
  display.print("Hotspot IP:");
  display.setCursor(0, 8);
  display.print(ipAddress);
  display.display(); // Display the IP address
}

void displayTimeAndAlarms(const RtcDateTime &now) {
  display.clearDisplay(); // Clear the display
  display.setTextSize(1); // Set text size
  display.setTextColor(SSD1306_WHITE); // Set text color
  display.setCursor(1, 0); // Set cursor position
  display.print("Time: ");
  display.print(now.Hour()); // Display hour
  display.print(":");
  display.print(now.Minute()); // Display minute
  display.print(":");
  display.print(now.Second()); // Display second
  display.setCursor(0, 16); // Set cursor position for alarms
  display.display(); // Display time and alarms
}

void handleRoot() {
  RtcDateTime now = Rtc.GetDateTime(); // Get current date and time

  String html = "<!DOCTYPE html>";
  html += "<html lang='en'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Timer Settings</title>";
  html += "<style>";
  html += "body {";
  html += "font-family: Arial, sans-serif;";
  html += "background-color: #f8f9fa;";
  html += "margin: 0;";
  html += "padding: 0;";
  html += "}";
  html += ".container {";
  html += "max-width: 800px;";
  html += "margin: 50px auto;";
  html += "padding: 20px;";
  html += "background-color: #ffffff;";
  html += "border-radius: 8px;";
  html += "box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);";
  html += "}";
  html += "h2, h3, p {";
  html += "margin: 0 0 15px 0;";
  html += "}";
  html += "h2 {";
  html += "color: #343a40;";
  html += "}";
  html += "h3 {";
  html += "color: #6c757d;";
  html += "}";
  html += "table {";
  html += "width: 100%;";
  html += "border-collapse: collapse;";
  html += "margin-bottom: 20px;";
  html += "}";
  html += "table, th, td {";
  html += "border: 1px solid #dee2e6;";
  html += "}";
  html += "th, td {";
  html += "padding: 8px;";
  html += "text-align: center;";
  html += "}";
  html += "th {";
  html += "background-color: #e9ecef;";
  html += "}";
  html += ".footer {";
  html += "text-align: center;";
  html += "color: #6c757d;";
  html += "font-size: 14px;";
  html += "margin-top: 20px;";
  html += "}";
  html += "button {";
  html += "background-color: #007bff;";
  html += "color: white;";
  html += "padding: 10px 20px;";
  html += "border: none;";
  html += "border-radius: 4px;";
  html += "cursor: pointer;";
  html += "font-size: 16px;";
  html += "}";
  html += "button:hover {";
  html += "background-color: #0056b3;";
  html += "}";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h2>IOT Based Automatic Bell Ring System</h2>";
  html += "<h3>Government Engineering College, Bhavnagar</h3>";
  html += "<p>Electronics and Communication Engineering</p>";
  html += "<p>Current date and time: <span id='currentTime'>" + String(now.Hour()) + ":" + String(now.Minute()) + ":" + String(now.Second()) + "</span></p>";
  html += "<h2>Timer Settings</h2>";
  html += "<label for='groupSelect'>Select Group: </label>";
  html += "<select name='groupSelect' id='groupSelect' onchange='toggleTables()'>";
  html += "<option value='Group1'>Group 1</option>";
  html += "<option value='Group2'>Group 2</option>";
  html += "</select>";
  html += "<form id='alarmForm'>";
  html += "<div id='group1Table'>";
  html += "<h3>Group 1 Alarms</h3>";
  html += "<table>";
  html += "<tr>";
  html += "<th>Timer</th>";
  html += "<th>Hr</th>";
  html += "<th>Min</th>";
  html += "<th>Sec</th>";
  html += "</tr>";

  // Generate HTML table rows for Group 1 alarms
  for (int i = 0; i < maxAlarms; i++) {
    html += "<tr>";
    html += "<td>" + String(i + 1) + "</td>";
    html += "<td><input type='number' name='hrGroup1_" + String(i + 1) + "' value='" + String(alarmTimes[i].Hour()) + "' min='0' max='23'></td>";
    html += "<td><input type='number' name='minGroup1_" + String(i + 1) + "' value='" + String(alarmTimes[i].Minute()) + "' min='0' max='59'></td>";
    html += "<td><input type='number' name='secGroup1_" + String(i + 1) + "' value='" + String(alarmTimes[i].Second()) + "' min='0' max='59'></td>";
    html += "</tr>";
  }

  html += "</table>";
  html += "</div>";
  html += "<div id='group2Table' style='display: none;'>";
  html += "<h3>Group 2 Alarms</h3>";
  html += "<table>";
  html += "<tr>";
  html += "<th>Timer</th>";
  html += "<th>Hr</th>";
  html += "<th>Min</th>";
  html += "<th>Sec</th>";
  html += "</tr>";

  // Generate HTML table rows for Group 2 alarms
  for (int i = 0; i < maxAlarms; i++) {
    html += "<tr>";
    html += "<td>" + String(i + 1) + "</td>";
    html += "<td><input type='number' name='hrGroup2_" + String(i + 1) + "' value='" + String(alarmTimes[i + maxAlarms].Hour()) + "' min='0' max='23'></td>";
    html += "<td><input type='number' name='minGroup2_" + String(i + 1) + "' value='" + String(alarmTimes[i + maxAlarms].Minute()) + "' min='0' max='59'></td>";
    html += "<td><input type='number' name='secGroup2_" + String(i + 1) + "' value='" + String(alarmTimes[i + maxAlarms].Second()) + "' min='0' max='59'></td>";
    html += "</tr>";
  }

  html += "</table>";
  html += "</div>";
  html += "<button type='button' onclick='uploadAlarms()'>Update Alarms</button>";
  html += "</form>";
  html += "<div class='footer'>";
  html += "<p>Project Team</p>"; // Footer text
  html += "</div>";
  html += "</div>";
  html += "<script>";
  html += "function toggleTables() {";
  html += "  var selectedGroup = document.getElementById('groupSelect').value;";
  html += "  if (selectedGroup === 'Group1') {";
  html += "    document.getElementById('group1Table').style.display = 'block';";
  html += "    document.getElementById('group2Table').style.display = 'none';";
  html += "  } else {";
  html += "    document.getElementById('group1Table').style.display = 'none';";
  html += "    document.getElementById('group2Table').style.display = 'block';";
  html += "  }";
  html += "}";
  html += "function uploadAlarms() {";
  html += "  var form = document.getElementById('alarmForm');";
  html += "  var formData = new FormData(form);";
  html += "  fetch('/setalarm', {";
  html += "    method: 'POST',";
  html += "    body: formData";
  html += "  }).then(response => response.text()).then(result => {";
  html += "    alert('Alarms updated successfully');";
  html += "  }).catch(error => {";
  html += "    console.error('Error:', error);";
  html += "  });";
  html += "}";
  html += "</script>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html); // Send HTML response to the client
}

void handleSetAlarm() {
  // Update alarm times based on form input
  for (int i = 0; i < maxAlarms; i++) {
    int hour = server.arg("hrGroup1_" + String(i + 1)).toInt();
    int minute = server.arg("minGroup1_" + String(i + 1)).toInt();
    int second = server.arg("secGroup1_" + String(i + 1)).toInt();
    alarmTimes[i] = RtcDateTime(0, 0, 0, hour, minute, second);

    hour = server.arg("hrGroup2_" + String(i + 1)).toInt();
    minute = server.arg("minGroup2_" + String(i + 1)).toInt();
    second = server.arg("secGroup2_" + String(i + 1)).toInt();
    alarmTimes[i + maxAlarms] = RtcDateTime(0, 0, 0, hour, minute, second);
  }

  // Save updated alarm times to EEPROM
  saveAlarmData(0, alarmTimes);
  saveAlarmData(1, alarmTimes + maxAlarms);

  server.send(200, "text/html", "<h1>Alarms updated</h1><p><a href='/'>Go back</a></p>");
}

void handleUploadAlarms() {
  server.send(200, "text/html", "<h1>Alarms uploaded</h1><p><a href='/'>Go back</a></p>");
}

void checkAlarms(const RtcDateTime &now) {
  // Check if current time matches any alarm time
  for (int i = 0; i < maxAlarms; i++) {
    if (alarmTimes[i] == now) {
      if (!relayActive) {
        digitalWrite(relay, HIGH); // Activate relay
        relayActive = true;
        lastActivationTime = millis(); // Record activation time
      }
    }
  }

  // Deactivate relay after 10 seconds
  if (relayActive && (millis() - lastActivationTime > 10000)) {
    digitalWrite(relay, LOW);
    relayActive = false;
  }
}
