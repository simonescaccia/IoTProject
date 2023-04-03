# Evaluation

## Evaluation methodology: Overall System

### During the development of the project

General performance evaluation of the system:
We want to detect if a water leak occur within 24h, in order to be able to repair it the next working day so that we can reduce water loss. Also, we want to know wich pipe is leaking.

We will use the following metrics to evaluate the performance of the system:

1. Leak is dectected within 24h
2. The pipe is correctly identified

### When the first complete version of the system will be ready for use

We will want to detect if a water leak occur within 24h, in order to be able to repair it the next working day so that we can reduce water loss. Also, we want to know wich pipe is leaking, providing information about the quantity of water leakage in a second.

1. Leak is dectected within 24h
2. The pipe is correctly identified
3. The quantity of water leaked per second with an accuracy of 95% (sensor precision error)

## Evaluation methodology: Individual Components

(CHIEF, FORK, BRANCH)
We wish to evaluate the power consumption of MCUs that are not attached to a power cord, so for the FORK and the BRANCH MCUs. Our goal is to make the batteries last for 1 year. For the CHIEF MCUs, we want to evaluate the balancing of load of requests, in particular when scaling the system. Needed metrics will be respectively the number of Wh and the average number of requests per hour.

## Network Technologies Performances

## Algorithms Performances

### Water leak detection Algorithm:

1. Test propagation phase: Close all the solenoid valves

+ One message from the CHIEF to the FORK childs
+ One message from the FORK to the FORK childs
+ One message from the FORK to the BRANCH childs

2. Detection phase: Check if the FORK recognize a water flow

+ One message from the BRACH to the FORK parent
+ One message from the FORK to the FORK parent
+ One message from the FORK to the CHIEF parent

3. Continue working phase: Test ended, continue working as usual

+ One message from the CHIEF to the FORK childs
+ One message from the FORK to the FORK childs
+ One message from the FORK to the BRANCH childs

4. If a leak is detected, the system will send a message to the cloud to notify the user

### Irrigation Algorithm:

1. Check if the soil is dry:
2. If the soil is dry, open the solenoid valve
3. Check if the soil is wet:
4. If the soil is wet, close the solenoid valve

### Management of the water flow Algorithm:

1. From the cloud, send a message to the BRANCH MCUs to close the solenoid valves

### Monitoring of the water flow Algorithm:

1. From the CHIEF MCUs, send periodically a message to the cloud to update the water flow consumed

## Embedded Devices Performances

## Response Time Performances
