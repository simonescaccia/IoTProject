# 2nd delivery

## Comments received

* Explain how a leak is detected and why we have choosen this architecture.
* Explain the problematics about the integration of RIOT LoraWan driver and the LoraMac driver (ttn vs p2p).
* Duty cycle: asynchronicity vs synchronicity to exchange messages.
* Sensing: how to avoid data noise. Possible solution: sliding window. Compare more samplings vs one sampling with a bigger duration.

## Changes

The most important changes in this delivery are:

* The first version of the prototype.
* First considerations about the algorithms for exchange messages between nodes.
* First try to formalize energy consumption, considering the algorithms choosen.
* First experiments on IoT Lab: leakage detection on a distributed system.

## Future work

* Experiments on prototype data and on IoT Lab, focussing on energy.
* Reasoning about improvement of the first version of the prototype and energy consumption.
* Web architecture: AWS.
* Improve the documentation, showing all the reasoning behind the choices made.
