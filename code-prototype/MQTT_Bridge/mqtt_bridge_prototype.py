from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import paho.mqtt.client as mqtt
import json
import time
from time import gmtime, strftime
from datetime import datetime
import signal

def on_message(_client, _userdata, message):

    date = datetime.now().strftime('%d/%m/%Y %H:%M:%S')

    print(date + ' - Received message: ' + str(message.payload))

    payload = json.loads(message.payload)
    if(payload['id']=='1'):
        json_payload = json.dumps({
        'Id': 'leakage',
        'Datetime': date,
        'Child': 'Prot_Son',
        'Father': 'Prot_Source',
        'Leakage': payload['flow_diff']
        })
    else:
        json_payload = json.dumps({
        'Id': 'flow',
        'Datetime': date,
        'Flow': payload['flow source']
        })

    # Topic will be MQTT_PUB_TOPIC_FIRE
    topic = MQTT_PUB_TOPIC_WATER

    success = myMQTTClient.publish(topic, json_payload, 0)

    time.sleep(5)
    if(success):
        print("published",json_payload)

    
# On connect subscribe to topic
def on_connect(_client, _userdata, _flags, result):
    """Subscribe to input topic"""

    print('Connected ' + str(result))
    myMQTTClient.publish(MQTT_PUB_TOPIC_WATER, "FIRST CONNECTION", 0)
    print("FIRST CONNECTION DONE")

    print('Subscribing to ' + MQTT_SUB_TOPIC)
    MQTT_CLIENT.subscribe(MQTT_SUB_TOPIC)

# Disconnect function
def disconnect_clients(signum, frame):
    MQTT_CLIENT.loop_stop()
    MQTT_CLIENT.disconnect()
    myMQTTClient.disconnect()
    print("DISCONNECTION")
    exit(0)

signal.signal(signal.SIGINT, disconnect_clients)

MQTT_BROKER_ADDR = "192.168.92.168"
MQTT_BROKER_PORT = 1883
MQTT_BROKER_CLIENT_ID = "broker"
AWS_IOT_ENDPOINT ="a1qrujr4l90wwz-ats.iot.us-east-1.amazonaws.com"
AWS_IOT_PORT = 8883
AWS_IOT_CLIENT_ID = "basicPubSub"
AWS_IOT_ROOT_CA = "./root-CA.crt"
AWS_IOT_PRIVATE_KEY = "./water-flow-sensor.private.key"
AWS_IOT_CERTIFICATE = "./water-flow-sensor.cert.pem"

# For certificate based connection
myMQTTClient = AWSIoTMQTTClient(AWS_IOT_CLIENT_ID)
myMQTTClient.configureEndpoint(AWS_IOT_ENDPOINT, 8883)
myMQTTClient.configureCredentials(AWS_IOT_ROOT_CA, AWS_IOT_PRIVATE_KEY, AWS_IOT_CERTIFICATE)
myMQTTClient.configureOfflinePublishQueueing(-1)
myMQTTClient.configureDrainingFrequency(2)
myMQTTClient.configureConnectDisconnectTimeout(10)
myMQTTClient.configureMQTTOperationTimeout(5)
MQTT_SUB_TOPIC = "home"
MQTT_PUB_TOPIC_WATER = "data/water"
MQTT_CLIENT = mqtt.Client(client_id=MQTT_BROKER_CLIENT_ID)

# MQTT callback function
def main():
    MQTT_CLIENT.on_connect = on_connect
    MQTT_CLIENT.on_message = on_message
    MQTT_CLIENT.connect(MQTT_BROKER_ADDR, MQTT_BROKER_PORT)
    myMQTTClient.connect()
    MQTT_CLIENT.loop_forever() #

if __name__ == '__main__':
    main()
