
import argparse
import time
import serial


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", default="COM3")
    parser.add_argument("--baud", type=int, default=115200)
    args = parser.parse_args()

    with serial.Serial(
        args.port,
        args.baud,
        timeout=1
    ) as ser:

        # ESP32 may reset when the serial port opens.
        time.sleep(3)

        # Discard startup messages before commands.
        ser.reset_input_buffer()

        print("ESP32 ready. Enter command, or quit.")

        while True:
            command = input("> ").strip()

            if command.lower() in ("quit", "exit"):
                break

            if not command:
                continue

            ser.write((command + "\n").encode())
            ser.flush()

            deadline = time.monotonic() + 2.0
            response = ""

            while time.monotonic() < deadline:
                line = ser.readline().decode(
                    errors="replace"
                ).strip()

                if not line:
                    continue

                # Ignore known ESP32 startup messages.
                if line.startswith((
                    "ets ",
                    "rst:",
                    "configsip:",
                    "clk_drv:",
                    "load:",
                    "entry "
                )):
                    continue

                response = line
                break

            print(response if response else "TIMEOUT")


if __name__ == "__main__":
    main()
