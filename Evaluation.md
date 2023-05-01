# Evaluation

## Evaluation methodology: Overall System

### During the development of the project

General performance evaluation of the system:
We want to detect if a water leak occur within 24h, in order to detect water leakages without tolerating more than one day of water losses. We want also to know wich pipe is leaking.

We will use the following metrics to evaluate the performance of the system:

1. Leak is dectected within 24h
2. The pipe is correctly identified

### When the first complete version of the system will be ready for use

We want to detect if a water leak occur within 24h, in order to detect water leakages without tolerating more than one day of water losses. We want also to know wich pipe is leaking, providing information about the quantity of water leakage in a second.

1. Leak is dectected within 24h
2. The pipe is correctly identified
3. The quantity of water leaked per second with an accuracy of 95% (sensor precision error)

## Evaluation methodology: Individual Components

We wish to evaluate the power consumption of MCUs that are not attached to a power cord, so for the FORK and the BRANCH MCUs.

+ Power supply of source site MCU is provided by a power cord, hence there are no specific constraints on power consumption.
+ MCUs at fork sites and crop site, due to the limited workload, we wish to obtain a battery life equal to the total period of irrigation.

## Network Technologies Performances

+ Latency (s)
+ Throughput (bit/s)

## Algorithms Performances

### Water leak detection Algorithm

1. Test propagation phase

+ One message from the CHIEF to the FORK childs
+ One message from the FORK to the FORK childs
+ One message from the FORK to the BRANCH childs

2. If a leak is detected, the MCU that computes the check, will send a message to the cloud to notify the user

We will evaluate the number of messages sent by the protocol and the time needed to complete the algorithm.
The complexity will change depending on the number of FORKs, and we will compare variuous topologies in order to find the best one.

### Monitoring of the water flow Algorithm

From the CHIEF MCUs, send periodically a message to the cloud to update the water flow consumed.

Performances depends on the latency between the cloud and the MCUs.

## Response Time Performances

The response time between CHIEF, FORKs and BRANCHEs depends on the distance between the MCUs, so it depends on the topology of the irrigation system.
