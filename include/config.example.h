#pragma once

// Copy this file to include/config.h and fill in your values.

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// AP fallback if station mode cannot connect.
#define FALLBACK_AP_SSID "SecureHome-ESP32"
#define FALLBACK_AP_PASSWORD "ChangeMe123"

// Optional name shown in logs.
#define DEVICE_NAME "secure-home-node-01"

// Sensor GPIO pins (edit for your board wiring).
#define PIR_PIN 27
#define DOOR_PIN 14

// Door contact logic level.
// true means contact is OPEN when pin reads HIGH.
#define DOOR_OPEN_IS_HIGH true

// ─── Event upload integration ─────────────────────────────────────────────
// Set PLATFORM_URL to your hub server's IP to enable event upload.
// Leave empty ("") to log to Serial only.
#define PLATFORM_URL  "http://192.168.4.1/api/events"   // your hub server IP:port
#define API_KEY       "devkey1234"                       // must match your server's device API key
