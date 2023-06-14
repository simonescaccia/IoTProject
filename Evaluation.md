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
Before designing a water leak detection algorithm, we focused on the dynamics of water in pipes. Since we are dealing with an irrigation system, a reasonable assumption is to consider **water in pipes under pressure**. It means that there are never empty pipes. Now, we can have two conditions, either stationary water or moving water, due to the increasing pressure at the source during irrigation time. This means that we can ignore the specific condition when doing a test, because the difference between adjacent flows will be bounded in both cases if there is no leakage, and not bounded otherwise. The detection algorithm starts from the source and propagates through the tree topology, always with the same logic for every father-child pair. The father sends water flow information to the child, which makes a proper computation and compares the obtained value with the received one, using a specific threshold. In case of leakage detection, it will communicate the leakage to the cloud through the LoRa gateway.

One of the main problems related to the test is related to synchronization, because of the delay related to transmission latency. We thought about an handshaking approach to take into account this discrepancy and to compare water flows starting the sampling process at two close moments.

1. Test propagation phase
2. Leak notification

The complexity will change depending on the number of FORKs, and we will compare variuous topologies in order to find the best one.

The three algorithms proposed in the design phase have been tested. The test of the Ack algorithm has shown that there is a problem in the code (caused by the use of threads in the program) and that the water flow before our system (that we consider always present) is not stable and this issue has to be solved by the synchronization algorithm. 
The problem of stability of the initial water flow can be seen in the following image in which there is a segmentation of the lines of the graph.
![ack](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/ack_flow.png) <br/>
About the Ack algorithm, it is possible to notice that it is good to detect a leakage but it says that the water flow of the Son is always higher than the water flow of the Source, and this can lead to error, so it is not the worst algorithm implemented.
![ack1](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/ack_test1.png)
![ack2](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/ack_test2.png)<br/>

About the Hanshake algorithm, it is perfect to detect the leakage and the trend of the Son higher than the Source is disappeared. There still is an error of the turbines, that we will face in the next chapter.
![h1](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/handshake_test1.png)
![h2](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/handshake_test2.png)<br/>

In the end, about the syncAck algorithm, it is possible to see the same trend of the handshake algorithm but with one less message. 
![s](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/syncAck_test1.png)<br/>

### Water leak detection Threshold
We have done some analysis to set a correct threshold in our algorithms to limit the presence of false positives and false negatives. We have also thought that a false negative is more serious than a false positive, and so our system will be prone to false positives.<br/>
We have analysed the time of the Handshake and SyncAck algorithm to understand the error that can be created and to use this values for the calucus of the energy consumption.
We have analysed that the standard deviation is high and this means that there is a large distribution of the data, in particular this means that the time taken by the messages and the code is fluctuating. Looking at the scheme, it is possible to understand the time of the messages and also the quality of the synchronization of the algorithms. <br/>**Hanshake**<br/>
![s](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/images/handshake_time.png)<br/>
Here there are 0,308s in the Son between the end of the test and the arrival of the value of the Source, knowing that in average the message takes 0,380s from the sender to the receiver, this means that the Source has ended the test 0,080s before the Son. We double it (also for the difference in starting time) and we obtain 0,160s.

<br/>**SyncAck**<br/>
![s](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/images/syncAck_time.png)<br/>
Here there are 0,325s(message) + 0,159s(difference) in the Son between the end of the test and the arrival of the value of the Source, knowing that in average the message takes 0,326s from the sender to the receiver, this means that the Source has ended the test 0,159s before the Son. We double it (also for the difference in starting time) and we obtain 0,318s. 

<br/>**Difference**<br/>
To find the error of the two algorithm we have to decide the time of testing: now is 3s but if we increase it, the influence of the error derived by the synchronization problem is less.
If we put the time of the test at 10s, we have that 0,160 : 10 = x : 100 and that 0,318 : 10 = x : 100. So, influence of handshake is x = 1,6% , while influence of syncAck is y = 3,18% .
Looking at the number of impulses per minute in the datasheet, that is 541 impulses/min, we can find the error of impulses derived by the percentage.<br/>
541imp / 60s and 30L / 60s -> 30L / 541imp <br/>
x = 1,6% -> flow = L/min = 541imp / 60s * 1,6% * 30L / 541imp = 0,48 L/min <br/>
y = 3,18% -> flow = L/min = 541imp / 60s * 3,18% * 30L / 541imp = 0,95 L/min <br/>
The syncAck is worse than the handshake but it uses one less message. Beacuse the instrumental error is higher, as we will see, the thing that is significant is the number of messages and so the syncACK will be the final algorithm of our application.<br/>
<br/> *It is possible to improve the algorithms using correctly a 'sleep' for some milliseconds; but, because the standard deviation is high, more data are needed to be accurate.*

### Turbine error
Another significant error of the architecture is the instrumental error of the water flow sensor. Because of we have not another turbine with the correct value of the water flow or other instruments, we have run the system for several tests and we have analysed the difference between the values of Source and Son. We have observed the difference because the water flow before our application is not costant (decided by the public pipeline).
We have found that . .. . .. . . . . 

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
<img src="https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/images/sensor_and_energy_image.png"  width="40%" height="40%"> <br/>

We have analysed if there is the possibility to charge the device using this turbine and it is feasible: 
1. Esp32 in the active mode consume 240 mA and it is powered at 5 V; so, because in the prototype the duty cicle is not implemented, it consumes 1,2 W.
2. The Water Flow Sensor consume 10 mA (max) and it is powered at 5 V (because it is connected with the Esp32); so it consumes 0,05 W.
3. The Micro Water Turbine Hydro Generator works at 12 V, the power is 10 W and the intensity is 1,2 A; beacuse the diameter is lower than the diameter of the Water Flow Sensor, we do a proportion: if the sensor works with max 30 L/min (diameter=15,24mm), the generator work with max 25,6 L/min (diameter=13mm). 
The generator ideally can charge the device and the sensor beacuse it gives 10 W and they need 1,25 W. Other analysis should be done to know the daily water flow and relative energy produced.


## Response Time Performances

The response time between CHIEF, FORKs and BRANCHEs depends on the distance between the MCUs, so it depends on the topology of the irrigation system. An idea of the time taken by the architecture is shown in the previous chapter about the setting of the water leak detection threshold.
