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

  server.begin();
  Serial.println("Server started");
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
        Serial.println("Server received: " + message);

        // Send response to client so it has to be client.println()
        client.println("Message successfully received on the server side");

        int number = 2;
        float coordinates[2] = {35.1589, -112.45698};
        bool isMarkerFound = true;
        bool moveUp = true;

        // Construct the array message using char arrays
        char arrayMessage[150]; // Ensure this size is large enough to hold the full message
        snprintf(arrayMessage, sizeof(arrayMessage), "[%d,[%.6f,%.6f],%s,%s]", 
                 number, coordinates[0], coordinates[1], 
                 isMarkerFound ? "true" : "false", 
                 moveUp ? "true" : "false");

        client.println("Array: " + String(arrayMessage)); // Sending array message as a char[]
        Serial.println("Sent array: " + String(arrayMessage));

        // Reset the activity timer since we've received a new message
        lastActivityTime = millis();
        reminderSent = false;
      }

      // Send a reminder after half of the timeout has passed
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
