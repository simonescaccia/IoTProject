# Evaluation

## Evaluation methodology: Overall System

### During the development of the project

As a general performance evaluation of the system, we wanted to detect if a water leak occurs within 24 hours, in order to detect water leakages without tolerating more than one day of water losses. We wanted also to know wich pipe is leaking.

We used the following metrics to evaluate the performance of the system:

1. Leak is dectected within 24 hours
2. The pipe is correctly identified

### After system completion

We want to detect if a water leak occurs within 24 hours, in order to detect water leakages without tolerating more than one day of water losses. We want also to know wich pipe is leaking, providing information about the quantity of water leakage in liters per minute. Finally, we also manage to evaluate the sensor precision error in order to specify a proper leakage threshold.

1. Leak is dectected within 24 hours
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

### Water leak detection Algorithm Problems
Before designing a water leak detection algorithm, we focused on the dynamics of water in pipes. Since we are dealing with an irrigation system, a reasonable assumption is to consider **water in pipes under pressure**. It means that there are never empty pipes. Now, we can have two conditions, either stationary water or moving water, due to the increasing pressure at the source during irrigation time. This means that we can ignore the specific condition when doing a test, because the difference between adjacent flows will be bounded in both cases if there is no leakage, and not bounded otherwise. The detection algorithm starts from the source and propagates through the tree topology, always with the same logic for every father-child pair. The father sends water flow information to the child, which makes a proper computation and compares the obtained value with the received one, using a specific threshold. In case of leakage detection, it will communicate the leakage to the cloud through the LoRa gateway.

One of the main problems related to the test is related to synchronization, because of the delay related to transmission latency. We thought about an handshaking approach to take into account this discrepancy and to compare water flows starting the sampling process at two close moments.

1. Test propagation phase
2. Leak notification

The complexity will change depending on the number of FORKs, and we will compare variuous topologies in order to find the best one.

The three algorithms proposed in the design phase have been tested. The test of the Ack algorithm has shown that there is a problem in the code (caused by the use of threads in the program) and that the water flow before our system (that we consider always present) is not stable and this issue has to be solved by the synchronization algorithm. 
The problem of stability of the initial water flow can be seen in the following image in which there is a segmentation of the lines of the graph.<br/>
![ack](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/ack_flow.png) <br/>
About the Ack algorithm, it is possible to notice that it is good to detect a leakage but it says that the water flow of the Son is always higher than the water flow of the Source, and this can lead to error, so it is not the worst algorithm implemented.<br/>
![ack1](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/ack_test1.png)<br/>
![ack3](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/ack_test3.png)<br/>

About the Hanshake algorithm, it is perfect to detect the leakage and the trend of the Son higher than the Source is disappeared. There still is an error of the turbines, that we will face in the next chapter.<br/>
![h1](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/handshake_test1.png)<br/>
![h2](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/handshake_test2.png)<br/>

In the end, about the syncAck algorithm, it is possible to see the same trend of the handshake algorithm but with one less message. There is also here the error of the turbines.<br/><br/>
![s1](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/syncAck_test1.png)<br/>
![s2](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/syncAck_test2.png)<br/>

### Water leak detection Algorithm Solutions
After several tests, we have found that one water flow sensor has some problems and detect less impulses.

### Water leak detection Threshold
We have done some analysis to set a correct threshold in our algorithms to limit the presence of false positives and false negatives. We have also thought that a false negative is more serious than a false positive, and so our system will be prone to false positives.<br/>
We have chosen the syncAck as the final algorithm because it uses one message less.
We have analysed the time of the SyncAck algorithm to understand the error that can be created and to use this values for the calucus of the energy consumption.
We have analysed that the standard deviation is high and this means that there is a large distribution of the data, in particular this means that the time taken by the messages and the code is fluctuating. Looking at the scheme, it is possible to understand the time of the messages and also the quality of the synchronization of the algorithms.<br/>
![s](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/images/syncAck_time1.png)<br/>
Here there are 0,462(message) + 0,374(difference) in the Son between the end of the test and the arrival of the value of the Source, knowing that in average the message takes 0,462s from the sender to the receiver, this means that the Source has ended the test 0,090s before the Son. We double it (also for the difference in starting time) and we obtain 0,180s. 

