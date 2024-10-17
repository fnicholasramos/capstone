#include <WiFi.h>
#include <ESP32Ping.h>

const char* ssid     = "PLDTHOMEFIBRhGx23";
const char* password = "PLDTWIFItEdna";

const IPAddress remote_ip(192, 168, 1, 17); 

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("CONNECTED TO WIFI SUCCESSFULLY!.");
  Serial.print("ESPping IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask()); 

  if (Ping.ping(remote_ip)) {
    Serial.println("Ping successful!");
  } else {
    Serial.println("Ping failed :(");
  }
}

void loop() {

}
