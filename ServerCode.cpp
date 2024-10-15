#include <WiFi.h>

const char* ssid = "ESP32_Host";
const char* password = "12345678";

WiFiServer server(80);

unsigned long timeout = 30000;
unsigned long lastActivityTime = 0;
bool reminderSent = false;

void setup() {
  Serial.begin(115200);

  // Set up WiFi Access Point
  WiFi.softAP(ssid, password);

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address of the server
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  // Check for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");

    // Reset the timer when the client connects
    lastActivityTime = millis();

    while (client.connected()) {
      if (client.available()) {
        String message = client.readStringUntil('\n');
        message.trim();
        Serial.println("Received message: " + message);

        // Send response to client so it has to be client.println()
        client.println("Message is succesfully received in the server side");

         // Reset the activity timer since we've received new message
        lastActivityTime = millis();
        reminderSent = false;
      }
      // send a reminder after half of the timeout passed
      if ((millis() - lastActivityTime >= timeout / 2) && !reminderSent) {
        Serial.println("I'm still listening...");
        client.println("Server: I'm still listening...");  
        reminderSent = true; 
      }

      if (millis() - lastActivityTime > timeout) {
        Serial.println("Timeout reached, disconnecting client");
        client.stop();  
        break;
      }
      // I removed delay to allow the loop to check for data continuously
    }

    if (!client.connected()) {
      Serial.println("Client disconnected");
    }
  }
}
