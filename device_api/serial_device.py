import time
import serial
from .base import Device
from .models import Measurement


def fields(response: str) -> dict[str, str]:
    result = {}
    for part in response.split(';'):
        if '=' in part:
            key, value = part.split('=', 1)
            result[key] = value
    return result


class SerialDevice(Device):
    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 2.0):
        self.ser = serial.Serial(port, baudrate=baudrate, timeout=timeout,
                                 write_timeout=timeout)
        # ESP32 USB-serial may reset when the port opens.
        time.sleep(2.0)
        self.ser.reset_input_buffer()

    def command(self, cmd: str) -> str:
        self.ser.reset_input_buffer()
        self.ser.write((cmd + '\n').encode('ascii'))
        self.ser.flush()
        # Ignore boot/log lines, but never treat arbitrary text as a reply.
        for _ in range(40):
            line = self.ser.readline().decode('ascii', errors='replace').strip()
            if not line:
                break
            if (line in ('PONG', 'OK') or line.startswith(('MODEL=', 'SENSOR=',
                   'TEMP_C=', 'PRESSURE_HPA=', 'STATUS='))):
                return line
        raise TimeoutError(f'No protocol response to {cmd!r}; check COM port and firmware')

    def ping(self):
        return self.command('PING') == 'PONG'

    def get_info(self):
        return fields(self.command('GET_INFO'))

    def get_status(self):
        return fields(self.command('GET_STATUS'))

    def _read(self, cmd, key):
        result = fields(self.command(cmd))
        if result.get('STATUS') == 'ERROR':
            return Measurement(None, False, result.get('ERROR', 'UNKNOWN'))
        if result.get('STATUS') != 'OK' or key not in result:
            raise ValueError(f'Invalid {cmd} response: {result}')
        return Measurement(float(result[key]), True)

    def read_temperature(self):
        return self._read('READ_TEMP', 'TEMP_C')

    def read_pressure(self):
        return self._read('READ_PRESSURE', 'PRESSURE_HPA')

    def inject_fault(self, fault):
        if fault != 'TIMEOUT':
            raise ValueError('Only TIMEOUT is supported')
        if self.command('INJECT_FAULT=TIMEOUT') != 'OK':
            raise RuntimeError('Fault injection rejected')

    def clear_fault(self):
        if self.command('CLEAR_FAULT') != 'OK':
            raise RuntimeError('Fault clear rejected')

    def close(self):
        self.ser.close()
