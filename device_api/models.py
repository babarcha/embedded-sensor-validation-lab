from dataclasses import dataclass


@dataclass(frozen=True)
class Measurement:
    value: float | None
    valid: bool
    error: str | None = None
