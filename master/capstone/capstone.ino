#define BLYNK_TEMPLATE_ID "TMPL6Sf_hDlZu" // Replace with your actual template ID
#define BLYNK_TEMPLATE_NAME "Health Guard IV Bag Monitoring and Alert System" // Replace with your actual template name

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h> // Use the standard LiquidCrystal_I2C library
#include "HX711.h"
#include <HTTPClient.h>  // Include HTTPClient for making HTTP requests

// Pin Definitions
#define DOUT  23
#define CLK  19
#define BUZZER 25

// Create instances
HX711 scale; // Declare the scale object without parameters
LiquidCrystal_I2C lcd(0x27, 20, 4); // LCD address and size

#define BLYNK_PRINT Serial

// Blynk credentials
char auth[] = "WDrXGLSZPOvFXVTejw7mhVT_vgxjggdp";
char ssid[] = "PLDTHOMEFIBRhGx23";
char pass[] = "PLDTWIFItEdna";

// wifi nila edrian
// char ssid[] = "SKYfiberC3E3";
// char pass[] = "260001003";

// Variables
int liter;
int val;
float weight = 0; 
float calibration_factor = 100000; // Adjust this value for your Load cell sensor
const int MAX_CAPACITY_ML = 1000; // Maximum capacity of the IV bag in ml

void setup() {
  Serial.begin(115200);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Initialize Buzzer
  pinMode(BUZZER, OUTPUT);
  
  // Initialize HX711 with pins
  scale.begin(DOUT, CLK);  // Use the begin() method to set the DOUT and CLK pins
  scale.set_scale();
  scale.tare(); // Reset the scale to 0
  
  long zero_factor = scale.read_average(); // Get a baseline reading
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass); 
}

void loop() {
  Blynk.run(); // Run Blynk
  measureweight(); // Measure weight
}

void measureweight() {
  scale.set_scale(calibration_factor); // Adjust to this calibration factor
  weight = scale.get_units(10); 
  
  if (weight < 0) {
    weight = 0.00;
  }
  
  liter = weight * 1000;
  // Ensure the IV bottle capacity does not exceed the maximum
  if (liter > MAX_CAPACITY_ML) {
    liter = MAX_CAPACITY_ML;
  }
  
  val = map(liter, 0, MAX_CAPACITY_ML, 0, 100); // Map liter to percentage

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("      BOMBA");
  lcd.setCursor(2, 1);
  lcd.print("      NA!!!");

  Serial.print("Kilogram: ");
  Serial.print(weight); 
  Serial.println(" Kg");

  lcd.setCursor(1, 2);
  lcd.print("IV Bottle = ");
  lcd.print(liter);
  lcd.print(" mL");

  Serial.print("IV BOTTLE: ");
  Serial.print(liter);
  Serial.println(" mL");

  lcd.setCursor(1, 3);
  lcd.print("IV Bag Percent=");
  lcd.print(val);
  lcd.print("%");

  Serial.print("IV Bag Percent: ");
  Serial.print(val);
  Serial.println("%");

  sendDataToServer(liter, val);  // Add this line to send the data to your server

  delay(500);

  if (val <= 50 && val >= 40) {
    Blynk.logEvent("iv_alert", "IV Bottle is 50%");
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    delay(50);
  } else if (val <= 20) {
    Blynk.logEvent("iv_alert", "IV Bottle is too LOW");
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  Blynk.virtualWrite(V0, liter);
  Blynk.virtualWrite(V1, val);
}

void sendDataToServer(int liter, int val) {
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected :(");
  } else {
    Serial.println("WIFI CONNECTED !!!");

    HTTPClient http;

    Serial.println("Connecting to server...");

    // Ensure this matches your server's URL and endpoint
    http.begin("http://192.168.1.3:3000/data"); 

    // Set a timeout for the HTTP request (5 seconds)
    http.setTimeout(5000); // 5 seconds timeout

    // Specify the content type as JSON
    http.addHeader("Content-Type", "application/json");

    // Create a JSON payload with the values you want to send
    String payload = "{\"liter\":" + String(liter) + ",\"percent\":" + String(val) + "}";
    // String payload = "{\"liter\":500,\"percent\":50}";


    // Send the POST request
    int httpResponseCode = http.POST(payload);

    // Print the response code to the Serial Monitor
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode); 
    Serial.println(WiFi.localIP());

    // Check the response
    if (httpResponseCode > 0) {
      String response = http.getString();  // Get the response from the server
      Serial.println("Response: " + response);  // Print the response
    } else {
      // Print more descriptive error message
      Serial.print("Error on sending POST: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());  // Descriptive error message
    }

    // Free resources
    http.end();  
  }
}



