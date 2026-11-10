import pytest
from device_api.simulator import SimulatorDevice


def pytest_addoption(parser):
    parser.addoption('--device', choices=['simulator', 'serial'],
                     default='simulator', help='Validation backend')
    parser.addoption('--port', default='COM3', help='Serial port for ESP32')


@pytest.fixture
def device(request):
    if request.config.getoption('--device') == 'serial':
        from device_api.serial_device import SerialDevice
        dut = SerialDevice(request.config.getoption('--port'))
    else:
        dut = SimulatorDevice()
    try:
        # Ensure a previous failed test did not leave fault injection enabled.
        dut.clear_fault()
        yield dut
    finally:
        try:
            dut.clear_fault()
        finally:
            dut.close()
