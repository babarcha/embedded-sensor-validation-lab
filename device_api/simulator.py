from .base import Device
from .models import Measurement


class SimulatorDevice(Device):
    """Contract simulator: does not execute ESP32 firmware."""

    def __init__(self):
        self.fault = None
        self.ready = True

    def ping(self):
        return True

    def get_info(self):
        return {"MODEL": "ESP32-SENSOR", "FW": "1.0.0"}

    def get_status(self):
        return {"SENSOR": "READY" if self.ready else "NOT_READY",
                "FAULT": self.fault or "NONE"}

    def _read(self, value):
        if self.fault == "TIMEOUT":
            return Measurement(None, False, "TIMEOUT")
        if not self.ready:
            return Measurement(None, False, "NOT_READY")
        return Measurement(value, True)

    def read_temperature(self):
        return self._read(23.45)

    def read_pressure(self):
        return self._read(1013.25)

    def inject_fault(self, fault):
        if fault != "TIMEOUT":
            raise ValueError("Only TIMEOUT is supported")
        self.fault = fault

    def clear_fault(self):
        self.fault = None
