import serial
import time
from pygame import mixer
import os

# Initialize the mixer
mixer.init()
mixer.set_num_channels(32)  # Adjust the number based on your needs

# Define the base directory for samples
base_dir = r'C:\Users\hp\Documents\Arduino\Invisible-Drum-Set-main\SAMPLES'

# Load the sound files into Sound objects
try:
    snare = mixer.Sound(os.path.join(base_dir, r'SNARES\07_Snare_16_SP.wav'))
    hihat = mixer.Sound(os.path.join(base_dir, r'HIHATS\Boom-Bap Hat CL 53.wav'))
    high_tom = mixer.Sound(os.path.join(base_dir, r'TOMS\TOM.wav'))
    crash = mixer.Sound(os.path.join(base_dir, r'CYMBALS\07_Perc_05_SP.wav'))
    kick = mixer.Sound(os.path.join(base_dir, r'KICKS\Boom-Bap Kick 53.wav'))
    mid_tom = mixer.Sound(os.path.join(base_dir, r'TOMS\Acoustic Mid Tom 06.wav'))
    ride = mixer.Sound(os.path.join(base_dir, r'CYMBALS\MachineRide.wav'))
    flor_tom = mixer.Sound(os.path.join(base_dir, r'TOMS\Tomm1.wav'))
except FileNotFoundError as e:
    print(f"Error: {e}")
    exit(1)

# Set the initial volume for each sound object
snare.set_volume(1)   # Adjust as needed
hihat.set_volume(1)   # Adjust as needed
high_tom.set_volume(1)   # Adjust as needed
crash.set_volume(1)   # Adjust as needed
kick.set_volume(1)   # Adjust as needed
mid_tom.set_volume(1)   # Adjust as needed
ride.set_volume(1)   # Adjust as needed
flor_tom.set_volume(1)   # Adjust as needed

# Open serial port
ser = serial.Serial('COM6', 115200, timeout=1)
time.sleep(2)  # Wait for the serial connection to initialize

# To keep track of currently playing sound
current_command = None

def play_sound(sound):
    sound.play()

while True:
    if ser.in_waiting > 0:
        try:
            received_data = ser.readline().decode('utf-8').strip()
            # Filter out only valid commands (0 to 7)
            if received_data.isdigit() and received_data in "01234567":
                if received_data != current_command:  # Prevent playing the same sound repeatedly
                    print(f"Received data: {received_data}")
                    if received_data == "0":
                        play_sound(snare)
                    elif received_data == "1":
                        play_sound(hihat)
                    # Skip the sound for "2"
                    elif received_data == "3":
                        play_sound(crash)
                    elif received_data == "4":
                        play_sound(kick)
                    elif received_data == "5":
                        play_sound(mid_tom)
                    elif received_data == "6":
                        play_sound(ride)
                    elif received_data == "7":
                        play_sound(flor_tom)
                    current_command = received_data
        except UnicodeDecodeError:
            print("Received corrupted data. Skipping...")
    time.sleep(0.05)  # Slightly increased delay to manage serial data rate
