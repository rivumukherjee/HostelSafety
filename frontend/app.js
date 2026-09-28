const byId = (id) => document.getElementById(id);
const elements = {
	connection: byId("connection"),
	socketState: byId("socket-state"),
	statusPanel: byId("status-panel"),
	statusText: byId("status-text"),
	statusDetail: byId("status-detail"),
	lastUpdate: byId("last-update"),
	alertBanner: byId("alert-banner"),
	alertTitle: byId("alert-title"),
	alertReason: byId("alert-reason"),
	alarmState: byId("alarm-state"),
	riskValue: byId("risk-value"),
	riskLabel: byId("risk-label"),
	riskFill: byId("risk-fill"),
	riskTrack: document.querySelector(".risk-track"),
	gasValue: byId("gas-value"),
	gasState: byId("gas-state"),
	temperatureValue: byId("temperature-value"),
	temperatureState: byId("temperature-state"),
	humidityValue: byId("humidity-value"),
	humidityState: byId("humidity-state"),
	fireValue: byId("fire-value"),
	fireState: byId("fire-state"),
	motionValue: byId("motion-value"),
	deviceState: byId("device-state"),
	deviceDetail: byId("device-detail"),
	history: byId("history-list"),
	eventCount: byId("event-count"),
	soundToggle: byId("sound-toggle"),
};

let socket;
let reconnectDelay = 1000;
let latestState;
let soundEnabled = false;
let audioContext;
let beepTimer;
let lastReceivedAt;

function setConnection(state, label) {
	elements.connection.dataset.state = state;
	elements.connection.lastElementChild.textContent = label;
	elements.socketState.textContent = state === "online" ? "Live stream connected" : "Waiting for local server";
}

function formatTime(value) {
	if (!value) return "--";
	return new Intl.DateTimeFormat(undefined, { hour: "2-digit", minute: "2-digit", second: "2-digit" }).format(new Date(value));
}

function setSensorState(name, state) {
	document.querySelector(`[data-sensor="${name}"]`).dataset.state = state;
}

function updateDashboard(state, history) {
	latestState = state;
	lastReceivedAt = state?.received_at ? Date.parse(state.received_at) : undefined;
	if (state) {
		const status = state.status || "UNKNOWN";
		const manual = Boolean(state.manual_emergency);
		const muted = Boolean(state.alarm_muted);
		const alarming = Boolean(state.alarm);
		const hasHazard = status !== "SAFE";
		const level = manual || state.fire || ["CRITICAL", "FIRE ALERT", "HIGH"].includes(status) ? "danger" : hasHazard ? "warning" : "safe";
		const risk = Math.max(0, Math.min(100, Number(state.risk_score) || 0));

		elements.statusPanel.dataset.level = level;
		elements.statusText.textContent = manual ? "MANUAL EMERGENCY" : status;
		elements.statusDetail.textContent = muted ? `Alarm outputs paused; sensor monitoring continues for ${state.mute_remaining_seconds || 0}s.` : manual ? "Emergency button activated. Manual alarm is active." : hasHazard ? `Detected: ${state.reason || "Hazard"}` : "No hazards detected. Monitoring is active.";
		elements.lastUpdate.textContent = `Updated ${formatTime(state.received_at)}`;

		elements.alertBanner.dataset.level = muted ? "warning" : manual || state.fire || ["CRITICAL", "FIRE ALERT", "HIGH"].includes(status) ? "danger" : hasHazard ? "warning" : "safe";
		elements.alertTitle.textContent = muted ? "Alarm temporarily silenced" : manual ? "Manual emergency active" : alarming ? `${status} alarm active` : hasHazard ? `${status} detected` : "No active alert";
		elements.alertReason.textContent = muted ? `Detected condition: ${state.reason || "Manual emergency"}. Alarm outputs resume after the mute period.` : manual ? "Emergency button was pressed while the alarm was off." : hasHazard ? `Detected: ${state.reason || "Hazard"}. Risk score ${risk}.` : "All monitored readings are within the configured limits.";
		elements.alarmState.textContent = muted ? "MUTED" : alarming ? "ALARM ON" : "ALARM OFF";
		elements.alarmState.dataset.state = muted ? "muted" : alarming ? "on" : "off";

		elements.riskValue.textContent = risk;
		elements.riskLabel.textContent = risk >= 100 ? "CRITICAL" : risk >= 60 ? "HIGH" : risk >= 30 ? "WARNING" : "LOW";
		elements.riskFill.style.width = `${risk}%`;
		elements.riskFill.style.background = risk >= 60 ? "var(--red)" : risk >= 30 ? "var(--amber)" : "var(--green)";
		elements.riskTrack.setAttribute("aria-valuenow", String(risk));

		elements.gasValue.textContent = Number(state.gas).toLocaleString();
		elements.gasState.textContent = state.gas >= 2800 ? "Critical level" : state.gas >= 1800 ? "Elevated level" : "Normal range";
		setSensorState("gas", state.gas >= 2800 ? "alert" : state.gas >= 1800 ? "warning" : "normal");

		elements.temperatureValue.innerHTML = `${Number(state.temperature).toFixed(1)}<small>°C</small>`;
		elements.temperatureState.textContent = state.temperature >= 45 ? "Critical level" : state.temperature >= 35 ? "Elevated level" : "Normal range";
		setSensorState("temperature", state.temperature >= 45 ? "alert" : state.temperature >= 35 ? "warning" : "normal");

		elements.humidityValue.innerHTML = `${Number(state.humidity).toFixed(0)}<small>%</small>`;
		elements.humidityState.textContent = "DHT22 reading";
		elements.fireValue.textContent = state.fire ? "DETECTED" : "CLEAR";
		elements.fireState.textContent = "LDR flame/light proxy";
		setSensorState("fire", state.fire ? "alert" : "normal");
		elements.motionValue.textContent = state.pir ? "MOTION" : "CLEAR";

		elements.deviceState.textContent = "ONLINE";
		elements.deviceDetail.textContent = `Last packet ${formatTime(state.received_at)}`;
		document.querySelector(".device-sensor").dataset.state = "online";
		setAlarmSound(alarming && !muted);
	} else {
		setAlarmSound(false);
	}
	renderHistory(history || []);
}

