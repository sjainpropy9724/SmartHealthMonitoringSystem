import tkinter as tk
import serial
import time
import threading
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from datetime import datetime
from supabase import create_client, Client
from dotenv import load_dotenv
import os

load_dotenv()  # Load environment variables from .env file
SUPABASE_URL = os.getenv("SUPABASE_URL")
SUPABASE_KEY = os.getenv("SUPABASE_KEY")
supabase: Client = create_client(SUPABASE_URL, SUPABASE_KEY)

# ----------------- Serial Setup -----------------
SERIAL_PORT = 'COM5'
BAUD_RATE = 115200

print(f"Connecting to {SERIAL_PORT} at {BAUD_RATE} baud...")
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    ser.reset_input_buffer()
    # Send a wake-up pulse (optional)
    ser.write(b'\r\n')
    time.sleep(5)
except Exception as e:
    print(f"Serial connection failed: {str(e)}")
    exit(1)

# Improved ready check
print("Waiting for ESP32 ready signal", end='', flush=True)
start_time = time.time()
while time.time() - start_time < 30:  # 30 second timeout
    if ser.in_waiting:
        line = ser.readline().decode('utf-8').strip()
        print(f"\nReceived: '{line}'")
        if "READY" in line.upper():
            break
    print(".", end='', flush=True)
    time.sleep(0.5)
else:
    print("\nError: Timeout waiting for ready signal")
    print("Possible solutions:")
    print("1. Verify ESP32 is powered and programmed correctly")
    print("2. Check USB cable connection")
    print("3. Press ESP32 reset button")
    ser.close()
    exit(1)

print("ESP32 synchronization complete")


# ----------------- Global Variables -----------------
vitals = {}
ecg_values = []

# ----------------- Function Definitions -----------------
def read_sensor_data(patient_id, patient_name):
    global ecg_values, vitals
    print("Waiting for sensor data from ESP32...")
    
    recording_ecg = False
    
    while True:
        try:
            line = ser.readline().decode('utf-8').strip()
            if not line:
                continue
            print("Received:", line)
            
            if line.startswith("WEIGHT:"):
                vitals["weight"] = float(line.split("WEIGHT:")[1])
            elif line.startswith("SPO2:"):
                vitals["spo2"] = float(line.split("SPO2:")[1])
            elif line.startswith("PULSE:"):
                vitals["pulse"] = int(line.split("PULSE:")[1])
            elif line.startswith("TEMP:"):
                vitals["temperature"] = float(line.split("TEMP:")[1])
            elif line == "ECG_START":
                print("ECG recording started...")
                recording_ecg = True
                continue
            elif line == "DATA_END":
                break
            elif recording_ecg:
                try:
                    ecg_values.append(int(line))
                except ValueError:
                    pass
        except Exception as e:
            print("Error reading serial:", e)
            break

    print("Sensor data capture complete.")
    print("Vitals:", vitals)
    print("ECG data length:", len(ecg_values))
    ecg_image_filename = generate_ecg_graph(patient_id, patient_name, ecg_values)
    upload_success = upload_to_supabase(patient_id, patient_name, vitals, ecg_image_filename)
    if upload_success:
        # Send confirmation to ESP32
        try:
            ser.write(b"DATA_UPLOADED\n")
            ser.flush()
            print("Sent upload confirmation to ESP32")
            time.sleep(2)
            return
        except Exception as e:
            print("Failed to send confirmation:", e)
    
    ser.close()
    

