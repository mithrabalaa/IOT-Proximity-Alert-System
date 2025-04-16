#define BLYNK_TEMPLATE_ID "TMPL4njNYKzr_"
#define BLYNK_TEMPLATE_NAME "IOT Proximity Alert System"
#define BLYNK_AUTH_TOKEN "ObEfsaj-KftphJn9uglSGdXtiG2vAOUK"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

// Ultrasonic Sensor Pins
#define TRIG_PIN     5
#define ECHO_PIN     18

// Outputs
#define LED_PIN      19
#define BUZZER_PIN   23
#define RELAY_PIN    4

// WiFi credentials (Wokwi)
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Non-blocking timing variables
unsigned long previousMillis = 0;  // stores the last time the sensor was read
const long interval = 1000;  // interval at which to update (milliseconds)

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C LCD at address 0x27

long duration;
float distance;

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Configure pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("IOT Proximity");
  lcd.setCursor(0, 1);
  lcd.print("Alert System");
  delay(2000);
  lcd.clear();
}

void loop() {
  Blynk.run();

  unsigned long currentMillis = millis();  // Get the current time

  // Check if it's time to update the sensor
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the sensor was read
    previousMillis = currentMillis;

    // Measure distance
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2); // short delay to ensure clean pulse
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10); // 10us pulse to trigger the sensor
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distance: ");
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.print(" cm");

    // Send to Blynk
    Blynk.virtualWrite(V0, distance);

    // Alerts
    String status = "Safe";
    String relayStatus = "RELAY OFF";

    if (distance < 10 && distance > 0) {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      status = "WARNING";
    } else {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }

    if (distance < 5 && distance > 0) {
      digitalWrite(RELAY_PIN, HIGH);
      relayStatus = "RELAY ON";
    } else {
      digitalWrite(RELAY_PIN, LOW);
    }

    // Update Blynk widgets
    Blynk.virtualWrite(V1, status);
    Blynk.virtualWrite(V2, relayStatus);

    // Log to terminal
    if (status == "WARNING") {
      String log = "ALERT: " + String(distance) + " cm | " + status + " | Time: " + String(millis() / 1000) + "s";
      Blynk.virtualWrite(V3, log);
    }
  }
}
