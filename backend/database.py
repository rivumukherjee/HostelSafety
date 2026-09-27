from collections import deque
from typing import Any


class EventStore:
	"""Transient dashboard state; contents intentionally reset on server restart."""

	def __init__(self) -> None:
		self.current: dict[str, Any] | None = None
		self.history: deque[dict[str, Any]] = deque(maxlen=50)
		self._last_signature: tuple[Any, ...] | None = None

	def update(self, state: dict[str, Any]) -> None:
		self.current = state
		signature = (
			state["status"],
			state["reason"],
			state["alarm"],
			state["alarm_muted"],
			state["manual_emergency"],
		)
		if signature == self._last_signature:
			return

		if state["manual_emergency"]:
			message = "Manual emergency activated"
		elif state["alarm_muted"]:
			message = "Alarm muted for 5 seconds"
		elif state["alarm"]:
			message = f"{state['status']}: {state['reason']}"
		elif state["status"] == "SAFE":
			message = "System returned to safe"
		else:
			message = f"{state['status']}: {state['reason']}"

		self.history.appendleft(
			{
				"time": state["received_at"],
				"message": message,
				"status": state["status"],
				"alarm": state["alarm"],
				"manual_emergency": state["manual_emergency"],
				"alarm_muted": state["alarm_muted"],
			}
		)
		self._last_signature = signature

	def snapshot(self) -> dict[str, Any]:
		return {"current": self.current, "history": list(self.history)}
