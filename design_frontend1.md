# SafeHostel Frontend Design

## Purpose

A single-screen, live monitoring dashboard for the hostel safety prototype. Wokwi remains the simulated hardware and source of sensor readings; this page presents those readings and alerts from the local FastAPI server.

## Design choice

Keep the interface simple, readable, and operational rather than adding a large admin navigation or analytics views. Use a light neutral canvas, charcoal text, and restrained green/amber/red safety colors. The status and active alert appear before sensor details; event history stays visible below them. On small screens, the status and risk sections stack and sensors use a two-column grid.

## Page layout

1. **Header:** SafeHostel name, server connection indicator, and an explicit browser alert-sound enable/disable button.
2. **Current status:** system state, short explanation, last packet time, risk score, and a compact risk meter.
3. **Active alert:** detected hazard or manual emergency, reason, and whether the buzzer is on, off, or muted.
4. **Sensor feed:** gas ADC reading, temperature, humidity, flame proxy, passive motion, and device link state.
5. **Recent events:** latest state transitions from the backend's in-memory log.

## States and behavior

- `SAFE`: green status, no active alert, and alarm off.
- Sensor warning/high/critical or flame: amber/red alert with the reason reported by the ESP32.
- Manual emergency: red alert clearly labeled as manual.
- Muted: show that the five-second output mute is active; continue showing sensor detections. The browser sound stops while muted.
- Server disconnected or telemetry older than 15 seconds: show a connection/stale state rather than presenting old values as live.
- Browser sound starts only after a user explicitly enables it, to comply with autoplay restrictions. Web Audio generates a short repeating buzzer tone; it stops when the ESP32 reports the alarm off or muted.

## Data flow

The ESP32 posts JSON to `POST /api/events`. FastAPI stores the latest reading and a bounded, in-memory transition history, then pushes snapshots over `/ws`. The page is served by FastAPI on the same origin, so no separate frontend build or CORS configuration is needed.

## Deliberate scope

The PIR reading is displayed as passive information and does not affect risk or alarm state. There are no dashboard controls that pretend to change the simulated ESP32. The Wokwi circuit and pin mapping remain unchanged. This is a demonstration prototype, not a deployed or certified life-safety system.