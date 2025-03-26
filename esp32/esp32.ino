#define APPLIANCE_ONE 22  
#define APPLIANCE_TWO 23  
#define TEMP_PIN 39       

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "wifi"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyCy5xj93dr2CIDGJZBLt_88kLlAbFS4UUg"
#define SECRET "mnNHMY8drHiDGEZkhlGqs4JERy8eTTXsUTdWeHOj"
#define DATABASE_URL "mini-iot-1-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  pinMode(APPLIANCE_ONE, OUTPUT);
  pinMode(APPLIANCE_TWO, OUTPUT);
  
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = SECRET;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  float currentTemp = readTemperature();

  if (Firebase.RTDB.getInt(&fbdo, "device/website/control")) {
    int controlState = fbdo.intData();
    
    if (controlState == 1 && currentTemp < 80.0) {
      digitalWrite(APPLIANCE_ONE, HIGH);
      digitalWrite(APPLIANCE_TWO, HIGH);
      Serial.println("Appliances ON (from web control, temp OK)");
    } else if (controlState == 0 || currentTemp >= 80.0) {
      digitalWrite(APPLIANCE_ONE, LOW);
      digitalWrite(APPLIANCE_TWO, LOW);
      if (currentTemp >= 80.0) {
        Serial.println("Appliances OFF (temp >= 80°C override)");
      } else {
        Serial.println("Appliances OFF (from web control)");
      }
    }
  } else {
    Serial.println("Failed to get control state: " + fbdo.errorReason());
  }

  if (currentTemp >= 80.0) {
    if (Firebase.RTDB.setString(&fbdo, "/alert", false)) {
      Serial.println("Alert sent: ร้อนเกิน" );
    } else {
      Serial.println("Failed to send alert: " + fbdo.errorReason());
    }
  } else {
    if (Firebase.RTDB.setString(&fbdo, "/alert", true)) {
      Serial.println("Alert cleared");
    }
  }

  if (Firebase.RTDB.setFloat(&fbdo, "device/sensor/currentTemp", currentTemp) &&
      Firebase.RTDB.setInt(&fbdo, "device/sensor/applianceState", digitalRead(APPLIANCE_ONE))) {
    Serial.println("Data updated successfully:");
    Serial.println("Current Temp: " + String(currentTemp) + "°C");
    Serial.println("Appliance State: " + String(digitalRead(APPLIANCE_ONE)));
  } else {
    Serial.println("Failed to update data: " + fbdo.errorReason());
  }

  delay(1000); 
}

float readTemperature() {
  int tempValue = analogRead(TEMP_PIN); 
  float temperature = map(tempValue, 0, 4095, 0, 100); 
  return temperature;
}