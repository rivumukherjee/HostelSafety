# SafeHostel Demo Script

## Before the demo

1. Start the FastAPI server and temporary tunnel using the steps in the project README.
2. Set the tunnel URL in `esp32/sketch.ino`, then start the Wokwi simulation.
3. Open the dashboard and click **Enable alert sound**.
4. Confirm that telemetry is arriving and the dashboard reports the device online.

## Demonstration sequence

1. **Normal monitoring:** show safe status, sensor values, passive motion reading, and the green LED/LCD in Wokwi.
2. **Automatic hazard:** raise the gas or temperature reading, or trigger the LDR flame proxy in Wokwi. Point out the detected reason on the LCD/dashboard, risk score, active buzzer, and lit warning/red LED.
3. **Mute and resume:** press the simulated button while the buzzer is sounding. Show the buzzer and LEDs turn off and the dashboard report the five-second mute. Keep the hazard present; after five seconds, show automatic alarm detection resume.
4. **Manual emergency:** return the system to a state with the buzzer off, then press the button. Show the manual emergency message, buzzer, red LED, and dashboard notification. Press again to mute for five seconds; after the pause, automatic sensor detection resumes.
5. **Recovery:** clear the sensor hazard in Wokwi and show the dashboard return to safe with the alarm off.

## If Wokwi cannot reach the server

Check that the public tunnel is running, `API_URL` includes `/api/events`, the URL is HTTPS, and the simulation uses `Wokwi-GUEST` with a blank password. Confirm that the local dashboard still opens at `http://127.0.0.1:8000`.

This is a simulation prototype for demonstration only, not a certified hostel alarm system.