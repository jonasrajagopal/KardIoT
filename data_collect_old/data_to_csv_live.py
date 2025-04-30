from serial import Serial
import matplotlib.pyplot as plt
import matplotlib
import pandas as pd
import sys
import os
from drawnow import *
import time

ser = Serial('/dev/ttyUSB0', 9600)



# plt.show()
plt.ion()
xs = []
ys = []
def make_plot():
    # plt.ion()
    # plt.title("Intensity vs Time")
    # plt.grid(True)
    plt.plot(xs, ys)
    # plt.xlabel('t (s)')
    # plt.ylabel('intensity')

i = 0
buf = ""
while True:
    # data = ser.readline()
    try:
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting)
            dec = data.decode()
            buf += dec 
            oldbuf = buf.split("\r\n")[:-1]
            buf = buf.split("\r\n")[-1]
            # print(oldbuf, buf, buf.split("\r\n"))
            for d in oldbuf:
                print(d)
                # continue
                # d = data.decode().strip()
                # print(d)
                t,y = d.split(",")
                t = int(t)
                y = int(y)
                xs.append(t)
                ys.append(y)
                if (i==100):
                    xs = xs[-200:]
                    ys = ys[-200:]
                    i=0
                i+=1
        drawnow(make_plot,stop_on_close=True)
        # time.sleep(0.01)

    except Exception as e:
                print("here",e)
                pass