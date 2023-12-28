import sys
import serial
import serial.tools.list_ports
import signal
from datetime import datetime

def timestamp_now():
    '''
    Returns the current time as milliseconds since 00:00
    '''
    midnight = datetime.now().replace(
        hour=0, minute=0, second=0, microsecond=0)
    delta = datetime.now() - midnight
    return delta.seconds * 1000 + delta.microseconds // 1000


def signal_handler(sig, frame):
    print("[ Monitor ] Serial port closed.")
    sys.exit(0)


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
        ser_in = ser.readline().decode('utf-8').strip('\n')
        if ser_in:
            now = datetime.now()
            print("[%02d:%02d:%02d:%03d]" % (now.hour, now.minute, now.second, (now.microsecond / 1000) % 1000), end=' ')
            print(ser_in)
