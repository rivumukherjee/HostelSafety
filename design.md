# SafeHostel Frontend Design Specification

## 1. Project Identity

**Project:** SafeHostel  
**Subtitle:** Smart Hostel Safety, Emergency Detection & Response System  
**Purpose:** A real-time web dashboard for monitoring a simulated hostel safety system running on an ESP32 in Wokwi.

The frontend receives safety events and sensor data from a Python backend. It must make dangerous conditions immediately understandable while still looking like a polished modern engineering/hackathon product.

### Core concept

> Sense. Analyze. Alert. Protect.

The interface should feel like a **real-time safety control dashboard**, not a generic admin panel.

---

# 2. Frontend Goals

The UI must allow a user to understand the system in less than 3 seconds.

The most important information hierarchy is:

1. **Current overall safety status**
2. **Active emergency/alarm**
3. **Reason for the alarm**
4. **Risk score**
5. **Sensor readings**
6. **Recent alert history**
7. **Connection/system health**

The interface should clearly distinguish:

- Normal / safe
- Warning
- High risk
- Critical / emergency
- Manual emergency
- Temporarily muted alarm
- Backend disconnected

Do not hide critical alerts inside tables or menus.

---

# 3. Design Direction

## Visual style

Use a modern **dark command-center / emergency monitoring** aesthetic.

Keywords:

- futuristic
- clean
- technical
- trustworthy
- high contrast
- minimal
- professional
- engineering dashboard
- real-time monitoring
- subtle glassmorphism
- restrained animations

Avoid:

- overly colorful dashboards
- cartoon-like illustrations
- excessive gradients
- excessive rounded cards
- generic SaaS appearance
- unnecessary decorative graphics
- complicated navigation

The dashboard should look appropriate for a college engineering hackathon demonstration.

---

# 4. Color System

Use dark neutral backgrounds with semantic status colors.

### Base colors

```text
Background:        #070B12
Secondary BG:      #0D131D
Card BG:           #111925
Elevated Card:     #151F2D
Border:            #243041
Primary Text:      #F4F7FB
Secondary Text:    #9AA7B8
Muted Text:        #647286
```

### Safety colors

```text
SAFE / GREEN:
#22C55E

WARNING / YELLOW:
#F59E0B

HIGH / ORANGE:
#F97316

CRITICAL / RED:
#EF4444

MANUAL EMERGENCY:
#DC2626

INFO / BLUE:
#3B82F6
```

Use safety colors primarily for status indicators, alerts, icons, borders, and important numbers.

Do not make the entire page red during an emergency. Instead, increase contrast and use a strong emergency banner/panel.

---

# 5. Typography

Use a clean modern sans-serif font.

Preferred:

```text
Inter
```

Fallback:

```text
system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif
```

For technical numbers or sensor values, a monospace font may be used:

```text
JetBrains Mono
```

### Typography hierarchy

```text
Page title:        28–36px, bold
Section heading:   18–22px, semibold
Card title:        13–15px, medium
Large metric:      28–40px, bold
Sensor value:      24–32px, semibold
Body:              14–16px
Small metadata:    11–13px
```

---

# 6. Overall Layout

Desktop-first layout:

