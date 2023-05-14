# Irrigation Water Leakage System RIOT code

## IoT-Lab experiment connection
Create a new IoT-Lab experiment with the needed number of nodes for the simulation. In our case, we defined an infrastructure with 8 nodes, one for each ESP32. Then, after properly setting an SSH key on IoT-Lab, connect through the following command:

    ssh <username>@saclay.iot-lab.info
    
Default 4.9 version of ARM gcc toolchain is not compatible, so run:
    
    source /opt/riot.source 
    
In order to check current gcc version the following command is available:
    
    arm-none-eabi-gcc --version
 
In our case, we refer to 2023.04 RIOT-OS version.

Clone RIOT-OS repository with:

    git clone https://github.com/RIOT-OS/RIOT.git -b 2023.04 branch

Then clone our repository:

    git clone https://github.com/simonescaccia/Irrigation-Water-Leakage-System.git

At this point compile the code with:

    <login>@saclay:~/Irrigation-Water-Leakage-System/code$ make clean all

To flash the firmware:

    <login>@saclay:~/Irrigation-Water-Leakage-System/code$ iotlab-node --flash bin/b-l072z-lrwan1/irrigation-water-leakage-detection.elf -l saclay,st-lrwan1,<node-id> -i <experiment-id>
      
The values <node-id> and <experiment-id> can be easily retrieved from IoT-Lab dashboard.
    
Now it is possible to access to the RIOT shell running on the node using netcat:
    
    nc st-lrwan1-<node-id> 20000
    
If the console is empty, using:
    
    setup
    
the output will print out.
