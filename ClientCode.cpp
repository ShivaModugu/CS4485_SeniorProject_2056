#include <WiFi.h>

const char* ssid = "ESP32_Host";     // Host's SSID
const char* password = "12345678";   // Host's password

const char* host_ip = "192.168.4.1"; // Host IP Address
const uint16_t port = 80;

String defaultMessage = "Default message: ESP32 communication established! You may now send messages to the server."; // Default message
unsigned long timeout = 30000;  //close connection after 30 sec inactive
unsigned long startTime = 0;
unsigned long retryDelay = 2000; // Retry connection delay (non-blocking)
unsigned long lastRetryTime = 0;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(defaultMessage);  //print default message
}

WiFiClient client;

void loop() {
  if (!client.connected() && millis() - lastRetryTime > retryDelay) {
    if (client.connect(host_ip, port)) {
      Serial.println("Connected to server");
      client.println(defaultMessage);    //send defautl message to the server
      Serial.println("Client sent: " + defaultMessage);
      startTime = millis();     // Start timeout timer
    } 
    else {
      Serial.println("Failed to connect to server");
      lastRetryTime = millis();  // Update the last retry time
    }
  }
  

  //read input from serial monitor
  if (Serial.available()) {
    String response = Serial.readStringUntil('\n');    
    response.trim();
    if (response.length() > 0) {
      client.println(response);      // send the input to the server
      Serial.println("Sent: " + response);
    }
  }

  // Wait for response from server
  while (client.connected()) {
    if (client.available()) {
      String response = client.readStringUntil('\n');
      Serial.println("Received from server: " + response);
      Serial.println("Connection will close after 30 seconds of inactive");
      startTime = millis();      //reset timeout
    }

    if (millis() - startTime >= timeout) {
      Serial.println("Timeout reached, closing connection");
      client.stop(); 
      break;
    }
    // remove delay to allow instant response  
  }

  if (!client.connected()) {
    Serial.println("Client disconnected");
  }
}