```text
┌──────────────────────────────────────────────────────────────────────┐
│ SAFEHOSTEL                         ● SYSTEM ONLINE    Last update  │
│ Smart Hostel Safety & Response                                      │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│ ┌──────────────────────────────┐  ┌───────────────────────────────┐ │
│ │                              │  │                               │ │
│ │       SYSTEM STATUS          │  │       RISK SCORE              │ │
│ │                              │  │                               │ │
│ │       ● SAFE                 │  │          18 / 100             │ │
│ │       All systems normal     │  │      Low Risk                 │ │
│ │                              │  │                               │ │
│ └──────────────────────────────┘  └───────────────────────────────┘ │
│                                                                      │
│ ┌──────────────────────────────────────────────────────────────────┐ │
│ │ ACTIVE ALERT                                                     │ │
│ │ No active emergency                                              │ │
│ └──────────────────────────────────────────────────────────────────┘ │
│                                                                      │
│ SENSOR MONITORING                                                    │
│                                                                      │
│ ┌────────────┐ ┌────────────┐ ┌────────────┐ ┌────────────┐         │
│ │ GAS        │ │ TEMP       │ │ HUMIDITY   │ │ FIRE       │         │
│ │ 182 ppm    │ │ 27.4 °C    │ │ 61 %       │ │ CLEAR      │         │
│ └────────────┘ └────────────┘ └────────────┘ └────────────┘         │
│                                                                      │
│ ┌────────────┐ ┌────────────┐ ┌─────────────────────────────────┐  │
│ │ MOTION     │ │ EMERGENCY  │ │ SENSOR / DEVICE HEALTH          │  │
│ │ DETECTED   │ │ BUTTON     │ │ ESP32 ●  Wokwi ●  API ●         │  │
│ └────────────┘ └────────────┘ └─────────────────────────────────┘  │
│                                                                      │
│ ALERT HISTORY                                                        │
│ ┌──────────────────────────────────────────────────────────────────┐ │
│ │ TIME       EVENT             LEVEL       STATUS                   │ │
│ │ 14:42:08   Gas detected      HIGH        ACTIVE                   │ │
│ │ 14:39:21   Motion detected   INFO        RESOLVED                 │ │
│ └──────────────────────────────────────────────────────────────────┘ │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
```

---

# 7. Header

The header should remain visually simple.

### Left

Display:

**SAFEHOSTEL**

Below it:

**Smart Hostel Safety & Emergency Response**

Add a small shield/safety icon.

### Right

Display:

- connection indicator
- `SYSTEM ONLINE` / `SYSTEM OFFLINE`
- last update time
- optional settings/info button

Example:

```text
● SYSTEM ONLINE
Updated 2s ago
```

When disconnected:

```text
● BACKEND OFFLINE
Last update 18s ago
```

Do not use a fake online state. The frontend should derive this from actual backend/WebSocket state.

---

# 8. System Status Hero

This is the most important component.

## Safe state

```text
┌─────────────────────────────────────┐
│ SYSTEM STATUS                       │
│                                     │
│        ● SAFE                       │
│        All monitored conditions     │
│        are within normal range.     │
│                                     │
│ Last scan: 2 seconds ago            │
└─────────────────────────────────────┘
```

Use green status indicator.

## Warning state

```text
● WARNING

An abnormal condition has been detected.
Review sensor readings.
```

## High state

```text
● HIGH RISK

Immediate attention required.
```

## Emergency state

```text
● EMERGENCY

ACTIVE HAZARD DETECTED

Gas level above threshold
```

The emergency state should have a subtle pulsing border/glow, but avoid distracting flashing effects.

---

# 9. Risk Score Card

Display risk as a numerical score from 0–100.

Example:

```text
RISK SCORE

72 / 100

HIGH RISK

██████████████░░░░░░
```

Use a circular progress indicator or horizontal gauge.

Suggested interpretation:

```text
0–29    SAFE
30–49   WARNING
50–74   HIGH
75–100  CRITICAL
```

These ranges are frontend presentation categories and should be easy to change from configuration.

The backend/ESP32 remains the source of truth for the actual risk score.

Never calculate a different risk score in the frontend unless explicitly required.

---

# 10. Active Alert Panel

This panel is hidden/minimized when there is no active alert and becomes prominent during an event.

## No alert

```text
ACTIVE ALERT

✓ No active emergency
The monitoring system is operating normally.
```

## Gas alert

```text
⚠ ACTIVE ALERT

GAS DETECTED

Gas sensor level is above the configured threshold.

Source:
MQ-2 Gas Sensor

Alarm:
● ACTIVE
```

## Fire alert

```text
⚠ ACTIVE ALERT

FIRE / FLAME DETECTED

Immediate attention required.

Source:
Flame/Light Proxy Sensor

Alarm:
● ACTIVE
```

## Manual emergency

```text
🚨 MANUAL EMERGENCY

Emergency button activated.

This alert was triggered manually.
```

The frontend must distinguish automatic sensor alerts from manual emergency alerts.

---

# 11. Sensor Cards

Create reusable sensor cards.

Every card should contain:

1. Sensor icon
2. Sensor name
3. Current value
4. Unit/status
5. Normal/warning indicator
6. Optional tiny trend indicator
7. Last updated time

---

## Gas

```text
GAS LEVEL
MQ-2

182
ppm

● NORMAL
```

