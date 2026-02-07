#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3-0568XG8"
#define BLYNK_TEMPLATE_NAME "Aquascent"
#define BLYNK_AUTH_TOKEN "edmz9PmLUCi7blOQIdPvigzVbGSvuREc"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "edmz9PmLUCi7blOQIdPvigzVbGSvuREc";
char ssid[] = "Esther"; 
char pass[] = "WIFI_PASS"; //(Redacted)

BlynkTimer timer;
float formalinPPM;
float nitrogenPPM;
float temperature;
float humidity;

// Helper for string formatting
String interpolate(const char* str, ...) {
  static char buffer[256];
  va_list args;
  va_start(args, str);
  vsnprintf(buffer, sizeof(buffer), str, args);
  va_end(args);
  return String(buffer);
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sensorvalue1);
}

void loop() {
  Blynk.run();
  timer.run();
  
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    if (data.startsWith("F:")) {
      data.remove(0, 2); formalinPPM = data.toFloat();
    } else if (data.startsWith("N:")) {
      data.remove(0, 2); nitrogenPPM = data.toFloat();
    } else if (data.startsWith("T:")) {
      data.remove(0, 2); temperature = data.toFloat();
    } else if (data.startsWith("H:")) {
      data.remove(0, 2); humidity = data.toFloat();
    }
  }
}

void sensorvalue1() {
  Blynk.virtualWrite(V0, formalinPPM);
  Blynk.virtualWrite(V1, nitrogenPPM);
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, humidity);

  // --- Alert Logic Required by Problem Statement ---
  if (nitrogenPPM > 320) {
    String message = interpolate("Dear Fish Trader,\n\n"
      "This is an automated message from Aquascent IoT.\n\n"
      "High nitrogen levels detected in fish storage. Immediate action needed.\n\n"
      "Nitrogen Level: %.2f ppm.\n\n"
      "For assistance, contact support team.\n\n"
      "Best regards,\nAquascent System.", nitrogenPPM);
      
    Blynk.logEvent("spoilage_alert", message);
  }
}
