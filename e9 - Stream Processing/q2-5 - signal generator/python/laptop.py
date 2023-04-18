import paho.mqtt.client as mqtt
import json
import math
import time
import ctypes

def current_time():
    ts = time.time()
    ts_rounded = round(ts, 2)
    return ts_rounded

def uptime():
    # getting the library in which GetTickCount64() resides
    lib = ctypes.windll.kernel32
    
    # calling the function and storing the return value
    t = lib.GetTickCount64()
    
    # since the time is in milliseconds i.e. 1000 * seconds
    # therefore truncating the value
    t = int(str(t)[:-3])
    
    # extracting hours, minutes, seconds & days from t
    # variable (which stores total time in seconds)
    mins, sec = divmod(t, 60)
    hour, mins = divmod(mins, 60)
    days, hour = divmod(hour, 24)
    
    # formatting the time in readable form
    # (format = x days, HH:MM:SS)
    return f"{days} days, {hour:02}:{mins:02}:{sec:02}"

# Define callback functions for MQTT events
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))    
    print("Current time: " + str(current_time()))
    print("Uptime: " + uptime())
    client.publish("dk/sdu/iot/2023/anton/uptime", payload=json.dumps({
            "time": current_time(),
            "value": uptime()
        }))

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    # Publish the message to all subscribers
    client.publish(msg.topic, payload=msg.payload)

# Create a MQTT client instance
client = mqtt.Client()

# Set the callback functions
client.on_connect = on_connect
client.on_message = on_message

# Start the broker
client.enable_logger()
client.connect("broker.hivemq.com", 1883)
client.subscribe("dk/sdu/iot/2023/anton/mavg/#")
client.loop_forever()