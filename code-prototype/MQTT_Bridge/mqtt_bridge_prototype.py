from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import paho.mqtt.client as mqtt
import json
import time
from time import gmtime, strftime
from datetime import datetime
import signal

def on_message(_client, _userdata, message):

    #a='13/06/2023 17:17:17'

    date = datetime.now().strftime('%d/%m/%Y %H:%M:%S')

    print(date + ' - Received message: ' + str(message.payload))

    payload = json.loads(message.payload)
    if (payload['id']=='1'):
        if (payload['value']=='LEAKAGE'):
            json_payload = json.dumps({
            'Id': 'leakage',
            'Datetime': date,
            'Child': 'Prot_Son',
            'Father': 'Prot_Source',
            'Leakage': payload['flow_diff']
            })
            # Topic will be MQTT_PUB_TOPIC_FIRE
            topic = MQTT_PUB_TOPIC_WATER

            success = myMQTTClient.publish(topic, json_payload, 0)

            time.sleep(5)
            if(success):
                print("published",json_payload)
        else:
            print("Not sent")

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

    myMQTTClient.publish(MQTT_PUB_TOPIC_WATER, "Connection", 0)
    print("Connection Done")

    MQTT_CLIENT.subscribe(MQTT_SUB_TOPIC)
    print('Subscribed to ' + MQTT_SUB_TOPIC)



MQTT_BROKER_ADDR = "PUT YOUR ADDRESS"
MQTT_BROKER_PORT = 1883
MQTT_BROKER_CLIENT_ID = "broker"
AWS_IOT_ENDPOINT ="PUT YOUR ENDPOINT"
AWS_IOT_PORT = 8883
AWS_IOT_CLIENT_ID = "basicPubSub"
AWS_IOT_ROOT_CA = "PUT YOUR AWS_IOT_ROOT_CA"
AWS_IOT_PRIVATE_KEY = "PUT YOUR AWS_IOT_PRIVATE_KEY"
AWS_IOT_CERTIFICATE = "PUT YOUR AWS_IOT_CERTIFICATE"

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
