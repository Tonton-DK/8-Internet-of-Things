import paho.mqtt.client as mqtt
import json
import math
import os

time = None
room1 = {
    'temp': None,
    'rhum': None
    }
room2 = {
    'temp': None,
    'rhum': None
    }

# Define callback functions for MQTT events
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

def absolute_humidity(T, H):
    return (6.112 * math.exp((17.67 * T)/(T + 243.5)) * H * 2.1674) / (273.15 + T)

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

    topic = msg.topic.split("/")
    sensor = topic[-1]

    payload = json.loads(msg.payload)
    timestamp = payload["time"]

    global time

    if (time is None):
        time = timestamp
    elif (time != timestamp):
        time = timestamp
        room1['temp'] = None
        room1['rhum'] = None
        room2['temp'] = None
        room2['rhum'] = None

    value = payload["value"]   

    if "room1" in msg.topic:
        room1[sensor] = value
    elif "room2" in msg.topic:
        room2[sensor] = value

    if (room1['temp'] != None and room1['rhum'] != None):
        temp = room1['temp']
        hum = room1['rhum']
        abs = absolute_humidity(temp, hum)
        # Publish the message to all subscribers
        client.publish("dk/sdu/iot/2023/anton/py/room1/abs", payload=json.dumps({
            "time": time,
            "value": abs
        }))
        room1['temp'] = None
        room1['rhum'] = None

    if (room2['temp'] != None and room2['rhum'] != None):
        temp = room2['temp']
        hum = room2['rhum']
        abs = absolute_humidity(temp, hum)
        # Publish the message to all subscribers
        client.publish("dk/sdu/iot/2023/anton/py/room2/abs", payload=json.dumps({
            "time": time,
            "value": abs
        }))
        room2['temp'] = None
        room2['rhum'] = None

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