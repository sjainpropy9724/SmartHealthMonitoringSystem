import serial
import matplotlib.pyplot as plt
import time
from collections import deque

# Set up serial communication
ser = serial.Serial('COM5', 115200, timeout=1)
time.sleep(2)  # Give some time for the connection to establish

# Store only the last 15 seconds of ECG data
time_window = 15  # seconds
sampling_rate = 1  # 1 sample per second
max_points = time_window * sampling_rate

# Deque to store the last 15 seconds of ECG data
ecg_values = deque(maxlen=max_points)
timestamps = deque(maxlen=max_points)

start_time = time.time()

plt.ion()  # Enable interactive mode
fig, ax = plt.subplots()

while True:
    try:
        # Read data from serial port
        line = ser.readline().decode('utf-8').strip()
        if line.isdigit():  # Ensure we received a valid ECG value
            ecg_value = int(line)
            
            # Append data with a rolling window of 15 seconds
            current_time = time.time() - start_time
            ecg_values.append(ecg_value)
            timestamps.append(current_time)

            # Ensure x-axis remains within the last 15 seconds
            if len(timestamps) > 1:
                min_x = max(0, timestamps[-1] - time_window)
                ax.set_xlim(min_x, timestamps[-1])

            # Plot the ECG data
            ax.clear()
            ax.plot(timestamps, ecg_values, label="ECG Signal")
            ax.set_xlabel("Time (s)")
            ax.set_ylabel("ECG Value")
            ax.set_title("ECG Signal")
            ax.legend()
            ax.grid(True)

            plt.pause(1)  # Update every second

    except KeyboardInterrupt:
        print("Stopping script...")
        break

# Close serial connection
ser.close()
plt.ioff()
plt.show()
