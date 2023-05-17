# Concept

## Problem

Today irrigation does often not keep into account possible water leaks from watering pipes. This superficial management is leading to a huge waste of water, which is one of main causes of drought. Agriculture is the most water demanding sector, with the 40% of total water usage per year in Europe.
In Italy over 26 billion of cube meters of water per year are consumed: around 55% is reserved for agriculture, 27% for the industry and 18% for the city. However, withdrawal of water effectively exceeds 33 billion of cube meters per year: this difference is motivated by the waste of around 22% of total water and 17% of these leakages are related to the agricultural sector. However, our project can also be used to monitor the state of regular water distribution system, if a proper adaptation is given. 

## Proposed solution

Our proposed solution consists of an interface through which it is possible to monitor the irrigation system. The user can know if a leakage is detected and where it is located, as well as the flow of water at the source site. Another available information is the past water usage schedule, with the relative periodical measurements of water flows.

## Use cases

Our system allows to do the following tasks:

* Leakages detection, quantification and localization
* Water usage monitoring

## User requirements
The starting point of our analysis is the collection of user requirements. In our context, there are some fixed parameters, mainly related to the chosen infrastructure, while other parameters should be finetuned according to the specific final user of the application, ideally a farmer owning a variable field. To cope with this issue, we made reasonable assumptions about these parameters, and we also guaranteed the scalability of the system according to user needs.

### Detect water leakages
* How can we detect leakages? Conceptually, we measure the water flow at different adjacent sites of the irrigation system and we compare these measurements in order to detect anomalies, according to a specific detection algorithm.
* The accuracy is computed considering the percentage of correct leakage identification on the total number of experiments.
* We also thought about the possibility to binary classify every state as faulty or as correct, focusing on the analysis of False Positive and False Negative events.

### Reaction time
We thougt about a proper time bound on the interval from leakage manifestation until detection. Some common values for water flow rate given by irrigation pumps are from 20 to 80 litters of water per minute. Now, there is much variability as concerns the irrigation procedure, depending on the type of crop. However, in most cases, the irrigation procedure should last only few minutes per day. Assuming also a severe leakage, it is difficult that more than 30% of water will be lost. So, in the worst case, from 6 to 24 liters of water per minute will be lost. A reasonable assumption is to tolerate at most 24 hours of no detection. These considerations are at the basis of our concept of duty cycle.

### Alarm system
We thought about suitable actuators to use in order to indicate an alarm condition. A simple LED with a buzzer are certainly a good option.
Obiouvsly, it makes no sense to place these actuators in the middle of a field, because it is unlikely that someone will see or hear something. Instead, they can be placed near water source, so they can be easily noticed and constantly connected to electric power.

### Events 
The main events that we are interested in are certainly water flow changes. We wish to monitor the water flow at the source and also the one at different sites of the system, in order to compare measurements and detect possible changes of flow.

### Communication protocol
LoRa is a suitable technology for different reasons:
* It uses low bandwidth, and this is a great advantage in our context. In fact, we need to exhange simple data, namely water flow measurements, so LoRa is certainly a good option.
* It can send data on long ranges. It is not an immediate advantage in our architecture, because we implemented a peer-to-peer communication with adjacent nodes. However, it can be very useful firstly to reach also far LoRa gateways and on large scale systems to exchange data between distant nodes, if needed.
* It works with low power consumption, and this is a crucial added value to the protocol since we use battery-powered MCUs.

### Energy consumption
* We thought about the best battery lifetime for our MCUs and our conclusion was to reason over the needed sampling cycles and the utilization period of the system. So, 1 year of battery life was considered as a good option.
* We also thought about the possibility to recover energy from water flow sensor dynamics, for example with a dynamo. However, this solution was not feasible for our hardware because of the impossibility to access to the enclosed rotor in a proper way.

## Simulation
We built a simulation infrastructure on IoT-LAB, in order to provide a large scale irrigation water leakage system and to test distributed interaction between MCUs. In detail, the system scheme is tree-based, with a root node corresponding to the source MCU, intermediate nodes corresponding to fork MCUs and leaf nodes corresponding to MCUs at valve site. The communication between simulated devices is implemented using LoRaMAC, while LoRaWAN and TheThingsNetwork are used to send data to the cloud.

## Prototype
We construct a real prototype to demonstrate the working principle of the system. It is made up of a linear pipe and two MCUs located at its endopoints, with a water flow sensor for each one, together with a water source and an intermediate valve used to simulate a leakage.

Unfortunately, we found some problems working with LoRa with our chosen ESP32, even if it should be officially supported by RIOT. So, in order to overcome this issue, we decided to switch to WiFi technology only for demonstating purposes. The idea is that when LoRa issue will be hopefully solved soon our project can be adapted with minor changes. Even if we will not obiouvsly have the same performance of LoRa, the procedure of evaluation is identical.