If dangerous:

```text
GAS LEVEL
MQ-2

HIGH

● ALERT
```

The exact unit should match the backend data. If the ESP32 sends a raw analog value rather than a calibrated ppm value, display `Raw Level` or `ADC` instead of inventing ppm.

---

## Temperature

```text
TEMPERATURE
DHT22

27.4
°C

● NORMAL
```

---

## Humidity

```text
HUMIDITY
DHT22

61
%

● NORMAL
```

---

## Fire / Flame

Because the Wokwi prototype uses the LDR/photoresistor as a flame/light proxy, label it honestly.

Recommended:

```text
FIRE / LIGHT PROXY
LDR

CLEAR

● NORMAL
```

When triggered:

```text
FIRE / LIGHT PROXY
LDR

DETECTED

● ALERT
```

Do not claim the simulation is using a certified flame detector.

---

## Motion

```text
MOTION
PIR

DETECTED

Last event: 14:42:08
```

Motion alone does not automatically activate the alarm in the current system design. The UI should therefore avoid displaying motion as an emergency unless the backend explicitly marks it as an alert.

---

## Emergency Button

```text
EMERGENCY BUTTON

READY

Manual trigger available
```

When pressed:

```text
EMERGENCY BUTTON

ACTIVATED

Manual emergency triggered
```

---

# 12. Alarm State

The dashboard should show whether the physical/simulated buzzer is active.

Possible states:

```text
ALARM ACTIVE
ALARM MUTED
ALARM OFF
```

Example:

```text
ALARM

● ACTIVE

Reason:
HIGH GAS LEVEL

Source:
Automatic Detection
```

When manually muted:

```text
ALARM

◼ MUTED

Manual button pressed

Auto detection resumes in:
3s
```

The 5-second mute behavior should be visible.

When the mute expires:

```text
ALARM

● ACTIVE

Hazard still detected
```

If the hazard disappeared during the mute period:

```text
ALARM

● OFF

No active hazard
```

---

# 13. Real-Time Alert Banner

When a new emergency event arrives, display a prominent banner near the top of the dashboard.

Example:

```text
┌──────────────────────────────────────────────────────────────────┐
│ 🚨  EMERGENCY DETECTED                                           │
│     High gas level detected by MQ-2 sensor                       │
│                                             14:42:08             │
└──────────────────────────────────────────────────────────────────┘
```

Animation:

- slide/fade into view
- subtle pulse
- do not continuously shake the entire page

The banner should disappear or become historical when the event is resolved.

---

# 14. Alert History

Create a chronological event table.

Columns:

```text
TIME
EVENT
SOURCE
SEVERITY
STATUS
```

Example:

```text
14:42:08   Gas detected       MQ-2       HIGH      ACTIVE
14:41:52   Alarm muted        Button     INFO      RESOLVED
14:39:21   Motion detected    PIR        INFO      RESOLVED
14:35:02   System started     ESP32      INFO      RESOLVED
```

Use badges for severity:

```text
INFO
WARNING
HIGH
CRITICAL
EMERGENCY
```

The newest event should appear first.

On mobile, transform rows into stacked event cards instead of forcing a wide table.

---

# 15. System Health

Add a compact system health section.

Show:

```text
ESP32              ● ONLINE
WOKWI SIMULATION    ● CONNECTED
BACKEND API         ● ONLINE
WEBSOCKET           ● CONNECTED
LAST EVENT          2s AGO
```

If the WebSocket disconnects:

```text
WEBSOCKET           ● DISCONNECTED
```

The frontend should visibly indicate stale data.

Example:

```text
DATA STALE
Last received update: 21 seconds ago
```

---

# 16. Connection States

Implement explicit frontend states.

## Connected

```text
● SYSTEM ONLINE
Live monitoring active
```

## Connecting

```text
◌ CONNECTING
Establishing live connection...
```

## Disconnected

```text
● SYSTEM OFFLINE
Unable to receive live sensor data
```

## Reconnecting

```text
↻ RECONNECTING
Attempting to restore connection...
```

Do not leave the dashboard looking normal if the backend is disconnected.

---

# 17. Audio Alert

The laptop/browser should reproduce the alarm state from the backend.

Important:

Browsers may block automatic audio playback until the user interacts with the page.

Therefore include a clear first-use control:

