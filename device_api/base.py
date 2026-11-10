from abc import ABC, abstractmethod
from .models import Measurement


class Device(ABC):
    @abstractmethod
    def ping(self) -> bool: ...

    @abstractmethod
    def get_info(self) -> dict[str, str]: ...

    @abstractmethod
    def get_status(self) -> dict[str, str]: ...

    @abstractmethod
    def read_temperature(self) -> Measurement: ...

    @abstractmethod
    def read_pressure(self) -> Measurement: ...

    @abstractmethod
    def inject_fault(self, fault: str) -> None: ...

    @abstractmethod
    def clear_fault(self) -> None: ...

    def close(self) -> None:
        pass
