import serial
import time

# Change COMx to your correct port (Check in Arduino IDE: Tools > Port)
ser = serial.Serial("COM5", 9600, timeout=1)  # Ensure baud rate matches Arduino code

filename = "ecg_data.csv"
with open(filename, "w") as file:
    file.write("Time,ECG_Value\n")  # CSV Header
    print("Recording data... Press CTRL+C to stop.")

    try:
        while True:
            data = ser.readline().decode("latin-1").strip()  # Use "latin-1" or ignore errors
            if data:
                timestamp = time.time()
                file.write(f"{timestamp},{data}\n")
                print(f"{timestamp},{data}")  # Display in console
    except KeyboardInterrupt:
        print("\nRecording stopped.")
