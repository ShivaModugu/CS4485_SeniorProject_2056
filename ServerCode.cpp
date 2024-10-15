#include <WiFi.h>

const char* ssid = "ESP32_Host";
const char* password = "12345678";

WiFiServer server(80);

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

    while (client.connected()) {
      if (client.available()) {
        String message = client.readStringUntil('\n');
        Serial.println("Received message: " + message);

        // Send response to client
        server.println("Message received");
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}
