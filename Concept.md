# Concept

## Problem

Today irrigation does often not keep into account possible water leaks from watering pipes. This superficial management is leading to a huge waste of water, which is one of main causes of drought. Agriculture is the most water demanding sector, with the 40% of total water usage per year in Europe.
In Italy over 26 billion of cube meters of water per year are consumed: around 55% is reserved for agriculture, 27% for the industry and 18% for the city. However, withdrawal of water effectively exceeds 33 billion of cube meters per year: this difference is motivated by the waste of around 22% of total water. The 17% of these leakages are related to the agricultural sector.

## Proposed solution

Our proposed solution consists of an interface through which it is possible to monitor the irrigation system. The user can know if a leakage is detected and where it is located, as well as the flow of water at the source site. Another available information is the past water usage schedule, with the relative periodical measurements of water flows.

## Use cases

Our system allows to do the following tasks:

* Leakages detection, quantification and localization
* Water usage monitoring

# User requirements
* Detect water leakages: how? Accuracy? FP/FN?
* Promptly: at most in 24 hours
* Alarm system: actuators
* Events: water movement (change of flow)
* LoRa is a suitable technology: low bandwidth (simple data to exchange), long range (large fields), low power (battery-powered MCUs)
* Works for 1 year on battery

# Simulation
We build a simulation infrastructure on IoT-LAB, in order to provide a large scale irrigation water leakage system and to test distributed interaction between MCUs. In detail, the system scheme is tree-based, with a root node corresponding to the source MCU, intermediate nodes corresponding to fork MCUs and leaf nodes corresponding to MCUs at valve site. The communication between simulated devices is implemented using LoRaWAN and TheThingsNetwork. 

# Prototype
We construct a real prototype to demonstrate the working principle of the system. It is made up of a linear pipe and two MCUs located at its endopoints, with a water flow sensor for each one, together with a water source and an intermediate valve used to simulate a leakage.
