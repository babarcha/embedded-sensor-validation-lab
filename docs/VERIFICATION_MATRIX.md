
# Sensor Validation — Verification Matrix

## Purpose

Map the GE HealthCare-inspired sensor lab requirements
to implementation components and verification tests.

This is an educational embedded-software project,
not a certified medical device.

| ID | Requirement | Implementation | Verification |
|---|---|---|---|
| REQ-01 | Device responds to connectivity requests | app_main.c | test_connectivity |
| REQ-02 | Device reports model and firmware identity | app_main.c | test_identity |
| REQ-03 | Device reports sensor readiness | sensor_service.c, app_main.c | test_sensor_status |
| REQ-04 | Device acquires temperature | bmp280.c, sensor_service.c | test_temperature |
| REQ-05 | Device acquires pressure | bmp280.c, sensor_service.c | test_pressure |
| REQ-06 | Timeout errors must not expose stale measurements | sensor_service.c | test_timeout_reports_no_stale_sample; native tests |
| REQ-07 | Invalid sensor measurements must be rejected | sensor_service.c | Native range-validation tests |
| REQ-08 | A subsequent successful read must recover after timeout | sensor_service.c | test_recovery_after_timeout; native tests |
| REQ-09 | Partial driver failures must not publish partial samples | sensor_service.c | Native partial-write fault test |
| REQ-10 | Native and simulator regression tests run automatically | GitHub Actions workflow | Sensor Validation CI |

## Verification levels

### Native unit testing
- Executes on the host without ESP32 hardware.
- Uses a mocked BMP280 driver.
- Tests error handling and measurement validation.

### Simulator validation
- Uses the Python device abstraction.
- Tests the command-level interface without hardware.

### Hardware-in-the-loop validation
- Executes against a physical ESP32 and BMP280.
- Verifies communication and actual sensor acquisition.
- Exercises command-level fault injection.

### Continuous integration
- Runs native C tests and simulator validation.
- Uses GitHub Actions on Ubuntu.
- Hardware tests remain manually executed.

## Verification evidence

- Native C tests: Passed in Pass 5.
- GitHub Actions: Workflow #1 passed.
- ESP-IDF firmware build: Passed in Pass 6.
- ESP32 firmware flash: Passed in Pass 6.
- Hardware validation: 7/7 passed in 14.31 seconds.

## Limitations

- No automated hardware-in-the-loop CI runner.
- No medical-device certification or formal safety assessment.
- No long-duration reliability or environmental testing.
- Fault injection is deterministic and does not reproduce
  every possible physical I2C failure.