<br/>**Difference**<br/>
To find the error of the two algorithm we have to decide the time of testing: now is 3s but if we increase it, the influence of the error derived by the synchronization problem is less.
If we put the time of the test at 10s, we have that 0,180 : 10 = x : 100. So, influence of syncAck is y = 1,8% .
Looking at the number of impulses per minute in the datasheet, that is 541 impulses/min, we can find the error of impulses derived by the percentage.<br/>
541imp / 60s and 30L / 60s -> 30L / 541imp <br/>
This error represents the possibility of a changing in the water flow rate before the Source<br/>
y = 1,80% -> imp/10s = 1,80% * 90 = 1,62imp -> flow = L/min = 1,62 * 30L / 541imp = 0,09 L/min <br/>

Beacuse the instrumental error is higher, as we will see, the thing that is significant is the number of messages and so the syncACK will be the final algorithm of our application.<br/>

*It is possible to improve the algorithms using correctly a 'sleep' for some milliseconds; but, because the standard deviation is high, more data are needed to be accurate.*

### Turbine error
Another significant error of the architecture is the instrumental error of the water flow sensor. Because of we have not another turbine with the correct value of the water flow or other instruments, we have run the system for several tests and we have analysed the difference between the values of Source and Son. We have observed the difference because the water flow before our application is not costant (decided by the public pipeline). It is important to observ that this error is influenced by the algorithmic error too. We have taken in consideration also the handshake algorithm.
![h3AB](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/handshake_error_AB.png)
![h3BA](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/handshake_error_BA.png)
![s3AB](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/syncAck_error_AB.png)
![s3BA](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/syncAck_error_BA.png)<br/>
The system is proned to have the Son with an higher water flow value, if you put turbine A before turbine B, but this happens because the turbine A is less efficient. <br/>
For our prototype and with a distance of 90cm between the nodes, based on the consideration of the several approssimations, We have initially fought to put a threshold of 1 L/min, but then we have done more considerations.<br/>
We have first set a fixed scenario: 
* turbine B = Source 
* turbine A = Son
Then, we have done a lot of tests changing time of sampling and water flow rate. <br/>
With the analysis of the algorithmic error done before, we thought that an higher sampling was more accurate but the data has not shown this. The tests are done with the use of the syncAck, the table below shows the error rate for the different situation: <br/>

|flow x time| 3s | 5s | 9s |
|--- |--- |--- |--- |
| 0-30 L/min | 0.84 L/min | 0.49 L/min | 0.74 L/min |
| 0-10 L/min | 0.14 L/min | 0.29 L/min | 0.13 L/min |
| 10-20 L/min | 0.58 L/min | 0.67 L/min | 0.76 L/min |
| 20-30 L/min | 1.11 L/min | 0.62 L/min | 1.10 L/min |

