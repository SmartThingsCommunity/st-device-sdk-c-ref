import serial
import sys
import time
import threading

def usage():
    print(f"Usage: {sys.argv[0]} <serial_port>")
    print(f"Example: {sys.argv[0]} /dev/ttyACM0 or COM3")
    sys.exit(1)

# get port from input
if len(sys.argv) != 2:
    print("len: %d", len(sys.argv))
    usage()

# configuration
PORT = sys.argv[1]
BAUDRATE = 2000000
SERIAL_BYTESIZE = serial.EIGHTBITS
SERIAL_PARITY = serial.PARITY_NONE
SERIAL_STOPBITS = serial.STOPBITS_ONE
SERIAL_FLOWCONTROL = False

# init
try:
    ser = serial.Serial(
        port=PORT,
        baudrate=BAUDRATE,
        bytesize=SERIAL_BYTESIZE,
        parity=SERIAL_PARITY,
        stopbits=SERIAL_STOPBITS,
        timeout=1,
        xonxoff=SERIAL_FLOWCONTROL
    )
    print("Serial port opened")
except serial.SerialException as e:
    print(f"Can not open serial port: {e}")
    exit()

def read_serial():
    """read data"""
    buffer = ""
    while True:
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
            buffer += data

            while '\n' in buffer:
                line, buffer = buffer.split('\n', 1)
                print(f"{line.strip()}")

def write_serial(data):
    """write data"""
    ser.write((data + '\n').encode())
    print(f"{data}")

if __name__ == "__main__":
    # launch read thread
    read_thread = threading.Thread(target=read_serial, daemon=True)
    read_thread.start()

    try:
        while True:
            user_input = input()
            if user_input.lower() == 'exit':
                break
            write_serial(user_input)
    except KeyboardInterrupt:
        print("interrupted")
    finally:
        ser.close()
        print("closed")
