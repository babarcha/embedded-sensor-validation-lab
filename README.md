# GE HealthCare-style ESP32 Sensor Validation Lab — Pass 2

**Independent educational interview lab. Not affiliated with GE HealthCare. Not a medical device.**

## Hardware and Pass 1 firmware
ESP32 DevKit + BMP280 (chip ID 0x58). I²C: GPIO21 SDA, GPIO22 SCL, 3.3V/GND. The sensor measures environmental temperature and pressure, not physiological signals.

From an ESP-IDF 5.5.5 terminal:
```powershell
cd firmware
idf.py set-target esp32
idf.py build
idf.py -p COM3 flash monitor
```
Exit the monitor with Ctrl+] before opening COM3 from Python.

## Pass 2 — shared validation tests
From the **repository root**, in a regular PowerShell terminal:
```powershell
python -m pip install -r requirements.txt
python -m pytest tests/validation --device simulator -v
python -m pytest tests/validation --device serial --port COM3 -v
```
Both commands execute the same seven test functions. `SimulatorDevice` models the device's external contract; `SerialDevice` talks to real ESP32 firmware. The simulator does not execute firmware code.

## Native C unit tests (actual sensor_service.c)
From repository root:
```powershell
.\tests\native\run_native.ps1
```
Requires GCC, preferably `C:\msys64\ucrt64\bin\gcc.exe`. A stub `esp_err.h` and fake BMP280 driver allow compiling the actual service C file without ESP-IDF or a board.

## Design
- L1: `tests/validation/` — shared behavioral tests
- L2: `device_api/base.py` and `models.py` — stable contract
- L3: `device_api/simulator.py` and `serial_device.py` — backends
- Separate native C tests: `tests/native/` — real service logic with mocked driver

## Fault model and limitations
`INJECT_FAULT=TIMEOUT` simulates a timeout in firmware service logic. It does not physically induce an I²C timeout. `CLEAR_FAULT` restores normal reads. A test fixture clears faults before and after each test. Range checks use BMP280 datasheet-like operating bounds for educational purposes, not clinical acceptance limits.

## Expected results
- Simulator: 7 passed
- Serial: 7 passed if Pass 1 firmware is flashed, COM3 is free, and BMP280 is detected
- Native: PASS: native sensor_service tests

## Interview explanation
The Python suite verifies the external device contract on two backends; native C tests verify real firmware logic; hardware tests verify physical integration. These provide complementary evidence rather than interchangeable proof.
