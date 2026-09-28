from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Literal

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel, Field

from .database import EventStore


class SensorEvent(BaseModel):
	status: Literal["SAFE", "WARNING", "HIGH", "CRITICAL", "FIRE ALERT", "MANUAL"]
	reason: str = Field(max_length=120)
	gas: int = Field(ge=0, le=4095)
	temperature: float = Field(ge=-40, le=125)
	humidity: float = Field(ge=0, le=100)
	pir: bool
	fire: bool
	risk_score: int = Field(ge=0, le=100)
	alarm: bool
	alarm_muted: bool
	manual_emergency: bool = False
	mute_remaining_seconds: int = Field(default=0, ge=0, le=5)


app = FastAPI(title="SafeHostel Local Monitor", version="1.0.0")
frontend_dir = Path(__file__).resolve().parent.parent / "frontend"
store = EventStore()
clients: set[WebSocket] = set()


def utc_now() -> str:
	return datetime.now(timezone.utc).isoformat(timespec="seconds")


async def publish_snapshot() -> None:
	message = {
		"type": "snapshot",
		**store.snapshot(),
	}
	disconnected: list[WebSocket] = []
	for client in clients:
		try:
			await client.send_json(message)
		except (RuntimeError, WebSocketDisconnect):
			disconnected.append(client)
	for client in disconnected:
		clients.discard(client)


@app.get("/api/health")
async def health() -> dict[str, Any]:
	return {
		"ok": True,
		"device_connected": store.current is not None
		and (datetime.now(timezone.utc) - datetime.fromisoformat(store.current["received_at"])).total_seconds() < 15,
		"received_at": store.current["received_at"] if store.current else None,
	}


@app.get("/api/state")
async def get_state() -> dict[str, Any]:
	return store.snapshot()


@app.post("/api/events")
async def receive_event(event: SensorEvent) -> dict[str, Any]:
	state = event.model_dump()
	state["received_at"] = utc_now()
	store.update(state)

	await publish_snapshot()
	return {"ok": True, "received_at": state["received_at"]}


@app.websocket("/ws")
async def websocket_updates(websocket: WebSocket) -> None:
	await websocket.accept()
	clients.add(websocket)
	try:
		await websocket.send_json(
			{"type": "snapshot", **store.snapshot()}
		)
		while True:
			await websocket.receive_text()
	except WebSocketDisconnect:
		clients.discard(websocket)


@app.get("/", response_class=HTMLResponse)
async def dashboard() -> HTMLResponse:
	return HTMLResponse((frontend_dir / "index.html").read_text(encoding="utf-8"))


app.mount("/", StaticFiles(directory=frontend_dir), name="frontend")
