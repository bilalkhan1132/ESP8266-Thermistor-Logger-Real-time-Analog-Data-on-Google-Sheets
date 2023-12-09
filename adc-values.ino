#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "Waheed-mobilenet"; 
const char* password = "9abbd7e64f97";

const char* host = "script.google.com";
const int httpsPort = 443;

WiFiClientSecure client;

String GAS_ID = "AKfycbzIB5kHD9Pc1YrQUXopQ7KIzOVqorTR2-QD3ThHBAzR5-XTypSAjWfe2P09BEG1X5cA"; 

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  client.setInsecure();
}

void loop() {
  // Read analog sensor values and calculate corresponding voltages
  int sensorValue = analogRead(A0); 
  float voltage = sensorValue * (3.3 / 1023.0);

  // Send analog sensor data to Google Sheets
  sendData(sensorValue, voltage);
  delay(2000); 
}

// Function to send analog sensor data to Google Sheets
void sendData(int sensorVal, float voltage) {
  Serial.println("Connecting to Google Sheets...");

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection to Google Sheets failed.");
    return;
  }

  String sensorData = String(sensorVal);
  String voltageData = String(voltage, 3);

  String url = "/macros/s/" + GAS_ID + "/exec?sensor=" + sensorData + "&voltage=" + voltageData;
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent.");

  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }

  Serial.println("Response received.");
  client.stop();
}
