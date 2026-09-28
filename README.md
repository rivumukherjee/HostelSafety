# SafeHostel

SafeHostel is a hostel-safety demonstration using an ESP32 simulation in Wokwi, a local Python server, and a live browser dashboard. Sensor thresholds and alarm decisions stay in the ESP32 firmware. The dashboard shows the ESP32's readings and notifications; it does not simulate or replace the hardware logic.

## Demo architecture

```text
Wokwi ESP32 --HTTPS POST--> public tunnel --> FastAPI on this laptop
                                               |
                                               +-- WebSocket --> browser dashboard
```

The Wokwi circuit is kept as designed. It uses the MQ-2 gas sensor, DHT22 temperature/humidity sensor, LDR as a flame/light proxy, PIR motion sensor, pushbutton, LEDs, buzzer, and LCD. Motion is displayed for context only and does not trigger an alarm.

## Run the local server

Use **64-bit Python 3.13** (the project is pinned to this stable release) and `uv`. From the project root in PowerShell, sync the locked dependencies and start the server:

```powershell
uv sync --python 3.13
uv run uvicorn backend.main:app --host 0.0.0.0 --port 8000 --reload
```

Open <http://127.0.0.1:8000> (do not open `frontend/index.html` directly; the page uses the FastAPI WebSocket at `/ws`). Click **Enable alert sound** once; browsers require a user gesture before playing audio. The sound is generated in the browser and follows the reported buzzer state.

To use `pip` instead of `uv`, first ensure `python --version` reports Python 3.13.x (not a prerelease), then run:

```powershell
python -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r backend\requirements.txt
.\.venv\Scripts\python.exe -m uvicorn backend.main:app --host 0.0.0.0 --port 8000 --reload
```

## Connect Wokwi

1. Start the FastAPI server above.
2. Start a temporary tunnel to port `8000`, for example `ngrok http 8000` or `cloudflared tunnel --url http://localhost:8000`.
3. Copy the tunnel's public HTTPS URL and set `API_URL` in `esp32/sketch.ino` to that URL plus `/api/events`, for example `https://your-assigned-host.example/api/events`. Do not use the example host literally.
4. In the hosted Wokwi simulator, use its internet network `Wokwi-GUEST` with an empty password. The firmware is configured for this by default.
5. Run the simulation. The first packet should mark the device online; sensor and alert changes should appear live.

The firmware uses an insecure TLS client for the temporary demo tunnel because Wokwi's ESP32 certificate setup is not provisioned here. Use this only with a temporary demo endpoint and do not send secrets or use it for a production safety system. Never commit personal Wi-Fi credentials or a private tunnel URL.

The tunnel exposes an unauthenticated demo endpoint; anyone who obtains its URL can submit readings. Keep the tunnel temporary and stop it after the demonstration.

## Button and alarm behavior

- An automatic gas, temperature, or flame-proxy hazard activates the ESP32 buzzer and the appropriate warning/red LED. The LCD and dashboard display the reported detection/reason.
- Pressing the button while an alarm is sounding silences the buzzer and turns off the LEDs for five seconds. Sensor sampling continues. After five seconds, remaining hazards activate the automatic alarm again.
- Pressing the button while the buzzer is off activates a manual emergency: the buzzer and red LED turn on and the dashboard labels it as a manual emergency. Pressing the button while that alarm is sounding applies the same five-second mute; afterwards automatic sensor detection resumes.
- The browser buzzer is opt-in and mirrors `alarm`/`alarm_muted` from the ESP32. Muting the browser sound control only affects laptop audio, not the simulated circuit.

## API

- `GET /api/health`: server and recent-device status.
- `GET /api/state`: latest reading and in-memory event history.
- `POST /api/events`: sensor/alarm JSON from the ESP32.
- `WS /ws`: sends an initial snapshot and live snapshots as readings arrive.

Each event includes `status` (`SAFE`, `WARNING`, `HIGH`, `CRITICAL`, `FIRE ALERT`, or `MANUAL`), `reason`, `gas` (0–4095), `temperature` (-40–125 °C), `humidity` (0–100%), `pir`, `fire`, `risk_score` (0–100), `alarm`, `alarm_muted`, optional `manual_emergency`, and optional `mute_remaining_seconds` (0–5). The dashboard shows the ESP32-reported status and risk; it does not recalculate alarm decisions. Device telemetry is considered stale after 15 seconds, based on its reported `received_at` timestamp.

The history is intentionally in memory and is cleared when the server restarts. The ESP32 posts updates when its state changes and sends periodic heartbeats so the dashboard can identify stale telemetry.

## Test without Wokwi

With the server running, post a sample event from PowerShell:

```powershell
$body = @{
	status = "FIRE ALERT"; reason = "FIRE"; gas = 500
	temperature = 28.4; humidity = 58; pir = $false; fire = $true
	risk_score = 60; alarm = $true; alarm_muted = $false
	manual_emergency = $false; mute_remaining_seconds = 0
} | ConvertTo-Json
Invoke-RestMethod -Uri http://127.0.0.1:8000/api/events -Method Post -ContentType "application/json" -Body $body
```

## Libraries

The ESP32 sketch uses the Arduino-ESP32 core, `LiquidCrystal_I2C`, and `DHT sensor library` (with `Adafruit Unified Sensor`). See [esp32/libraries.txt](esp32/libraries.txt).

This is a simulation/demo prototype, not a certified life-safety system.