def generate_ecg_graph(patient_id, patient_name, ecg_data):
    if not ecg_data:
        print("No ECG data captured.")
        return None
    
    # Keep only the last 15 seconds of data (50 samples/sec * 15 sec = 750 samples)
    samples_per_second = 50
    desired_seconds = 15
    desired_samples = desired_seconds * samples_per_second
    ecg_data = ecg_data[-desired_samples:]  # Get last 750 samples or all if less
    
    # Calculate sampling parameters based on trimmed data
    sampling_rate = samples_per_second
    total_seconds = len(ecg_data) / sampling_rate
    
    # Create larger figure (width in inches)
    plt.figure(figsize=(20, 6))  # Width x Height (20" wide for better detail)
    
    # Create time axis in seconds for the trimmed data
    time_axis = [i/sampling_rate for i in range(len(ecg_data))]
    
    # Plot with time axis
    plt.plot(time_axis, ecg_data, label="ECG Signal", linewidth=0.8)
    
    # Formatting (rest remains the same)
    plt.xlabel("Time (seconds)", fontsize=12)
    plt.ylabel("ECG Value (ADC units)", fontsize=12)
    plt.title(f"ECG Data for {patient_name} ({patient_id}) - Last {total_seconds:.1f}s", fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.7)
    
    max_time = int(total_seconds) + 1
    plt.xticks(range(0, max_time, 1))
    for sec in range(0, max_time):
        plt.axvline(x=sec, color='gray', linestyle=':', alpha=0.5)
    
    plt.legend(fontsize=12)
    plt.tight_layout()
    
    filename = f"ecg_graph_{patient_id}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.png"
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()
    return filename

def upload_to_supabase(patient_id, patient_name, vitals, ecg_image_filename):
    try:
        with open(ecg_image_filename, "rb") as img_file:
            image_data = img_file.read()
            
        try:
            storage_response = supabase.storage.from_("ecg-images").upload(path=ecg_image_filename, file=image_data, file_options={"content-type": "image/png"})
            ecg_image_url = f"{SUPABASE_URL}/storage/v1/object/public/ecg-images/{ecg_image_filename}"
            print("ECG image uploaded. URL:", ecg_image_url)
        except Exception as e:
            print("Exception during image upload:", e)
            return False

        data = {
            "patient_id": patient_id,
            "patient_name": patient_name,
            "weight": vitals.get("weight", None),
            "spo2": vitals.get("spo2", None),
            "pulse": vitals.get("pulse", None),
            "temperature": vitals.get("temperature", None),
            "ecg_image": ecg_image_url,
            "timestamp": datetime.now().isoformat()
        }
        try:
            response = supabase.table("patient_data").insert(data).execute()
            if not response.data:
                print("Database Insert Warning: Empty response")
                return False
            else:
                print("Data uploaded successfully!")
                return True
        except Exception as db_error:
            print("Database Insert Error:", str(db_error))
            return False
    except Exception as e:
        print("General Exception:", str(e))
        return False

def send_patient_info():
    patient_id = entry_id.get().strip()
    patient_name = entry_name.get().strip()
    if patient_id and patient_name:
        time.sleep(1)  # Small buffer
        
        ser.write((patient_id + "\n").encode())
        time.sleep(0.5)  # Important delay
        ser.write((patient_name + "\n").encode())
        
        print(f"Sent patient info: {patient_id} | {patient_name}")
        
        # Wait for confirmation with timeout
        start_time = time.time()
        while time.time() - start_time < 5:
            line = ser.readline().decode('utf-8').strip()
            if line == "PATIENT_RECEIVED":
                break
        else:
            print("Warning: Didn't receive confirmation from ESP32")
        
        root.destroy()
        threading.Thread(target=read_sensor_data, args=(patient_id, patient_name)).start()

# ----------------- GUI Setup -----------------
root = tk.Tk()
root.title("Enter Patient Information")

tk.Label(root, text="Patient ID:").grid(row=0, column=0, padx=10, pady=5)
entry_id = tk.Entry(root)
entry_id.grid(row=0, column=1, padx=10, pady=5)

tk.Label(root, text="Patient Name:").grid(row=1, column=0, padx=10, pady=5)
entry_name = tk.Entry(root)
entry_name.grid(row=1, column=1, padx=10, pady=5)

tk.Button(root, text="Submit", command=send_patient_info).grid(row=2, column=0, columnspan=2, pady=10)
root.mainloop()
