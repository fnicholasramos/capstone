#define BLYNK_TEMPLATE_ID "TMPL6Sf_hDlZu" // Replace with your actual template ID
#define BLYNK_TEMPLATE_NAME "Health Guard IV Bag Monitoring and Alert System" // Replace with your actual template name

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include <HTTPClient.h>

// Pin Definitions
#define DOUT  23
#define CLK  19
#define BUZZER 26

// Create instances
HX711 scale;
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define BLYNK_PRINT Serial

// Blynk credentials
// wifi nila francis
// char auth[] = "WDrXGLSZPOvFXVTejw7mhVT_vgxjggdp";
// char ssid[] = "PLDTHOMEFIBRhGx23";
// char pass[] = "PLDTWIFItEdna";

char auth[] = "WDrXGLSZPOvFXVTejw7mhVT_vgxjggdp";
char ssid[] = "pokoloko";
char pass[] = "12345678";


// Variables
int liter;
int val;
float weight;
float calibration_factor = 1500000; 
const int MAX_CAPACITY_ML = 1000;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(BUZZER, OUTPUT);
  
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare();
  
  long zero_factor = scale.read_average();
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  measureWeight();
}

void measureWeight() {
  scale.set_scale(calibration_factor);
  weight = scale.get_units(5);
  
  if (weight < 0) {
    weight = 0.00;
  }
  
  liter = weight * 1000;
  if (liter > MAX_CAPACITY_ML) {
    liter = MAX_CAPACITY_ML;
  }
  
  val = map(liter, 0, MAX_CAPACITY_ML, 0, 100);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("   Health Guard");
  lcd.setCursor(2, 1);
  lcd.print("");

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

  sendDataToServer(liter, val); // send the data to your server

  delay(500);

  if (val <= 50 && val >= 40) {
    Blynk.logEvent("iv_alert", "IVF Volume is 50%");
    for (int i = 0; i < 5; i++) {
      digitalWrite(BUZZER, HIGH);
      delay(100);
      digitalWrite(BUZZER, LOW);
      delay(100);
    }
  } else if (val <= 10 && val >= 1) {
    Blynk.logEvent("iv_alert", "IVF Volume is too LOW");
    digitalWrite(BUZZER, HIGH);  // Continuous sound for critical alert
  } else {
    digitalWrite(BUZZER, LOW);    // No sound if above alert levels
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


    // http.begin("https://healthguardiv.site/admin/server.php"); 
    http.begin("http://192.168.1.16/capstone/server.php");
    http.setTimeout(5000); // 5 seconds timeout
    http.addHeader("Content-Type", "application/json");

    String deviceID = "pt0001";
    String payload = "{\"device_id\":\"" + deviceID + "\",\"liter\":" + String(liter) + ",\"percent\":" + String(val) + "}";

    int httpResponseCode = http.POST(payload);
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode); 
    Serial.println(WiFi.localIP());

    if (httpResponseCode > 0) {
      String response = http.getString(); 
      Serial.println("Response: " + response);  
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();  
  }
}
