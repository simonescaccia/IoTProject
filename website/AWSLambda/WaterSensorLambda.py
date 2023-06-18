import json

# import the AWS SDK
import boto3
import calendar
from datetime import datetime
import time

# Create a DynamoDB object using the AWS SDK
dynamodb = boto3.resource('dynamodb')

# Use the DynamoDB object to select our table
table_flow = dynamodb.Table('WaterFlowSourceTable')
table_leakage = dynamodb.Table('WaterLeakageTable')

# Define the handler function that the Lambda service will use
def lambda_handler(event, context):
    
    # Generate a timestamp for the event
    time_now = datetime.now().strftime('%d/%m/%Y %H:%M:%S')

    current_GMT = time.gmtime()
    timestamp = calendar.timegm(current_GMT)
    
    if (event['Id']=='flow') :
        # Write payload and time to the DynamoDB table using the object we instantiated and save response in a variable
        response = table_flow.put_item(
            Item={
                'Timestamp': timestamp,
                'Datetime': event['Datetime'],
                'Flow': event['Flow']
                })
        #response = table_flow.scan()
        #output = response['Items']
    
        #print(response['Items'])
        
        return {
           'statusCode': 200
        }        
                
    else:
        # Write payload and time to the DynamoDB table using the object we instantiated and save response in a variable
        response = table_leakage.put_item(
            Item={
                'Timestamp': timestamp,
                'Datetime': event["Datetime"],
                'Child': event["Child"],
                'Father': event["Father"],
                'Leakage': event["Leakage"]
                })
            
        #response = table_leakage.scan()
        #output = response['Items']
    
        #print(response['Items'])
        
        return {
           'statusCode': 200
        }
