import speech_recognition as sr
import requests
import pyttsx3
from pyttsx3 import speak
url = "https://grouprproject.uk/api/datatext"
url2 = "https://grouprproject.uk/api/data/latest"
url3 = "https://grouprproject.uk/api/data/light"
url4 = "https://grouprproject.uk/api/light"
engine = pyttsx3.init()
import json

# your JSON data from the API
data = {
    "temperature": 25.7,
    "humidity": 60.4,
    "have_fire": 0,
    "light_level": 2330,
    "temperature2": 34.16,
    "timestamp": "2025-12-06T12:45:39.183Z"
}

# extract temperature


# convert to speech text

# Create recognizer
recognizer = sr.Recognizer()

def execute_command(text):
    """Handle recognized commands here."""
    text = text.lower()
    print("in execute_command")
    if "temperature" in text:
        response = requests.get(url2, timeout=10)
        data = response.json()
        temperature = data["temperature"]
        text = f"The current temperature is {temperature} degrees."
        speak(text)
    elif "light" in text:
        response = requests.get(url2, timeout=10)
        data = response.json()
        light = data["light_level"]
        text = f"The current light level is {light} "
        speak(text)
    elif "fire" in text:
        response = requests.get(url2, timeout=10)
        data = response.json()
        fire = data["have_fire"]
        if fire == 1:
            text = f"There is a fire detected "
        else:
            text = f"There is no fire detected "
        speak(text)
        # os.system("shutdown /s /t 1")
    elif "humidity" in text:
        response = requests.get(url2, timeout=10)
        data = response.json()
        humidity = data["humidity"]
        text = f"The current humidity is {humidity} percent."
        speak(text)
    elif "led" in text:
        response = requests.get(url3, timeout=10)
        li = response.json()
        text = "turn off LED"

        print(li)
        if li == 1:
            text = "turn off LED"
        else:
            text = "turn on LED"
            
        response2 = requests.get(url4, timeout=10)
        speak(text)
    else:
        print(f"Command not recognized: {text}")

def listen():
    with sr.Microphone() as source:
        print("Listening...")
        recognizer.adjust_for_ambient_noise(source)
        audio = recognizer.listen(source)

        try:
            print("Recognizing...")
            text = recognizer.recognize_google(audio)
            print("You said:", text)
            data = {"message": text}
            requests.post(url, json=data)
            execute_command(text)

        except sr.UnknownValueError:
            print("Sorry, I didn't catch that.")

        except sr.RequestError:
            print("API unavailable or no internet.")

# Main loop
if __name__ == "__main__":
    while True:
        listen()