function renderHistory(events) {
	elements.eventCount.textContent = `${events.length} event${events.length === 1 ? "" : "s"}`;
	if (!events.length) {
		elements.history.innerHTML = '<tr class="empty-row"><td colspan="3">Events will appear when the device reports a state change.</td></tr>';
		return;
	}
	elements.history.replaceChildren(...events.map((event) => {
		const row = document.createElement("tr");
		if (event.alarm || event.manual_emergency) row.className = "event-active";
		else if (event.alarm_muted) row.className = "event-muted";
		const time = document.createElement("td");
		time.textContent = formatTime(event.time);
		const message = document.createElement("td");
		message.textContent = event.message;
		const state = document.createElement("td");
		state.textContent = event.alarm_muted ? "MUTED" : event.alarm ? "ACTIVE" : "LOGGED";
		row.append(time, message, state);
		return row;
	}));
}

function connect() {
	const scheme = location.protocol === "https:" ? "wss:" : "ws:";
	socket = new WebSocket(`${scheme}//${location.host}/ws`);
	socket.addEventListener("open", () => {
		reconnectDelay = 1000;
		setConnection("online", "Server online");
	});
	socket.addEventListener("message", (message) => {
		const data = JSON.parse(message.data);
		if (data.type === "snapshot") updateDashboard(data.current, data.history);
	});
	socket.addEventListener("close", () => {
		setConnection("offline", "Reconnecting");
		window.setTimeout(connect, reconnectDelay);
		reconnectDelay = Math.min(reconnectDelay * 2, 10000);
	});
	socket.addEventListener("error", () => socket.close());
}

function beep() {
	if (!audioContext || audioContext.state !== "running") return;
	const oscillator = audioContext.createOscillator();
	const gain = audioContext.createGain();
	oscillator.type = "square";
	oscillator.frequency.value = 880;
	gain.gain.setValueAtTime(0.0001, audioContext.currentTime);
	gain.gain.exponentialRampToValueAtTime(0.12, audioContext.currentTime + 0.015);
	gain.gain.exponentialRampToValueAtTime(0.0001, audioContext.currentTime + 0.16);
	oscillator.connect(gain);
	gain.connect(audioContext.destination);
	oscillator.start();
	oscillator.stop(audioContext.currentTime + 0.17);
}

function setAlarmSound(active) {
	if (active && soundEnabled && !beepTimer) {
		beep();
		beepTimer = window.setInterval(beep, 650);
	} else if ((!active || !soundEnabled) && beepTimer) {
		window.clearInterval(beepTimer);
		beepTimer = undefined;
	}
}

elements.soundToggle.addEventListener("click", async () => {
	soundEnabled = !soundEnabled;
	if (soundEnabled) {
		const AudioContextClass = window.AudioContext || window.webkitAudioContext;
		if (AudioContextClass) {
			audioContext ||= new AudioContextClass();
			await audioContext.resume();
		} else {
			soundEnabled = false;
		}
	}
	elements.soundToggle.setAttribute("aria-pressed", String(soundEnabled));
	elements.soundToggle.textContent = soundEnabled ? "Disable alert sound" : "Enable alert sound";
	setAlarmSound(Boolean(latestState?.alarm && !latestState?.alarm_muted));
});

window.setInterval(() => {
	const stale = !lastReceivedAt || Date.now() - lastReceivedAt > 15000;
	if (stale && latestState) {
		latestState = undefined;
		elements.deviceState.textContent = "STALE";
		elements.deviceDetail.textContent = "No telemetry received for 15 seconds";
		document.querySelector(".device-sensor").dataset.state = "offline";
		elements.lastUpdate.textContent = "Telemetry timed out";
		elements.statusPanel.dataset.level = "unknown";
		elements.statusText.textContent = "Device offline";
		elements.statusDetail.textContent = "Check the Wokwi simulation and tunnel connection.";
		elements.alertBanner.dataset.level = "warning";
		elements.alertTitle.textContent = "No recent device data";
		elements.alertReason.textContent = "The last sensor update is older than 15 seconds.";
		elements.alarmState.textContent = "NO DATA";
		setAlarmSound(false);
	}
}, 2000);

connect();
