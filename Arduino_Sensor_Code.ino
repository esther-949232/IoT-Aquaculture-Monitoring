#include <DHT.h> 
#include "U8glib.h"
#include <Wire.h>
#include <SoftwareSerial.h>

// --- Pin Definitions ---
const int VOUT_PIN = A1; // Formaldehyde sensor
const int HEATER_PIN = 7; 
#define RESET_LED_BUILTIN
#define BUZZER_PIN 2
#define DHTPIN A2 
#define DHTTYPE DHT22 

// --- Constants ---
const float VCC = 5.0; 
const float FORMALDEHYDE_RL = 10000; // Load resistance
const int HEATER_VOLTAGE = 1800; 
float V0 = 1.0; // Initial voltage in clean air
const float R0 = 20.0; // Calibrated Resistance for MQ-137
#define RL 47

// --- Variables ---
float formalinPPM = 0.0;
float nitrogenVoltage = 0.0; 
float Rs; 
float ratio; 
float nitrogenPPM = 0.0; 
float temperature = 0.0; 
float humidity = 0.0; 

// --- Objects ---
DHT dht(DHTPIN, DHTTYPE);
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE); 
SoftwareSerial nodemcu(3, 4); // RX, TX

void setup() {
  u8g.begin();
  Serial.begin(9600);
  nodemcu.begin(9600);
  dht.begin(); 
  pinMode(BUZZER_PIN, OUTPUT); 
  pinMode(VOUT_PIN, INPUT);
  
  // Initial Display
  digitalWrite(BUZZER_PIN, HIGH);
  displayAquaScent();
  delay(2000); 
  digitalWrite(BUZZER_PIN, LOW); // Turn off after beep
}

void loop() {
  // 1. Formaldehyde Calculation
  float Vs = analogRead(VOUT_PIN) * (VCC/1023.0);
  float vs_vo_ratio = Vs/V0;
  formalinPPM = calculateFormaldehydeConcentration(vs_vo_ratio);

  // 2. Nitrogen (Ammonia) Calculation
  int nitrogenSensorValue = analogRead(A0);
  nitrogenVoltage = nitrogenSensorValue * (5.0/1023.0);
  Rs = ((5.0 * RL)/nitrogenVoltage) - RL;
  ratio = Rs/R0; 
  nitrogenPPM = pow(10, ((log10(ratio)-0.42)/-0.263));

  // 3. Environment
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // 4. Send to NodeMCU (Gateway)
  nodemcu.print("F:"); nodemcu.println(formalinPPM);
  nodemcu.print("N:"); nodemcu.println(nitrogenPPM);
  nodemcu.print("T:"); nodemcu.println(temperature);
  nodemcu.print("H:"); nodemcu.println(humidity);

  // 5. Local Display
  displayData(formalinPPM, nitrogenPPM, temperature, humidity);
  
  // 6. Alert Buzzer (Local)
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(1000);
}

// --- Helper Functions ---
float calculateFormaldehydeConcentration(float vs_vo_ratio) {
  if (vs_vo_ratio < 1.2) return 0.0;
  else if (vs_vo_ratio < 1.4) return (vs_vo_ratio-1.2)/(1.4-1.2)*(0.3-0.1)+0.1;
  else if (vs_vo_ratio < 1.6) return (vs_vo_ratio-1.4)/(1.6-1.4)*(0.4-0.3)+0.3;
  else if (vs_vo_ratio < 1.8) return (vs_vo_ratio-1.6)/(1.8-1.6)*(0.5-0.4)+0.4;
  else if (vs_vo_ratio < 2.0) return (vs_vo_ratio-1.8)/(2.0-1.8)*(0.6-0.5)+0.5;
  else if (vs_vo_ratio < 2.2) return (vs_vo_ratio-2.0)/(2.2-2.0)*(0.7-0.6)+0.6;
  else if (vs_vo_ratio < 2.4) return (vs_vo_ratio-2.2)/(2.4-2.2)*(0.8-0.7)+0.7;
  else if (vs_vo_ratio < 2.6) return (vs_vo_ratio-2.4)/(2.6-2.4)*(0.9-0.8)+0.8;
  else if (vs_vo_ratio < 2.8) return (vs_vo_ratio-2.6)/(2.8-2.6)*(1.0-0.9)+0.9;
  else return (vs_vo_ratio-2.8)/(3.0-2.8)*(1.2-1.0)+1.0;
}

void displayAquaScent() {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont22);
    u8g.drawStr(10, 40, "AQUASCENT");
  } while (u8g.nextPage());
}

void displayData(float f, float n, float t, float h) {
  u8g.firstPage();
  do {
    u8g.drawFrame(0, 0, u8g.getWidth(), u8g.getHeight());
    u8g.setFont(u8g_font_6x10);
    
    u8g.drawStr(5, 12, "Formalin :"); u8g.setPrintPos(68, 12); u8g.print(f); u8g.print(" ppm");
    u8g.drawStr(5, 27, "Nitrogen :"); u8g.setPrintPos(68, 27); u8g.print(n); u8g.print(" ppm");
    u8g.drawStr(5, 42, "Temp:"); u8g.setPrintPos(68, 42); u8g.print(t); u8g.print(" C");
    u8g.drawStr(5, 57, "Humidity:"); u8g.setPrintPos(68, 57); u8g.print(h); u8g.print("%");
    
    u8g.drawHLine(4, 14, u8g.getStrWidth("Formalin"));
    u8g.drawHLine(4, 29, u8g.getStrWidth("Nitrogen"));
  } while (u8g.nextPage());
}
