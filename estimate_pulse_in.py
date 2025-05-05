import numpy as np
import sys
from scipy.fft import fft, fftfreq
from scipy.signal import find_peaks, lombscargle, butter, filtfilt

T_arr = []
Y_arr = []

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
            T_arr.append(t)
            Y_arr.append(y)
            if (len(Y_arr)<100): continue
            T_arr = T_arr[-300:]
            Y_arr = Y_arr[-300:]
            T = np.array(T_arr)
            Y = np.array(Y_arr)

            # T = np.tile(T,2)
            # Y = np.tile(Y,2)
            
            # amplitude = np.tile(Y,1)
            time = T#np.tile(T,1)
            dt = np.mean(np.diff(time))  # or use np.mean(np.diff(time)) if it's not perfectly uniform


            # amplitude = amplitude - np.mean(amplitude)



            sample_rate = 1/np.mean(np.diff(T))
            nyq = sample_rate/2
            order=4
            low_cutoff= 5
            high_cutoff = 0.5
            low_normal_cutoff = low_cutoff/nyq
            high_normal_cutoff = high_cutoff/nyq
            high_b, high_a = butter(order, high_normal_cutoff, btype='highpass')
            low_b, low_a = butter(order, low_normal_cutoff, btype='lowpass')

            Y3 = filtfilt(high_b, high_a, filtfilt(low_b, low_a, Y))
            # Y3 = -1*Y

            amplitude = Y3

            # print(amplitude, len(amplitude))

            # Compute FFT
            N = len(amplitude)*10
            yf = fft(amplitude,N)
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
            peak_idx = np.argmax(smoothed_amplitude)
            # print(f"Simple Max: {xf[peak_idx] * 60}")

            print("Heart rate: "+ str(xf[peak_idx] * 60) +"bpm")

        else:
            # Handle the case where no input is available 
            # (e.g., sleep to avoid busy-waiting)
            pass
except KeyboardInterrupt:
    print("Stopped reading from stdin.")