```text
🔊 ENABLE ALERT SOUND
```

After activation:

```text
🔊 ALERT SOUND ENABLED
```

The sound should:

- start when `alarm_active = true`
- stop when `alarm_active = false`
- stop when `alarm_muted = true`
- restart if a new active hazard occurs
- not play continuously when the system is safe

Do not make the sound the only indication of danger. The visual UI must always communicate the alarm state.

---

# 18. Notification Behavior

For important events, show browser-style in-app notifications.

Example:

```text
┌───────────────────────────────┐
│ 🚨 Emergency Detected         │
│ High gas level detected       │
│ 14:42:08                      │
└───────────────────────────────┘
```

Notification priority:

### Critical

- emergency banner
- alert sound
- visual status change
- event history entry

### High

- alert banner
- alert sound if alarm is active
- status change
- event history entry

### Warning

- warning banner/card
- event history entry

### Info

- history entry
- subtle toast notification

---

# 19. Optional Live Sensor Chart

Include a compact real-time chart section below the primary monitoring area.

Possible charts:

- Gas level over time
- Temperature over time
- Humidity over time
- Risk score over time

Do not make charts the primary UI.

During an emergency, the current status must remain more visually prominent than historical graphs.

Use Chart.js if a chart library is desired.

---

# 20. Responsive Design

The dashboard must work on:

- laptop
- desktop monitor
- tablet
- mobile phone

## Desktop

Use a multi-column grid.

Suggested:

```text
Main status: 2 columns
Risk score: 1 column
Sensor cards: 4 columns
Secondary cards: 2–3 columns
```

## Tablet

Use:

```text
2-column sensor grid
```

## Mobile

Use:

```text
1-column layout
```

Order on mobile:

1. Header
2. System status
3. Active alert
4. Risk score
5. Gas
6. Temperature
7. Humidity
8. Fire/light proxy
9. Motion
10. Emergency button
11. Alarm state
12. System health
13. Alert history

Critical information must never require horizontal scrolling.

---

# 21. Navigation

Keep navigation minimal.

For a single-page hackathon dashboard, prefer:

```text
SAFEHOSTEL
Overview
Alerts
System
```

Anchor navigation can scroll to sections.

Do not build a complicated multi-page admin system unless necessary.

---

# 22. Component Architecture

Recommended React-style component breakdown if React is used:

```text
App
├── Header
├── ConnectionStatus
├── EmergencyBanner
├── SystemStatusCard
├── RiskScoreCard
├── AlarmStatusCard
├── SensorGrid
│   ├── GasSensorCard
│   ├── TemperatureCard
│   ├── HumidityCard
│   ├── FireSensorCard
│   ├── MotionCard
│   └── EmergencyButtonCard
├── SystemHealth
├── AlertHistory
├── SensorCharts
├── AudioControl
└── ToastContainer
```

If plain HTML/CSS/JS is used, keep the same conceptual component separation.

---

# 23. Frontend Data Model

The backend should provide a payload approximately like:

```json
{
  "status": "WARNING",
  "reason": "HIGH_GAS",
  "gas": 620,
  "temperature": 28.4,
  "humidity": 63,
  "pir": true,
  "fire": false,
  "risk_score": 64,
  "alarm_active": true,
  "alarm_muted": false,
  "mute_remaining": 0,
  "timestamp": "2026-09-27T14:42:08"
}
```

The frontend should be prepared for missing optional fields.

Never crash the dashboard because one sensor value is unavailable.

---

# 24. Event Model

For event history, support events such as:

```json
{
  "event": "GAS_DETECTED",
  "source": "MQ2",
  "severity": "HIGH",
  "message": "High gas level detected",
  "timestamp": "2026-09-27T14:42:08"
}
```

Possible events:

```text
SYSTEM_STARTED
SYSTEM_CONNECTED
SYSTEM_DISCONNECTED
GAS_DETECTED
GAS_CLEARED
FIRE_DETECTED
FIRE_CLEARED
HIGH_TEMPERATURE
TEMPERATURE_CLEARED
MOTION_DETECTED
MANUAL_EMERGENCY
ALARM_MUTED
ALARM_RESUMED
ALERT_RESOLVED
```

The UI should gracefully handle future event types.

---

# 25. WebSocket Behavior

