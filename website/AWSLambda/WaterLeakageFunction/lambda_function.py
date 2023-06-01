import json

# import the AWS SDK
import boto3

# Create a DynamoDB object using the AWS SDK
dynamodb = boto3.resource('dynamodb')

# Use the DynamoDB object to select our table
table = dynamodb.Table('WaterLeakageTable')


# Define the handler function that the Lambda service will use
def lambda_handler(event, context):
    
    # Write payload and time to the DynamoDB table using the object we instantiated and save response in a variable
    response = table.put_item(
        Item={
            'Child': event['child'],
            'Father': event['father'],
            'Leak': event['leak']
            })
    
    response = table.scan()
    output = response['Items']
    
    return {
       'statusCode': 200,
       'body': json.dumps(output)
    }
