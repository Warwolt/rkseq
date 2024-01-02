import sys
import serial
import serial.tools.list_ports
import signal
from datetime import datetime
import time

COLOR_GREEN = "\033[32m"
COLOR_RED = "\033[31m"
COLOR_YELLOW = "\033[33m"
COLOR_RESET = "\033[0m"

log_level_str = {
    "I": "{}INFO{}".format(COLOR_GREEN, COLOR_RESET),
    "W": "{}WARNING{}".format(COLOR_YELLOW, COLOR_RESET),
    "E": "{}ERROR{}".format(COLOR_RED, COLOR_RESET),
}


def signal_handler(sig, frame):
    print("[ Monitor ] Serial port closed.")
    sys.exit(0)


def timestamp_str():
    now = datetime.now()
    return "%02d:%02d:%02d:%03d" % (
        now.hour, now.minute, now.second, (now.microsecond / 1000) % 1000)


if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)

    if len(sys.argv) < 2:
        print("Usage: monitor.py PORT", end="")
        print("")
        print("PORT should be the serial port name on your OS, e.g. COM0 on Windows or ttyS0 on Linux")
        print("")
        print("Detected serial ports:")
        for name in [comport.device for comport in serial.tools.list_ports.comports()]:
            print(" - %s" % name)
        exit(1)

    baud = 9600
    port = sys.argv[1]
    ser = serial.Serial(port, baud, timeout=0.5)

    print("[ Monitor ] Serial port opened (close with Ctrl+C).")

    while True:
        serial_input = ser.readline().decode('utf-8').strip('\n')
        if serial_input:
            [level, file, text] = serial_input.split(" ", 2)
            print("[%s %s %s] %s" %
                  (timestamp_str(), log_level_str[level], file, text))
