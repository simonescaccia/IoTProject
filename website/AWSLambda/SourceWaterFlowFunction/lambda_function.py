import json

# import the AWS SDK
import boto3

from time import gmtime, strftime

# Create a DynamoDB object using the AWS SDK
dynamodb = boto3.resource('dynamodb')

# Store current time in a human-readable format 
now = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())

# Use the DynamoDB object to select our table
table = dynamodb.Table('SourceWaterFlowTable')


# Define the handler function that the Lambda service will use
def lambda_handler(event, context):
    
    # Write payload and time to the DynamoDB table using the object we instantiated and save response in a variable
    response = table.put_item(
        Item={
            'Datetime': now,
            'Flow': str(event['flow'])
            })
            
    response = table.scan()
    output = response['Items']

    print(response['Items'])
    
    return {
       'statusCode': 200,
       'body': json.dumps(output)
    }
