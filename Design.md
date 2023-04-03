# Design

## Architecture 
![Field](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/Field.png) <br/>
Our architecture is scalable and can be used to control the water within a small field, if there is the same crop, or can be used to control different types of crop in the same field. For every field a MCU at the fork site (that can be at different depth levels) is needed, plus a MCU for each pipeline branch.

## High level diagram
![architecture](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/architecture.png) <br/>
Our system is composed of 3 different pieces:
* MCU near the water source (connected through a power cord) called CHIEF
* MCU near the pipeline fork (using a battery) called FORK, with an ID for the fork and a NUMBER for each branch water flow sensor
* MCU connected to a soil humidity sensor and a solenoid valve (using a battery) called BRANCH and with a NUMBER (consistent with the NUMBER value in the relative FORK)

## Components
Our infrastructure is composed of:
* ESP32 MCU with LoRa support for peer and cloud communication 
* Water flow sensor
* Soil humidity sensor 
* Solenoid valve
* Cloud system (AWS) to collect and store data


### ESP32
The ESP32 manages the different sensors and actuators in the infrastructure and exchanges messages with the other microcontrollers and with the cloud. The main technical issue is that the device will be attached to every object of interest and thus should be battery powered:
* CHIEF is connected to a power cord and does not have energy problems (could be a LoRa Gateway and not a Thing???)
* FORK wakes up rarely and only when a test is needed, so it has a small duty cicle
* BRANCH wakes up periodically in order to check the humidity of the soil and possibly irrigate crop

### LoRa
???????


### Sensors
#### Water flow sensors
Water flow sensors are installed at the water source or pipes to measure the flow rate of water. The relative metric is as liters per hour or cubic meters, to be scaled in our context. The structure of the sensor consists of a plastic valve (from which water can pass) and a water rotor along with a Hall effect (a voltage difference is induced in the conductor due to the rotation of the rotor) sensor, measuring the sense and the intensity of the flow. When water flows through the valve, it causes a change of speed of the rotor, calculated as output as a pulse signal. The sensor contains three wires, one for supply voltage (5 V of DC), one for the ground and one to collect output from Hall effect sensor. 

#### Soil humidity sensors
Soil humidity sensors are fork-shaped probes inserted into the soil, with two exposed conductors, hence acting as a variable resistor. The resistance varies inversely with soil humidity. In fact capacitance is used in order to measure dielectric permittivity of the soil, which is a function of the water content. The sensor produces an output voltage proportional to the dielectric permittivity (so inversely proportional to resistance), and therefore the water content of the soil (soil moisture level). Only four pins are required to connect: AO (Analog Output), generating analog output voltage, DO (Digital Output) indicating whether the soil moisture level is within the limit, VCC for power supply (from 3.3 V to 5 V) and GND for the ground.

### Actuator
#### Solenoid valve

### Cloud storage
The tagged and aggregated data are stored on AWS for long term storage. These data can then be queried by farmers in order to gain insights on the usage of water and to select a particular crop for a field.

### Frontend
An interactive dashboard will provide a way to visualize the data.

## Algorithms and protocols 
![network](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/network.png) <br/>
This is the communication scheme for IoT elements, Edge components, and Cloud components. Building the system we will test this communication algorithm. 

## Network architecture 
