
# ESP32 Sensor Validation Lab

A compact embedded software development and validation project
inspired by sensor software engineering workflows in healthcare
technology.

Built with ESP-IDF, C, Python, pytest, and GitHub Actions.

## Project objectives

- Develop an ESP32 firmware interface for a BMP280 sensor.
- Separate hardware access from application-level validation.
- Verify sensor behavior using native unit tests.
- Support simulator and physical hardware testing.
- Exercise timeout handling, invalid measurements, and recovery.
- Automate regression testing through GitHub Actions.

This is an educational engineering project and is not a
medical device or certified medical software.

## Architecture

    Python pytest validation
              |
              v
       Device API abstraction
          /          \
         v            v
     Simulator    Serial adapter
                       |
                       v
                  ESP32 UART
                       |
                       v
                 Sensor service
                       |
                       v
                  BMP280 driver
                       |
                       v
                  I2C hardware

Native C unit tests compile the real sensor-service
implementation against a mocked BMP280 driver.

## Hardware

- ESP32 development board
- BMP280 pressure and temperature sensor
- USB cable
- Breadboard and jumper wires

### I2C connections

| BMP280 | ESP32 |
|--------|-------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

Use the appropriate I2C mode configuration for the
specific sensor breakout board.

## Firmware

Firmware source is located in `firmware/`.

Main components:

- `app_main.c`: UART command interface
- `sensor_service.c`: Sensor acquisition and validation
- `bmp280.c`: BMP280 hardware driver

### Build and flash

From an ESP-IDF-enabled terminal:

    cd firmware
    idf.py build
    idf.py -p COM3 flash

Replace COM3 with the appropriate serial port.

## UART commands

    PING
    GET_INFO
    GET_STATUS
    READ_TEMP
    READ_PRESSURE
    INJECT_FAULT=TIMEOUT
    CLEAR_FAULT

Example responses:

    PONG
    MODEL=ESP32-SENSOR;FW=1.0.0
    SENSOR=READY;FAULT=NONE
    TEMP_C=21.24;STATUS=OK
    PRESSURE_HPA=997.67;STATUS=OK

The measurements above are examples, not fixed values.

## Native C unit testing

On Windows:

    .\tests\native\run_native.ps1

On Linux:

    bash tests/native/run_native.sh

Native tests use a mocked BMP280 driver to verify:

- Initialization and argument validation
- Successful sensor acquisition
- Injected timeout behavior
- Driver-level failures
- Invalid temperature and pressure values
- Partial sensor read failures
- Recovery after transient errors
- Prevention of stale measurement publication

## Python simulator validation

Install dependencies:

    python -m pip install -r requirements.txt

Run:

    python -m pytest tests/validation --device simulator -v

## ESP32 hardware validation

Connect the ESP32 and BMP280.

Build and flash the firmware, then run:

    python -m pytest tests/validation --device serial --port COM3 -v

Ensure no other application is using the serial port.

## Continuous integration

GitHub Actions automatically runs:

1. Native C unit tests on Ubuntu.
2. Python simulator validation using pytest.

The physical ESP32 tests are performed locally.

## Verification

Final hardware regression:

- ESP-IDF build: PASS
- ESP32 flash: PASS
- Hardware pytest: 7/7 PASS
- Execution time: 14.31 seconds

Native unit tests and GitHub Actions CI have also passed.

See `docs/VERIFICATION_MATRIX.md` for the
requirements-to-test mapping.

## Engineering practices demonstrated

- Embedded C development
- Hardware abstraction and mocking
- Deterministic fault injection
- Defensive programming
- Unit testing and HIL validation
- Automated regression testing
- Requirements traceability
- Git version control and CI

## Limitations

- No formal medical-device certification
- No automated physical hardware CI runner
- No long-duration environmental or reliability testing
- No complete hardware fault coverage
- No claim of production medical-device readiness

## Technology stack

- ESP32
- ESP-IDF 5.5.5
- C11
- Python 3.11
- pytest
- GCC
- GitHub Actions
