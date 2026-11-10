def test_connectivity(device):
    assert device.ping()


def test_identity(device):
    info = device.get_info()
    assert info['MODEL'] == 'ESP32-SENSOR'
    assert info['FW'] == '1.0.0'


def test_sensor_status(device):
    assert device.get_status()['SENSOR'] == 'READY'


def test_temperature(device):
    result = device.read_temperature()
    assert result.valid, result.error
    assert result.value is not None
    assert -40.0 <= result.value <= 85.0


def test_pressure(device):
    result = device.read_pressure()
    assert result.valid, result.error
    assert result.value is not None
    assert 300.0 <= result.value <= 1100.0


def test_timeout_reports_no_stale_sample(device):
    device.inject_fault('TIMEOUT')
    try:
        result = device.read_temperature()
        assert result.valid is False
        assert result.value is None
        assert result.error == 'TIMEOUT'
    finally:
        device.clear_fault()


def test_recovery_after_timeout(device):
    device.inject_fault('TIMEOUT')
    try:
        assert device.read_temperature().error == 'TIMEOUT'
    finally:
        device.clear_fault()
    assert device.get_status()['FAULT'] == 'NONE'
    result = device.read_temperature()
    assert result.valid and result.value is not None
