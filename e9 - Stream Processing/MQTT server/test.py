import paho.mqtt.client as mqtt
import os

# Define callback functions for MQTT events
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

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
client.connect("localhost", 1883)
client.subscribe("/topic/qos0")
client.loop_forever()