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
* LoRa gateway to comunicate with the Cloud system (AWS) 
* Cloud system (AWS) to collect and store data 
* Solenoid valve 
* Water flow sensor
* Temperature humidity sensor 

### Esp 32
The Esp32 manages the different sensors in the infrastructure and exchanges messages with the other microcontrollers and with the Cloud.

Te

### Cloud storage
The tagged and aggregated data is stored on AWS for long term storage. This data can then be queried by farmers in order to gain insights on the usage of water and to select a particular crop for a field.

## Algorithms and protocols 

## Network architecture 
![network](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/network.png)


