# Technology

## High-level architecture 

![architecture](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/architecture.png)


## Components
Our infrastructure is composed of:
* Esp32 <br/>
* LoRa gateway to comunicate with the Cloud system (AWS) <br/>
* Cloud system (AWS) to collect and store data <br/>
* Electrovalve <br/>
* Water flow sensor <br/>
* Temperature humidity sensor <br/>

### Esp 32
The Esp32 manages the different sensors in the infrastructure and exchanges messages with the other microcontrollers and with the Cloud.

The main technical issue is that the device will be attached to every object of interest and should thus be battery powered. Implementation will be done via periodic interrupts from one of the MCU's internal timers. The task is relatively simple, so it imposes no particular constraints on the processor, and power consumption should be the main driving factor in the choice.

The transmission period will be determined through experimentation in order to provide most accurate data without saturating the network. If the evaluation shows this to be required a movement sensor will be added in order to limit the updates if the beacon is stationary.

### Cloud storage
The tagged and aggregated data is stored on AWS for long term storage. This data can then be queried by farmers in order to gain insights on the usage of water and to select a particular crop for a field.

## Algorithms and protocols 
![Field](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/field.png)

## Network architecture 
![network](https://github.com/simonescaccia/Smart-Irrigation-System/blob/main/images/network.png)


