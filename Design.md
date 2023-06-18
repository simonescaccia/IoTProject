# Design

## Components

Our infrastructure is composed of:

* ESP32 MCU with LoRa support for peer and cloud communication
* Water flow sensor
* LED
* Buzzer
* Cloud system (AWS) to collect and store data

### ESP32

The ESP32 manages the different sensors and actuators in the infrastructure and exchanges messages with the other microcontrollers and with the cloud. The main technical issue is that the device will be attached to every object of interest and thus should be battery powered:

* CHIEF is connected to a power cord and does not have energy constraints, however a duty cycle is implemented as well
* FORK and BRANCH wakes up only when a test is needed, so we modulated a proper duty cicle in order to provide a longer battery lifetime

### LoRa

LoRa is a physical radio communication protocol, based on spread spectrum modulation techniques derived from chirp spread spectrum (CSS) technology. The protocol is supported by our chosen ESP32 MCUs, allowing a low power and long range communication between microcontrollers in order to accomplish distributed tasks.

### Sensors

#### Water flow sensors

Water flow sensors are installed at the water source or pipes to measure the flow rate of water. The chosen metric for flow measurement is liters per minute. The structure of the sensor consists of a plastic valve (from which water can pass) and a water rotor along with a Hall effect sensor (a voltage difference is induced in the conductor due to the rotation of the rotor), measuring the sense and the intensity of the flow. When water flows through the valve, it causes a change of speed of the rotor, calculated as output as a pulse signal. The sensor is powered with a 5 V supply voltage of DC.

