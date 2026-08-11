#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

#include "config.h"

WebServer server(80);

unsigned long lastPirChangeMs = 0;
unsigned long lastDoorChangeMs = 0;
const unsigned long debounceMs = 80;

int pirStableState = LOW;
int doorStableState = LOW;

bool usingFallbackAp = false;

int readDoorPin() {
  int raw = digitalRead(DOOR_PIN);
  if (DOOR_OPEN_IS_HIGH) {
    return raw;
  }
  return raw == HIGH ? LOW : HIGH;
}

void logEvent(const char* eventName, int value) {
  JsonDocument doc;
  doc["device"] = DEVICE_NAME;
  doc["type"]   = eventName;
  doc["value"]  = value;
  doc["ts"]     = millis();

  serializeJson(doc, Serial);
  Serial.println();

  // POST to the hub server if in station mode and URL is configured
  if (!usingFallbackAp && strlen(PLATFORM_URL) > 0 &&
      WiFi.status() == WL_CONNECTED) {
    String payload = "[";
    String entry;
    serializeJson(doc, entry);
    payload += entry + "]";

    HTTPClient http;
    http.begin(PLATFORM_URL);
    http.addHeader("Content-Type", "application/json");
    if (strlen(API_KEY) > 0) {
      http.addHeader("X-API-Key", API_KEY);
    }
    int code = http.POST(payload);
    http.end();
    Serial.printf("[POST] %s → HTTP %d\n", eventName, code);
  }
}

void handleStatus() {
  JsonDocument doc;
  doc["device"] = DEVICE_NAME;
  doc["uptimeMs"] = millis();
  doc["pir"] = pirStableState;
  doc["doorOpen"] = doorStableState;
  doc["wifiMode"] = usingFallbackAp ? "ap" : "station";

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void startApFallback() {
  usingFallbackAp = true;
  WiFi.mode(WIFI_AP);
  bool apOk = WiFi.softAP(FALLBACK_AP_SSID, FALLBACK_AP_PASSWORD);

  Serial.print("AP fallback start: ");
  Serial.println(apOk ? "ok" : "failed");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}

void connectWifiOrFallback() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  const unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    usingFallbackAp = false;
    Serial.print("Station connected, IP: ");
    Serial.println(WiFi.localIP());
    return;
  }

  Serial.println("Station connect timeout, enabling AP fallback.");
  startApFallback();
}

void setup() {
  Serial.begin(115200);
  delay(250);

  pinMode(PIR_PIN, INPUT);
  pinMode(DOOR_PIN, INPUT_PULLUP);

  pirStableState = digitalRead(PIR_PIN);
  doorStableState = readDoorPin();

  connectWifiOrFallback();

  server.on("/status", HTTP_GET, handleStatus);
  server.begin();

  logEvent("boot", 1);
}

void loop() {
  server.handleClient();

  int pirNow = digitalRead(PIR_PIN);
  if (pirNow != pirStableState) {
    if (millis() - lastPirChangeMs >= debounceMs) {
      pirStableState = pirNow;
      logEvent("pir", pirStableState);
      lastPirChangeMs = millis();
    }
  } else {
    lastPirChangeMs = millis();
  }

  int doorNow = readDoorPin();
  if (doorNow != doorStableState) {
    if (millis() - lastDoorChangeMs >= debounceMs) {
      doorStableState = doorNow;
      logEvent("door", doorStableState);
      lastDoorChangeMs = millis();
    }
  } else {
    lastDoorChangeMs = millis();
  }

  delay(20);
}
