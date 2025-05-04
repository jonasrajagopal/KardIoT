import numpy as np
import sys
from scipy.fft import fft, fftfreq

T = []
Y = []

# Smooth using moving average
def moving_average(x, w):
    return np.convolve(x, np.ones(w), mode='same') / w

try:
    while True:
        line = sys.stdin.readline().strip()
        if line:
            # Process the input line
            print(f"{line}")
            if (len(line.split(",")) >2): continue
            t,y = line.split(",")
            t = int(t)/1000
            y = int(y)
            T.append(t)
            Y.append(y)
            if (len(Y)<100): continue
            T = T[-300:]
            Y = Y[-300:]
            amplitude = np.array(Y)
            time = np.array(T)
            dt = np.mean(np.diff(time))  # or use np.mean(np.diff(time)) if it's not perfectly uniform


            amplitude = amplitude - np.mean(amplitude)

            # Compute FFT
            N = len(amplitude)
            yf = fft(amplitude)
            xf = fftfreq(N, dt)[:N//2]  # Take the positive half of the frequencies
            amplitude_spectrum = 2.0/N * np.abs(yf[:N//2])

            # Define frequency range
            f_min = 0.01  # Hz, adjust based on your data
            f_max = 10    # Hz, or something like 1 / min(np.diff(time)) if unsure

            # Convert to angular frequencies (radians/second)
            frequencies = np.linspace(f_min, f_max, 5000)
            angular_freqs = 2 * np.pi * frequencies

            smoothed_amplitude = moving_average(amplitude_spectrum, w=5)  # Adjust window size as needed

            start = 1
            end = int(len(xf) / 3)
            # print(max(smoothed_amplitude[start:end]))
            # print(np.argmax(smoothed_amplitude[start:end]))
            print("Heart rate: "+ str(xf[start:end][np.argmax(smoothed_amplitude[start:end])]*60) +"bpm")

        else:
            # Handle the case where no input is available 
            # (e.g., sleep to avoid busy-waiting)
            pass
except KeyboardInterrupt:
    print("Stopped reading from stdin.")