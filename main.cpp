
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ---------- Wi-Fi and MQTT ----------
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

const char* MQTT_SERVER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

// Use a unique prefix if you share this public MQTT broker.
// Example: streetlight/suman2026
const char* TOPIC_BASE = "streetlight/suman2026";

// ---------- Hardware pins ----------
const int LDR_PIN = 34;
const int POLE1_PIN = 21;
const int POLE2_PIN = 19;
const int POLE3_PIN = 18;

// Adjust this threshold after checking Serial Monitor readings.
const int DARK_THRESHOLD = 2000;

// ---------- System state ----------
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

bool autoMode = true;
bool pole1On = false;
bool pole2On = false;
bool pole3On = false;

unsigned long lastPublish = 0;
const unsigned long PUBLISH_INTERVAL = 2000;

// ---------- Topic helper ----------
String topic(const char* suffix) {
  return String(TOPIC_BASE) + "/" + suffix;
}

// ---------- Apply outputs ----------
void applyLights() {
  digitalWrite(POLE1_PIN, pole1On ? HIGH : LOW);
  digitalWrite(POLE2_PIN, pole2On ? HIGH : LOW);
  digitalWrite(POLE3_PIN, pole3On ? HIGH : LOW);
}

// ---------- Publish MQTT message ----------
void publishMessage(const String& suffix, const String& message) {
  if (mqtt.connected()) {
    mqtt.publish(topic(suffix.c_str()).c_str(), message.c_str());
  }
}

// ---------- Automatic lighting ----------
void updateAutomaticLights() {
  if (!autoMode) return;

  int lightLevel = analogRead(LDR_PIN);
  bool dark = lightLevel < DARK_THRESHOLD;

  pole1On = dark;
  pole2On = dark;
  pole3On = dark;

  applyLights();
}

// ---------- MQTT command handler ----------
void mqttCallback(char* incomingTopic, byte* payload,
                  unsigned int length) {
  String receivedTopic = String(incomingTopic);
  String message = "";

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  message.trim();
  message.toLowerCase();

  Serial.print("MQTT command: ");
  Serial.print(receivedTopic);
  Serial.print(" = ");
  Serial.println(message);

  if (receivedTopic == topic("mode")) {
    if (message == "auto") {
      autoMode = true;
      updateAutomaticLights();
    } else if (message == "manual") {
      autoMode = false;
    }
  } else if (receivedTopic == topic("pole1")) {
    if (message == "on") pole1On = true;
    else if (message == "off") pole1On = false;
  } else if (receivedTopic == topic("pole2")) {
    if (message == "on") pole2On = true;
    else if (message == "off") pole2On = false;
  } else if (receivedTopic == topic("pole3")) {
    if (message == "on") pole3On = true;
    else if (message == "off") pole3On = false;
  }

  // Apply individual pole commands only in manual mode.
  if (!autoMode) {
    applyLights();
  }

  publishMessage("currentMode", autoMode ? "auto" : "manual");
}

// ---------- Wi-Fi connection ----------
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi connection timed out.");
  }
}

// ---------- MQTT connection ----------
void connectMQTT() {
  if (mqtt.connected() || WiFi.status() != WL_CONNECTED) {
    return;
  }

  String clientId = "StreetLight-";
  clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

  Serial.print("Connecting to MQTT... ");

  if (mqtt.connect(clientId.c_str())) {
    Serial.println("connected!");

    mqtt.subscribe(topic("mode").c_str());
    mqtt.subscribe(topic("pole1").c_str());
    mqtt.subscribe(topic("pole2").c_str());
    mqtt.subscribe(topic("pole3").c_str());

    publishMessage("currentMode", autoMode ? "auto" : "manual");
  } else {
    Serial.print("failed, state = ");
    Serial.println(mqtt.state());
  }
}

// ---------- Publish sensor and pole status ----------
void publishStatus() {
  int lightLevel = analogRead(LDR_PIN);

  publishMessage("sensor/ldr", String(lightLevel));
  publishMessage("currentMode", autoMode ? "auto" : "manual");
  publishMessage("pole1/status", pole1On ? "on" : "off");
  publishMessage("pole2/status", pole2On ? "on" : "off");
  publishMessage("pole3/status", pole3On ? "on" : "off");

  Serial.print("LDR: ");
  Serial.print(lightLevel);
  Serial.print(" | Mode: ");
  Serial.print(autoMode ? "AUTO" : "MANUAL");
  Serial.print(" | Pole 1: ");
  Serial.print(pole1On ? "ON" : "OFF");
  Serial.print(" | Pole 2: ");
  Serial.print(pole2On ? "ON" : "OFF");
  Serial.print(" | Pole 3: ");
  Serial.println(pole3On ? "ON" : "OFF");
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);

  pinMode(POLE1_PIN, OUTPUT);
  pinMode(POLE2_PIN, OUTPUT);
  pinMode(POLE3_PIN, OUTPUT);

  analogReadResolution(12);

  applyLights();

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);

  connectWiFi();
  connectMQTT();

  Serial.println("Adaptive Smart Street Lighting started.");
}

// ---------- Main loop ----------
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqtt.connected()) {
    connectMQTT();
  }

  mqtt.loop();

  updateAutomaticLights();

  if (millis() - lastPublish >= PUBLISH_INTERVAL) {
    lastPublish = millis();
    publishStatus();
  }

  delay(50);
}
