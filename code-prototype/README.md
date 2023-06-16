# Irrigation Water Leakage System RIOT code-prototype

## Introduction
In this folder there are three directories, one per each algorithm implemented. <br/>
We suggest to use the algorithm SyncAck because it is the best one, read the evaluation in the documentation for the motivations. <br/>
There is also the code for the MQTT Bridge, this has to be executed in the device that you want to use as bridge for AWS. <br/>

## How to run the code
Firstly, you have to download mosquitto, go to the site https://github.com/eclipse/mosquitto and follow the instructions. <br/>

Secondly, you have to connect your computer and AWS following this guide https://aws.amazon.com/it/blogs/iot/how-to-bridge-mosquitto-mqtt-broker-to-aws-iot/. <br/>

Then, download RIOT from github:

    git clone https://github.com/RIOT-OS/RIOT.git

Go in RIOT folder and then in the directory /example, download here our code, go in the directory of the SyncAck code (you can do the same for the other algorithms):

    cd RIOT/examples/
    git clone https://github.com/simonescaccia/Irrigation-Water-Leakage-System.git
    cd /Irrigation-Water-Leakage-System/code-prototype/code-ack
    
Now try to do make of the code with command (change type of board and port if needed):

    sudo BOARD=esp32-heltec-lora32-v2 BUILD_IN_DOCKER=1 DOCKER="sudo docker" PORT=/dev/ttyUSB0 make all

If all works, connect your board and do (repeat the same thing for the other board, changing the value of the variable "BOARD" to 0 for Source or 1 for Son):

    sudo BOARD=esp32-heltec-lora32-v2 BUILD_IN_DOCKER=1 DOCKER="sudo docker" PORT=/dev/ttyUSB0 make flash term

Now your ESP32 are working and are sending messages to the MQTT topic selected. Open another terminal, go in the correct directory and launch the command:

    cd /Irrigation-Water-Leakage-System/code-prototype/
    python3 client.py
    
This script take the values sent by the ESP32s and redirect them to AWS.

### Common errors
For this project you need two different boards, one for the Source and one for the Son. <br/>
Before launching the code, change the private values of the code like information of the Wifii and AWS properties.

    
