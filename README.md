# SecureHomeStarter — Basic Sensor Monitor

**Status:** Complete  
**Board:** ESP32 dev board  
**Role:** Lightweight starter project — PIR motion + door sensor with direct OPERATOR POST

---

## What It Does

SecureHomeStarter is the minimal version of the sensor concept. It monitors a PIR motion sensor and a door/window reed switch, then POSTs JSON events to OPERATOR (or any HTTP endpoint) on state change. No SPIFFS buffering, no OTA, no OLED — just the sensor logic and a `/status` endpoint.

This project is useful as a reference implementation or starting point for new sensor nodes.

---

## Hardware

| Item | Notes |
|---|---|
| ESP32 dev board | Any standard 38-pin ESP32 |
| PIR sensor | e.g. HC-SR501 → GPIO 27 (configurable) |
| Reed switch | Normally-closed → GPIO 14 (configurable) |

No OLED required for this project.

---

## Wiring

```
PIR OUT     → GPIO 27
Door contact → GPIO 14
VCC/GND     → 3V3 / GND
```

---

## Configuration

Copy `include/config.example.h` → `include/config.h` and fill in:

```cpp
#define WIFI_SSID        "your-wifi"
#define WIFI_PASSWORD    "password"
#define FALLBACK_AP_SSID     "SecureHome-ESP32"
#define FALLBACK_AP_PASSWORD "ChangeMe123"    // change before deploying
#define DEVICE_NAME      "secure-home-node-01"
#define PIR_PIN          27
#define DOOR_PIN         14
#define DOOR_OPEN_IS_HIGH true   // true = HIGH when door open; false = NC reed (LOW = open)
#define PLATFORM_URL     "http://192.168.4.1/api/events"   // OPERATOR IP
#define API_KEY          "devkey1234"                       // must match OPERATOR's DEVICE_API_KEY
```

Leave `PLATFORM_URL` blank (`""`) to log to Serial only without uploading.

---

## Behaviour

- Connects to `WIFI_SSID` on boot; if unavailable, falls back to its own AP (`SecureHome-ESP32`)
- Debounces sensor inputs (80 ms)
- POSTs a JSON event to `PLATFORM_URL` on every state change:
  ```json
  {"device":"secure-home-node-01","type":"PIR","value":1,"ts":12345}
  ```
- Exposes `GET /status` — returns current sensor states and uptime as JSON

---

## Flash

```bash
cd ESP32Projects/SecureHomeStarter
pio run -t upload
pio device monitor   # 115200 baud
```

---

## Companion Tools

| Tool | Relationship |
|---|---|
| SENSOR_HUB | Full-featured version: OLED, SPIFFS buffering, tamper detection, OTA |
| OPERATOR | Receives events from this device via `/api/events` |
