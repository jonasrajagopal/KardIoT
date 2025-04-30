from serial import Serial
import sys
ser = Serial(sys.argv[1], 9600)

while True:
    data = ser.readline()
    if data:
        try:
            print(data.decode().strip(), flush=True)
        except:
            pass