The first line is not relevant because there are not enough data and are not distributed well.<br/>
Then, it is possible to note that the difference is not caused by the different time of sampling, but by the different water flow rate: this lead to say that the main error is the error of the turbines and not the error of the algorithm. <br/>
So, we can decide to use the syncAck algorithm knowing that it uses one message less, not keeping in mind the algorithmic error. <br/>
![](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/Comparison_error_syncAck_3s.png)
![](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/Comparison_error_syncAck_3s_1.png)
![](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/Comparison_error_syncAck_5s.png)
![](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/Comparison_error_syncAck_5s_1.png)
![](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/Comparison_error_syncAck_9s.png)
![](https://github.com/simonescaccia/Irrigation-Water-Leakage-System/blob/main/graph/Comparison_error_syncAck_9s_1.png)<br/>
We have analysed these data to fix a threshold for the detection of the leakages, and we have done the mean of (flowSource-flowSon)x100/flowSource %:
* 3s -> 4,29%
* 5s -> 6,34%
* 9s (done 9s and not 10s because of a problem of MQTT) -> 4,30% <br/>
We have decide to take 10s for the sampling and this will be similar to 9s result, so in this situation we can put a **fixed threshold of > 2L/min** or **dynamic threshold flowSource-flowSon > 5% flowSource L/min**. The dynamic threshold is more efficient because it is more accurate in the lower water flow rate.

### Energy consumption
#### Duty cycle
Our requirement is not to tolerate a water loss of more than one day, so we wish to detect a leakage within 24 h. Now, since the leakage is an unpredictable event, we cannot define a precise strategy apriori, but we want to indentify the best one in order to minimize power consumption. We compute this strategy analitically. Firstly, for simplicity, we focus on a simple father-child pair, since the same reasoning holds for every adjacent pair of nodes of the tree topology. Now, we define x as the send rate (msg/day) of the father, so the number of messages sent per day, and y as the total listen interval of the child (in hours/day). 


![dutycycle](./images/duty_cycle.png)


In order to be sure to correctly listen to at least one message in one day, y should be equal to (24/x + epsilon) hours/day, where epsilon is a neglectable time interval if compared with 24/x hours. Computing the energy consumption, there are three contributions, one related to the sender, one to the receiver and one to the exchange of messages linked to the leakage detection algorithm, so both for synchronization and test. This last component can be omitted in our considerations because it is not relevant for our estimation of x, which we wish to know. So, the energy consumption to minimize is given by:


$$E(x,y) = E(x) + E(y) + E(algorithm) \approx E(x) + E(y)$$

The two components are: 
$$E(x) = (P_{trans} \cdot t_{trans}) \cdot x + (P_{on} \cdot t_{on}) \cdot x +(P_{off} \cdot t_{off}) \cdot x$$

$$E(y) = P_{listen} \cdot y + P_{on} \cdot t_{on} + P_{off} \cdot t_{off}$$

Our analysis is hence focused on finding the integer value of x (number of messages per day of the father) such that the energy consumption is minimized. The listen time of the child will depends on x value. 

In order to estimate the value of x to minimize energy consumption, we analysed some graphs from IoT-Lab simulation.
First, we considered the transmission parameters of LoRa:

![trans_energy](./energy_consumption/10-Lora-TTN-sending-one-message.png)

The transmission time and power are:
$$P_{trans} = (0.455 - 0.270) W = 0.185 W$$ 
$$t_{trans} = (0.17 - 0.080) s = 0.090 s$$

The effective transmission effectively starts at 0.085 seconds, however, we considered all the interval time where some variation is revealed. In this way we also considered the contribution of the energy for switching on and off the transmission. Hence:

$$E(x) = (P_{trans} \cdot t_{trans}) \cdot x + (P_{on} \cdot t_{on}) \cdot x +(P_{off} \cdot t_{off}) \cdot x$$
$$E(x) \approx (P_{trans} \cdot t_{trans}) \cdot x = (0.185 W) \cdot (0.090 s) \cdot x = 0.00765 J \cdot x$$

The power consumption due to listening can be found analysing the listening process:

![listen_energy](./energy_consumption/4-Lora-p2p-listen.png)

So the average listening power is:

$$P_{listen} = 0.325 W$$

While the average sleeping power, useful for further considerations, is:

$$P_{sleep} = 0.28 W$$

Also, for this case, we do not consider the small constributions due to the a single on and a single off switching, so:

$$E(y) = P_{listen} \cdot y + P_{on} \cdot t_{on} + P_{off} \cdot t_{off}$$
$$E(y) \approx P_{listen} \cdot y $$

So, the final analysis is on the function:

$$E(x) + E(y) = 0.00765 \cdot x + (0.325 \cdot \frac{24}{x}) $$

So the function to minimize is:

$$f(x) = 0.00765 \cdot x + \frac{7.8}{x}$$

The minimum of the function, considering the domain of $$x \geq 1$$
because we have to send at least one message per day. Solving the function analytically, we found a global minimum for:

$$x = 10.0976 \approx 10$$

So the send rate of the father should be 10 messages/day, hence the listen time will be:

$$y = \frac{24}{x} = \frac{24}{10} = 2.4 \frac{hours}{day}$$

#### Estimation of system duration
To analyse the duration of our system, we must also consider other energy contributions, mainly the measurement, the sleep and the cloud transmission energy consumptions. We considered the daily worst case, so the case of an intermediate node that measures and sends data to child, 10 times in the worst case, listens for 2.4 hours, receives data from the father once and sends once to the cloud the difference with father water flow. In our analysis,

$$E_{cloud} \approx E_{trans}$$

The energy of measurement has been computed from the prototype, since water flow values are simply simulated and not measured in IoT-Lab simulation. Hence:

$$E_{measure} = P_{sensor} * t_{sampling} = (0.05W) \cdot (10 s) = 0.5 J$$

Daily energy consumption is computed as:

$$E_{tot} = (E_{measure} + E_{trans})\cdot 10 + E_{cloud} + P_{listen} \cdot 2.4h + P_{sleep} \cdot (24 - 2.4) h$$

$$E_{tot} = [(0.5 J) + (0.00765 J)] \cdot 10 + (0.00765 J) + (0.325 W) \cdot 2.4h + (0.28 W) \cdot 21.6h$$
$$E_{tot} \approx (0.001391014 Wh) + (0.78 Wh) + (6.048) Wh \approx 6.828 Wh $$

So, for a year:

$$E_{tot} = E_{tot} \cdot 365 = 2492.22 Wh$$

Here, we have used the power of the sleep of Iot-Lab and indeed the consumption is huge, but in the datasheet of the ESP32 there is the Hibernation Mode whose consumption is 
$$P_{sleep_esp32} $$ ~5µA \cdot 5V = 0.000025 W $$
$$E_{tot} \approx (0.001391014 Wh) + (0.78 Wh) + (0.000025 W \cdot 21.6 h) Wh \approx 0.78 Wh $$
So, for a year:

$$E_{tot} = E_{tot} \cdot 365 = 284 Wh$$

Also here the consumption is too huge, because the time of listening is too high. So we propose another approach.

#### Drift rate clock

Starting from the output of the chapter before, we use another approach doing a synchronization of the nodes between themselves. We have observed that for these environments, battery of 1000-2000 mAh are used.

The time of listening is reduced while the power of it remains the same, in particular we have to synchronize the nodes in pairs and for each node there will be a situation in which it will be the passive one (waiting in listening) and another situation in which it will be active (send the message for starting the test). We use hibernation mode and sot the drift rate is 1.7 sec per day, so if we do a listening of 20s (start 5s before the previous day), we have to double it because each node has to do two communications:

$$E_{tot} = (E_{measure} \cdot 10 )\cdot 2 + E_{cloud} + E_{trans1} + E_{trans2} + E_{trans3} + P_{listen} \cdot 40  + P_{sleecdot p_esp32} \cdot (24h - 40) h$$
$$E_{tot} = [(0.5 J) \cdot 10 ] \cdot 2 + 0.0306 J + (0.325 W) \cdot 0.0111111h \cdot + (0.000025 W) \cdot 24h \cdot 2$$
$$E_{tot} \approx (0.0028 Wh) + (0.0000084 Wh) + (0.00361 Wh) + (0.0006 Wh) \approx  0.007 Wh $$
So, for a year:

$$E_{tot} = E_{tot} \cdot 365 = 2.56 Wh$$

Now, with a battery of 1000 mAh (3.7 V) that gives us 3.7 Wh, we can power the device for at leat one year, that is our user requirement.


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
The generator ideally can charge the device and the sensor beacuse it gives 10 W and they need 1,25 W. Other analysis should be done to know the daily water flow and relative energy produced.<br/>

So the system can be changed creating an algorithm that wake up the node only when there is water flow and so when there is energy. In this way, there will be a relationship between the number of sampling and the water flow rate in a day. The energy obtained by the Micro Water Turbine Hydro Generator can be also stored in a battery to allow the node to wake up in other occasions (this is not our situation because we are interested only when there is water flow).
