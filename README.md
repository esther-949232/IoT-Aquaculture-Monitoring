# IoT-Based Smart Aquaculture Monitoring System (Aquascent)

## 1. Problem Statement
In the aquaculture industry, post-harvest losses due to spoilage and illegal adulteration (using Formalin) are major economic challenges. Traditional methods of checking fish freshness are subjective and slow. This project designs an IoT monitoring solution that monitors **Ammonia (Nitrogen)** and **Formaldehyde** levels in real-time to trigger alerts before spoilage occurs.

## 2. System Architecture
[cite_start]The system follows a 3-layer IoT architecture:
* **Sensing Layer (Edge):** Arduino Uno R3 connected to MQ-137 (Ammonia), Grove HCHO (Formaldehyde), and DHT22 sensors. It processes raw analog signals into PPM values locally.
* **Connectivity Layer (Gateway):** NodeMCU ESP8266 receives processed data via Serial communication and transmits it to the cloud via Wi-Fi.
* **Application Layer (Cloud):** Blynk IoT platform visualizes data and manages the Alert Engine.

![Architecture Diagram](architecture_diagram.png)


## [cite_start]3. Sensor List & Justification [cite: 3816-3829]
| Sensor | Purpose | Justification |
| :--- | :--- | :--- |
| **MQ-137** | Ammonia (NH3) Detection | Detects volatile nitrogen (TVB-N) released during protein breakdown. Spoilage threshold set at >320 ppm. |
| **Grove HCHO** | Formaldehyde Detection | Detects illegal adulteration. Used to identify if fish has been treated with carcinogenic preservatives. |
| **DHT22** | Temp & Humidity | Monitors storage conditions to ensure the cold chain ($4^{\circ}C$ or $-18^{\circ}C$) is maintained. |

## 4. Data Flow & Alert Logic
1.  **Acquisition:** Sensors read analog voltage every second.
2.  **Edge Processing:** Arduino converts voltage to PPM using calibrated resistance curves (Rs/R0).
3.  **Transmission:** Data is sent to NodeMCU as a string: `F:value,N:value,T:value,H:value`.
4.  **Cloud Sync:** NodeMCU parses the string and pushes data to Blynk Virtual Pins (V0-V3).
5.  **Alert Logic:**
    * **Condition:** `IF Nitrogen_PPM > 320`
    * **Action:** Trigger `Blynk.logEvent("spoilage_alert")`.
    * **Result:** Trader receives an automated email: "High nitrogen levels detected. Immediate action needed."

## 5. Sample Output
![Dashboard Alert](dashboard_alert.png)