Water flow sensors used for our protoype can be found [here](https://www.amazon.it/dp/B079QYRQT5?psc=1&ref=ppx_yo2ov_dt_b_product_details)

### Actuators

#### LED

A LED is used to provide a visual alarm indication through blinking.

#### Buzzer

A buzzer is used to provide an acoustic alarm indication through intermitting activation.

### Cloud system

Data are stored on AWS for long term storage. Then these data can be displayed by farmers in order to gain insights about water usage, in detail the water flow at the source, and pipeline leakages, with the indication of detected location.

## How to detect a leakage

As a first step, we have to reason about our main requirement, leakage detection. In particular, we need to do a feasibility study about how to detect a leakage in a pipeline first, and then how to detect it in a distributed system. The following images show our reasoning process. <br> </br>

![no_leakage](./images/HowToDetectLeakages-No_leakage_situations.drawio.png)
<br> </br>
![leakage](./images/HowToDetectLeakages-Leakage_situations.drawio.png)
<br> </br>
![distributed_problem](./images/HowToDetectLeakages-Distributed_system_leakage_problem.drawio.png)
<br> </br>
![distributed_solution](./images/HowToDetectLeakages-Distributed_system_leakage_solution.drawio.png)
<br> </br>
![distributed_other](./images/HowToDetectLeakages-Distributed_system_other_leakages.drawio.png)

## Architecture

![field](./images/field.jpg)

We propose a tree architecture with a MCU at source site connected to a water flow sensor, a LED and a buzzer. A fork is defined as a site where a single pipe divides itself in two or more output pipes. We place a water flow sensor for each of these diramations, all connected to a single MCU. So. a MCU at each fork site, which can be at different depth levels, is needed. Furthermore, we place a MCU for each pipeline branch, more precisely in correspondence of each irrigation valve, where we place a single water flow sensor connected to the MCU. Our architecture is scalable, so it can be used to control water leakages within a small field with the same crop or to control leakages from pipes irrigating different types of crop in the same field. </br>

Since in the simulation we used a node for each MCU, our tree architecture can be represented in a more understandable way as follows.

![tree]()

## High level diagram

![architecture](./images/architecture.jpg)

Our system is composed of 3 different pieces:

* MCU near the water source (connected through a power cord) called CHIEF
* MCU near the pipeline fork (using a battery) called FORK
* MCU connected to a water flow sensor at valve site (using a battery) called BRANCH

Every node has an identificative number (in the case of the simulation given by IoT-Lab) and we designed a properly configuration function to extract the information for each node from the topology, given as father-child pairs.

## Prototype Environment

For our prototype, we used the fllowing objects:

* A 1,5 meters long garden hose (20-25 mm ⌀)
* A 1/2 inch garden tap
* A 1/2 inch T-adapter
* 6 metal adjustable hose clamps (16-25 mm)
* 2 water flow sensors
* 2 ESP32 LoRa MCUs  

We started from a 1,5 meters long garden hose with an inner diameter of 20 mm and an outer one of 25 mm. We cut the garden hose into 4 main segments approximately 35 cm long. We placed two segments at the inner endpoints of each water flow sensor and we joined them using the T-adapter. So we connected the tap to the last free endpoint of the T-adapter and the last two segments to the outer endpoints of water flow sensor. We defined a direction for the flow placing the water flow sensors in the correct flow measurement direction. Finally, we connected the water flow sensors to the MCUs and the MCUs to a battery.
The tap is used to simulate a leakage and it is initially closed. We did not use clamps at first, but we experienced water leaks in correspondence of joints. Therefore we placed 2 clamps at the outer endpoints of each water flow sensor and other 2 for the endpoints of the T-adapter not connected to the tap. In this way we solved the problem of leakages at joints. The water flow sensor of father MCU and the water flow sensor of child MCU are 90cm apart.

## Network architecture

![network](./images/network.jpg)

The network architecture is focused on checking the actual state of the irrigation system, with a communication between devices based on LoRaWAN and MQTT. We will also make some considerations about the scalability of different possible network schemes.

![AWS](./images/AWS_architecture.png)
The AWS architecture shows both the link with the cloud of the prototype and the simulation.

## Algorithms

### Water leakage detection algorithm

In the following we describe the communication scheme between IoT elements, Edge components, and Cloud components, related to the algorithm for the leakage detection.

We analyse the water flow independently for each segment, intended as the pipe between two adjacent water flow sensors. Given a segment, we will call the node near the water source as 'parent' and the other one as 'child'.
Output water flows at fork site will be evaluated according to given paramters as pipe length or slope.

Test algorithm:

1. Node MCU sends water flow value to its children, if any.
2. If it is a FORK, child MCU computes expected values of water flow, by summing the values of its sensors.
3. Child MCU compares received value with its own value.
4. In case of leakage, child sends an alert to the cloud.

We have tried 3 different algorithms for allow a correct and synchronized test between two diffent nodes. The three algorthms are:

* Ack
* Handdshake
* SyncAck
All the algorithms are available in the directory "code-prototype"

#### Ack
![ack](./images/ack.png)

This is the first algorithm that we have implemented and it does not work well. Firstly there is a bug in the code, in fact with this code the Son has an higher flow that the Source, and this is not physically possible (if you find the problem, please tell us where is it), Secondly with the data obtained, we have seen that the water flow arriving to the Source (before our architecture) is not stable and the code was not robust against this problem.

In particular the Source controls if there is a water flow, if yes it does the test and sends the value to the Son; the Son obtains the value, sends an "answr" to the Source (used to control the Son is not broken) and starts the test; after that it does the difference between the two values and see if there is a leakage or not, if yes it sends the value of the leakage in L/min to AWS.

For the motivation written before, this system is proned to the false positive and we have stopped the analysis on it.

#### Handshake
![handshake](./images/handshake.png)

This is the second algorithm that we have implemented and it works well.

In particular the Source controls if there is a water flow, if yes it sends an "heloy" to the Son, the Son sends an "answr" to the Source, Source sends the message "start" and starts the test; when the Son obtains the "start" it also starts the test; the Source sends to the Son the result of the test and, when the Son has both the values, sees if there is a leakage, if yes it sends the value of the leakage in L/min to AWS.

This system works well but it is uselessly complex and it is less efficient than the next one.

#### SyncAck

![syncAck](./images/syncAck.png)

This is the third algorithm that we have implemented and it is the one that will be used in the real scenario.

In particular the Source controls if there is a water flow, if yes it sends an "heloy" to the Son, the Son sends an "answr" to the Source and it also starts the test, Source receives the message "answr" and starts the test; Source sends to the Son the result of the test; when the Son has both the values, sees if there is a leakage, if yes it sends the value of the leakage in L/min to AWS.

This system works well and uses one less message.

In the evaluetion part, it is available the different analysis on the data.

## Simulation

### Assumptions

The simulation aims to detect leakages in a scalable distributed infrastructure, while the prototype focus on detecting leakages for a single pipe. For this reason, in the simulation we have made some assumptions:

* A code function simulates samples to avoid delays in message propagation, false positives, and all the problems discussed in the prototype. Instead, we will use a logic time to simulate the sampling at the same time.
* Forks on a pipe will divide the water flow into equal parts.
* Since we are simulating samples, we can set the threshold for leakages to 0.

### Iot-Lab Architecture

As mentioned before, to build a scalable infrastucture we need to use three different types of nodes: CHIEF, FORK, and BRANCH. They can cover all the possible cases of a distributed infrastructure.
The minimal architecture of the system should be the following:

![simulation_goal](./images/Simulation-Goal.drawio.png)

Node configuration:

* Due to the fact that the firmware is distributed, we need to use the same firmware for all the nodes. For this reason, we need to build a configuration file named config.c that can be used for all the nodes, to understand which type of node is and who are its children.
After flashing the firmware we need to assign an ID to each node compliant with the configuration file.

Sample generator:

* The sample generator is a function that simulates the sampling of the water flow. Its aim is also to test the various leakage scenario discussed in the 'How to detect a leakge' paragraph.

Comunication protocols:

* The nodes communicate with each other using RIOT-OS Semtech SX127x radios drivers implementation, sending asynchronous messages containing the water flow value.
* Each node sends leakage messages to The Things Network (TTN) using a RIOT adaption of Semtech LoRaMAC implementation.

But, during the implementation we have found some difficulties on integrating the Semtech LoRaMAC implementation with the RIOT-OS Semtech SX127x radios drivers implementation. In particular, initialising both the drivers on the same node, causes some conflicts that invalidate the communication of both node-TTN ans node-to-node.
For this reason we have decided to use another node to connect our infrastructure to TTN, and in order to be able to send leakage messages generated by the nodes using LoRaMAC, we have decided to pick these leakage information directly from the sample generator since building a UART communication between the nodes requires time that can be used to solve our initial problem i.e. the conflicts between the LoRa drivers.

So, our implementation is the following:

![simulation_implementation](./images/Simulation-Implementation.drawio.png)