Use WebSocket for real-time updates if available.

Conceptual flow:

```text
Python Backend
      │
      │ WebSocket
      ▼
Frontend
      │
      ├── update system status
      ├── update sensor cards
      ├── update risk score
      ├── update alarm
      ├── update alert history
      └── trigger sound/notification
```

When a message arrives:

1. Validate the payload.
2. Update current dashboard state.
3. Determine whether the event is new.
4. Add a new history entry if required.
5. Update alarm state.
6. Trigger audio only when appropriate.
7. Update connection/last-update timestamp.

---

# 26. UI State Machine

The frontend should visually support these main states:

```text
SAFE
  ↓
WARNING
  ↓
HIGH
  ↓
CRITICAL / EMERGENCY
```

And:

```text
EMERGENCY
   ↓
ALARM MUTED
   ↓
5 SECOND TIMER
   ↓
HAZARD STILL ACTIVE → ALARM ACTIVE
   OR
HAZARD CLEARED → SAFE
```

Manual emergency should be visually distinct from automatic sensor detection.

---

# 27. Emergency Mode

When an emergency is active:

Increase visual urgency without destroying usability.

Changes:

- system status becomes red
- active alert panel expands
- risk score changes to emergency state
- alarm indicator becomes active
- relevant sensor card gets highlighted
- new event appears in history
- browser alarm sound starts if enabled
- subtle red glow/pulse around the active alert

Do NOT:

- flash the whole screen rapidly
- use unreadable red text everywhere
- cover the entire dashboard with a modal
- hide sensor readings
- require multiple clicks to understand the cause

---

# 28. Accessibility

Follow basic accessibility principles.

Requirements:

- sufficient text/background contrast
- never communicate status using color alone
- use icons + text
- buttons must have visible labels
- keyboard accessible controls
- visible focus states
- meaningful `aria-label`s where needed
- alert messages should be accessible to screen readers
- avoid rapid flashing animations

Example:

Bad:

```text
●
```

Better:

```text
● SYSTEM ONLINE
```

---

# 29. Animation Guidelines

Use animation only to communicate change.

Recommended:

- card value transition: 150–250ms
- alert entrance: 200–350ms
- status transition: 200ms
- emergency pulse: slow 1–2 second cycle
- toast: slide/fade

Avoid:

- constant bouncing
- spinning icons everywhere
- excessive parallax
- decorative animations
- rapidly flashing emergency effects

---

# 30. Empty and Loading States

## Initial loading

```text
SAFEHOSTEL

Connecting to safety monitoring system...

◌ Establishing secure connection
```

## No alerts

```text
ALERT HISTORY

✓ No safety events recorded
```

## No sensor data

```text
WAITING FOR SENSOR DATA
```

## Backend unavailable

```text
BACKEND OFFLINE

Live sensor updates are currently unavailable.
Retrying connection...
```

---

# 31. Error Handling

Errors should be understandable to a student/demo audience.

Bad:

```text
WebSocket error code 1006
```

Better:

```text
LIVE CONNECTION LOST

The dashboard is not receiving new sensor data.

Attempting to reconnect...
```

Developer details can be logged to the browser console, but should not dominate the UI.

---

# 32. Demo Mode

Because this project is intended for a hackathon demonstration, the interface should make state changes highly visible.

Recommended demo sequence:

### Step 1 — Safe

Show:

```text
SYSTEM ONLINE
SAFE
Risk: low
Gas: normal
Temperature: normal
Fire: clear
Alarm: OFF
```

### Step 2 — Gas event

Wokwi gas level increases.

Dashboard changes immediately:

```text
HIGH RISK
GAS DETECTED
Alarm: ACTIVE
```

Laptop alert sound starts.

### Step 3 — Fire/light event

Show:

```text
EMERGENCY
FIRE / LIGHT PROXY DETECTED
```

### Step 4 — Manual emergency

Show:

```text
MANUAL EMERGENCY
Emergency button activated
```

### Step 5 — Mute

Show:

```text
ALARM MUTED
Auto detection resumes in 5s
```

### Step 6 — Resume

If the hazard still exists:

```text
ALARM ACTIVE
Hazard still detected
```

This sequence should be visually obvious even when projected on a large screen.

---

# 33. Project Branding

Use:

**SAFEHOSTEL**

