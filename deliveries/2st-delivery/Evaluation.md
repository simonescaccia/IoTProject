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
Before designing a water leak detection algorithm, we focused on the dynamics of water in pipes. Since we are dealing with an irrigation system, a reasonable assumption is to consider water in pipes under pressure. It means that there are never empty pipes. Now, we can have two conditions, either stationary water or moving water, due to the increasing pressure at the source during irrigation time. This means that we can ignore the specific condition when doing a test, because the difference between adjacent flows will be bounded in both cases if there is no leakage, and not bounded otherwise. The detection algorithm starts from the source and propagates through the tree topology, always with the same logic for every father-child pair. The father sends water flow information to the child, which makes a proper computation and compares the obtained value with the received one, using a specific threshold. In case of leakage detection, it will communicate the leakage to the cloud through the LoRa gateway.

One of the main problems related to the test is related to synchronization, because of the delay related to transmission latency. We thought about an handshaking approach to take into account this discrepancy and to compare water flows starting the sampling process at two close moments.

1. Test propagation phase
2. Leak notification

The complexity will change depending on the number of FORKs, and we will compare variuous topologies in order to find the best one.

### Energy consumption
Our requirement is not to tolerate a water loss of more than one day, so we wish to detect a leakage within 24 h. Now, since the leakage is an unpredictable event, we cannot define a precise strategy apriori, but we want to indentify the best one in order to minimize power consumption. We compute this strategy analitically. Firstly, for simplicity, we focus on a simple father-child pair, since the same reasoning holds for every adjacent pair of nodes of the tree topology. Now, we define x as the send rate (msg/day) of the father, so the number of messages sent per day, and y as the total listen interval of the child (in hours/day). In order to be sure to correctly listen to at least one message in one day, y should be equal to (24/x + epsilon) hours/day, where epsilon is a neglectable time interval if compared with 24/x hours. Computing the energy consumption, there are three contributions, one related to the sender, one to the receiver and one to the exchange of messages linked to the leakage detection algorithm, so both for synchronization and test. This last component can be omitted in our considerations because it is not influenced by the choice of x, which we wish to know. So, the total energy consumption is given by:


$$E(x,y) = E(x) + E(y) + E(algorithm) \approx E(x) + E(y)$$

The two components are: 
$$E(x) = P_{trans} \cdot t_{trans} + (P_{on} \cdot t_{on}) \cdot x +(P_{off} \cdot t_{off}) \cdot x$$

$$E(y) = P_{listen} \cdot y + P_{on} \cdot t_{on} + P_{off} \cdot t_{off}$$

Our analysis is hence focused on finding the integer value of x (number of messages per day of the father) such that the total energy consumption is minimized. The listen time of the child will depends on x value. 


### Monitoring of the water flow Algorithm

From the CHIEF MCUs, send periodically a message to the cloud to update the water flow consumed.
Performances depends on the latency between the cloud and the MCUs.

## Water flow sensor for energy harvesting
This water flow sensor can not create energy for our batteries beacuse the electronic part is not connected to the turbine, it is only connected to the Hall Effect sensor.

To allow the harvesting of the energy, we should have an alternator connected to the turbine to transform the mechanical energy into electrical energy.

A possible solution for the energy harvesting is here: https://www.ebay.it/itm/322724692568 with the tutorial of the turbine https://www.youtube.com/watch?v=mtMO3VmCmiQ .

We should for example connect this type of turbine called micro hydro water turbine generator to our water flow sensor.

## Response Time Performances

The response time between CHIEF, FORKs and BRANCHEs depends on the distance between the MCUs, so it depends on the topology of the irrigation system.
