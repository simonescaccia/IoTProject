# Technology

## Architecture 

### Architecture in the real world
![Field](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/Field.png) <br/>
Our architecture is scalable and can be used to control the water until a small field, if there is the same crop, or can be used to control different colture in the same field. For every field is needed a controller plus a controller for the pipeline branch.

### High level diagram
![architecture](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/architecture.png) <br/>
Our system is composed of 3 different pieces:
* Controller near the water source (connected by cable) called CHIEF
* Controller near the pipeline branch (using a battery) called BRANCH and with and ID for the branch and a NUMBER for each solenoid valve
* Controller for humidity sensor and solenoid valve (using a battery) called SENSOR and with and NUMBER (equal to the relevant valve)

## Components
Our infrastructure is composed of:
* Esp32 
* LoRa to comunicate between controllers and to comunicate with the Cloud system (AWS) 
* Solenoid valve 
* Water flow sensor
* Temperature humidity sensor 
* Cloud system (AWS) to collect and store data 

### Esp 32
The Esp32 manages the different sensors in the infrastructure and exchanges messages with the other microcontrollers and with the Cloud. The main technical issue is that the device will be attached to every object of interest and should thus be battery powered:
* CHIEF is connected to the cable and does not have energy problems (could be a LoRa Gateway and not a Thing???)
* BRANCH gets up rarely and only when a test is needed, small duty cicle
* SENSOR gets up every time the humidity sensor causes an event

### LoRa
???????

### Cloud storage
The tagged and aggregated data is stored on AWS for long term storage. This data can then be queried by farmers in order to gain insights on the usage of water and to select a particular crop for a field.

### Frontend
An interactive dashboard will provide a way to visualize the data.

## Algorithms and protocols 
![network](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/network.png) <br/>
This is the communication between IoT elements, Edge components, and Cloud components. Building the system we will test this communication algorithm. 

## Network architecture 
