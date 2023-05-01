# Design

## Architecture 
![Field1](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/field.jpg) <br/>
![Field2](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/field_alternative.jpg) <br/>
Our architecture is scalable and can be used to control water leakages within a small field, if there is the same crop, or can be used to control different types of crop in the same field. For every field a MCU at the fork site (that can be at different depth levels) is needed, plus a MCU for each pipeline branch.</br>
We have proposed two different possibilities of set of fields (we will analyse which one is the better or if there are other possible architecture).

## High level diagram
![architecture](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/architecture.jpg) <br/>
Our system is composed of 3 different pieces:
* MCU near the water source (connected through a power cord) called CHIEF
* MCU near the pipeline fork (using a battery) called FORK, with an ID for the fork and a NUMBER for each branch water flow sensor
* MCU connected to a water flow sensor at valve site (using a battery) called BRANCH and with a NUMBER (consistent with the NUMBER value in the relative FORK)

## Components
Our infrastructure is composed of:
* ESP32 MCU with LoRa support for peer and cloud communication 
* Water flow sensor
* LED
* Buzzer
* Cloud system (AWS) to collect and store data


### ESP32
The ESP32 manages the different sensors and actuators in the infrastructure and exchanges messages with the other microcontrollers and with the cloud. The main technical issue is that the device will be attached to every object of interest and thus should be battery powered:
* CHIEF is connected to a power cord and does not have energy problems
* FORK and BRANCH wakes up rarely and only when a test is needed, so it has a small duty cicle

### LoRa
LoRa is a physical radio communication protocol, based on spread spectrum modulation techniques derived from chirp spread spectrum (CSS) technology. The protocol is supported by our chosen ESP32 MCUs, allowing a low power and long range communication between microcontrollers in order to accomplish distributed tasks.

### Sensors
#### Water flow sensors
Water flow sensors are installed at the water source or pipes to measure the flow rate of water. The relative metric is as liters per hour or cubic meters, to be scaled in our context. The structure of the sensor consists of a plastic valve (from which water can pass) and a water rotor along with a Hall effect (a voltage difference is induced in the conductor due to the rotation of the rotor) sensor, measuring the sense and the intensity of the flow. When water flows through the valve, it causes a change of speed of the rotor, calculated as output as a pulse signal. The sensor contains three wires, one for supply voltage (5 V of DC), one for the ground and one to collect output from Hall effect sensor. 

### Actuators
#### LED

#### Buzzer

### Cloud system
Data are stored on AWS for long term storage. These data can then be queried by farmers in order to gain insights about water usage and pipeline leakages.

## Network architecture 
![network](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/network.jpg) <br/>
The network architecture is focused on checking the actual state of the irrigation system, with a communication between devices based on LoRaWAN and MQTT. We will also make some considerations about the scalability of different possible network schemes.

## Algorithms 
### Water leakage detection algorithm
![messages](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/messages.jpg) <br/>
This is the communication scheme between IoT elements, Edge components, and Cloud components, related to the algorithm for the leakage detection.