Possible supporting text:

> Smart Hostel Safety & Emergency Detection

Primary tagline:

> Sense. Analyze. Alert. Protect.

Optional footer:

```text
SafeHostel • ESP32 + Wokwi • Real-Time Safety Monitoring
```

Do not over-brand the dashboard.

---

# 34. Iconography

Use a consistent outline icon set.

Suggested icons:

```text
Shield          → system
Activity        → monitoring
Flame           → fire
Cloud/Database  → backend
Thermometer     → temperature
Droplets        → humidity
Wind / Gauge    → gas
Person          → motion
Bell            → alarm
AlertTriangle   → warning
CircleCheck     → safe
Wifi            → connection
Clock           → timestamp
```

Lucide Icons is a good choice if an icon library is needed.

---

# 35. Suggested Page Structure

```text
<App>

  <Header />

  <main>

    <TopStatusGrid>
      <SystemStatusCard />
      <RiskScoreCard />
    </TopStatusGrid>

    <EmergencyBanner />

    <Section title="Live Monitoring">
      <SensorGrid />
    </Section>

    <Section title="Alarm & Response">
      <AlarmStatusCard />
      <EmergencyButtonCard />
    </Section>

    <Section title="System Health">
      <SystemHealth />
    </Section>

    <Section title="Alert History">
      <AlertHistory />
    </Section>

    <Section title="Trends">
      <SensorCharts />
    </Section>

  </main>

  <Footer />

</App>
```

---

# 36. Important Product Rules

1. **ESP32 is the source of truth for sensor state.**
2. Frontend should display received data rather than inventing sensor values.
3. Never fake a live connection.
4. Never label the LDR simulation as a certified flame sensor.
5. Manual emergency must be visually distinguishable from automatic detection.
6. Alarm mute state must be visible.
7. The 5-second mute countdown must be visible.
8. If a hazard remains after the mute period, the alarm must visually return to active.
9. Sensor cards must update without full-page reloads.
10. New alerts must appear in history immediately.
11. Connection loss must be obvious.
12. Critical information must remain readable on a projector.
13. The UI should remain functional even if optional charts fail to load.
14. Do not block the entire dashboard with an emergency modal.
15. Keep the interface focused on monitoring and response.

---

# 37. LLM Implementation Prompt

When using another LLM to generate the frontend, use this design document as the primary design specification.

The LLM should:

- build the dashboard according to this document
- preserve the SafeHostel terminology
- use semantic safety states
- implement responsive design
- implement real-time WebSocket updates
- implement REST/API fallback if required
- implement alert history
- implement audio alert controls
- implement the 5-second alarm mute display
- implement connection states
- use reusable components
- avoid hardcoded fake live data in production mode
- provide a clearly separated mock/demo data mode if needed
- keep the UI polished enough for a hackathon presentation
- make the emergency state immediately visible
- avoid adding unrelated pages or features

### Preferred implementation stack

```text
HTML
CSS
JavaScript
Python FastAPI backend
WebSocket
Chart.js (optional)
Lucide Icons (optional)
```

React may be used if the project already uses React, but it is not required.

---

# 38. Final Visual Target

The finished dashboard should communicate this within seconds:

```text
SAFEHOSTEL

● SYSTEM ONLINE

        SYSTEM STATUS
            SAFE

        RISK SCORE
           18/100

        LIVE MONITORING

   GAS       TEMP      HUMIDITY      FIRE
   182       27.4°C       61%        CLEAR
  NORMAL     NORMAL      NORMAL      NORMAL

        ALARM
          OFF

        SYSTEM HEALTH
 ESP32 ●   API ●   WebSocket ●

        ALERT HISTORY
        No active emergencies
```

During an emergency it should transform into:

```text
SAFEHOSTEL

● SYSTEM ONLINE

🚨 ACTIVE EMERGENCY
HIGH GAS LEVEL DETECTED
MQ-2 • 14:42:08

        SYSTEM STATUS
          EMERGENCY

        RISK SCORE
           82/100

        GAS
          HIGH
        ● ALERT

        ALARM
          ACTIVE
        🔊 ALERT SOUND

        RECENT EVENTS
        14:42:08  GAS DETECTED
```

The visual hierarchy must make the emergency state unmistakable without making the interface chaotic.
