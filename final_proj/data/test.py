from serial import Serial
ser = Serial('/dev/ttyUSB0', 9600)

while True:
    data = ser.readline()
    if data:
        try:
            print(data.decode().strip())
        except:
            pass