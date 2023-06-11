# AWSLambda
This folder contains Python scripts used for AWS Lambda functions. 

In detail, `TakeFunctionLambda.py` processes messages sent from LoRa TTN node of our IoT-Lab simulation to our TTN application, and from this application to AWS. Then it inserts obtained information in two DynamoDB tables. 

The function `WaterMonitorFunction.py`, instead, returns information from the two tables, that are accessed by the web application through a proper AWS API Gateway. 

Finally, the function `WaterSensorLambda.py` is an slightly different Python script used for our prototype in order to retrieve data from tables.
