import os
import time
import speech_recognition as sr
import requests
import google.generativeai as genai

# Configuration Parameters
GEMINI_KEY = "Insert your Gemini API key here"  # Replace with your actual Gemini API key
ESP32_IP = "http://192.168.137.54"  # Your active ESP32 IP address
BURST_DURATION = 0.8                # Time in seconds the car moves per command

genai.configure(api_key=GEMINI_KEY)

model = genai.GenerativeModel(
    model_name="models/gemini-2.5-flash",
    generation_config={"temperature": 0.0},
    system_instruction=(
        "You are a strict translation gateway for a robotic car. "
        "Analyze the spoken user input and output EXACTLY one character based on intent:\n"
        "F = Move forward, advance, go straight\n"
        "B = Move backward, reverse, back up\n"
        "L = Turn left, spin counter-clockwise\n"
        "R = Turn right, spin clockwise\n"
        "S = Stop, halt, brake, kill power\n"
        "If the intent is unclear, output S. Do not provide explanations or extra characters."
    )
)

# Local command routing matrix to preserve API tokens
LOCAL_COMMAND_MAP = {
    "forward": "F", "go forward": "F", "move forward": "F", "advance": "F",
    "backward": "B", "go backward": "B", "move backward": "B", "reverse": "B", "back up": "B",
    "left": "L", "turn left": "L", "spin left": "L",
    "right": "R", "turn right": "R", "spin right": "R",
    "stop": "S", "halt": "S", "brake": "S", "kill": "S"
}

recognizer = sr.Recognizer()

def send_stop():
    """Directly commands the ESP32 to halt all motor channels."""
    try:
        requests.get(f"{ESP32_IP}/S", timeout=0.3)
    except Exception as e:
        print(f"Stop command delivery failure: {e}")

def listen_and_command():
    with sr.Microphone() as source:
        print("Voice Control Architecture Online via Wi-Fi. Listening...")
        recognizer.adjust_for_ambient_noise(source, duration=1)
        try:
            audio = recognizer.listen(source, timeout=4, phrase_time_limit=4)
            print("Processing audio profile...")
            
            raw_text = recognizer.recognize_google(audio)
            text = str(raw_text).lower().strip()
            print(f"Recognized Speech: '{text}'")

            command = None

            # Optimization Layer: Check local dictionary
            if text in LOCAL_COMMAND_MAP:
                command = LOCAL_COMMAND_MAP[text]
                print(f"Local Matrix Match (API Bypassed): {command}")
            else:
                print("No local match found. Routing query to Gemini Engine...")
                response = model.generate_content(text)
                command = response.text.strip().upper()

            # Pulse Architecture Execution Gate
            if command in ['F', 'B', 'L', 'R']:
                print(f"Target Execution Vector: {command}")
                
                # Step 1: Initiate Movement
                requests.get(f"{ESP32_IP}/{command}", timeout=0.3)
                print(f"Motors Energized. Driving for {BURST_DURATION} seconds...")
                
                # Step 2: Hold state for the burst window
                time.sleep(BURST_DURATION)
                
                # Step 3: Enforce automatic local stop
                send_stop()
                print("Motors Halted Automatically.")
                
            elif command == 'S':
                print("Manual Stop Vector Received.")
                send_stop()
            else:
                send_stop()

        except sr.WaitTimeoutError:
            print("Listening timed out. No speech detected.")
            send_stop()
        except sr.UnknownValueError:
            print("Audio profile ambiguous. Could not parse speech.")
            send_stop()
        except Exception as e:
            print(f"Runtime processing error: {e}")
            send_stop()
            if "429" in str(e):
                print("Quota exhausted. Initiating 10-second cool-down cycle...")
                time.sleep(10)

if __name__ == "__main__":
    # Force safe initial state
    send_stop()
    while True:
        listen_and_command()
        print("-" * 40)
        time.sleep(0.1)