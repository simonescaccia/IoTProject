import json

# import the AWS SDK
import boto3

# import calendar
# from datetime import datetime
# import time

from decimal import Decimal

# Funtion to sort by numeric timestamp
def sort_by_key(list):
    return list['Timestamp']

# Define the handler function that the Lambda service will use
def lambda_handler(event, context):

    # Create a DynamoDB object using the AWS SDK
    dynamodb = boto3.resource('dynamodb')

    # Use the DynamoDB object to select our tables
    table_leakage = dynamodb.Table('WaterLeakageTable')
    table_flow = dynamodb.Table('WaterFlowSourceTable')

    """
    # Generate a timestamp for the event
    time_now = datetime.now().strftime('%d/%m/%Y %H:%M:%S')

    current_GMT = time.gmtime()
    timestamp = calendar.timegm(current_GMT)
  
    # Write payload and time to the DynamoDB table using the object we instantiated and save response in a variable
    response = table_flow.put_item(
        Item={
            'Timestamp': timestamp,
            'Datetime': time_now,
            'Flow': "9"
            })
    
    # Write payload and time to the DynamoDB table using the object we instantiated and save response in a variable
    response = table_leakage.put_item(
        Item={
            'Timestamp': timestamp,
            'Datetime': time_now,
            'Child': "st-lrwan1-11",
            'Father': "st-lrwan1-12",
            'Leakage': "6"
            })
    """
            
    # Retrieve tuples of our tables to return
    response_leakage = table_leakage.scan()
    response_flow = table_flow.scan()

    output_leakage = response_leakage['Items']
    output_flow = response_flow['Items']
    
    # Convert numeric values to strings
    for flow_json in output_flow:
        flow_json['Timestamp'] = str(flow_json['Timestamp'])
        
    for leakage_json in output_leakage:
        leakage_json['Timestamp'] = str(leakage_json['Timestamp'])
    
    # Sort by timestamp
    output_flow = sorted(output_flow, key=sort_by_key);
    output_leakage = sorted(output_leakage, key=sort_by_key)

    
    return {
       'statusCode': 200,
       'body_flow': json.dumps(output_flow),
       'body_leakage': json.dumps(output_leakage)
    }
    
