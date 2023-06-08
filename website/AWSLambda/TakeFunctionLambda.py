 
import json

# import the AWS SDK
import boto3

# Create a DynamoDB object using the AWS SDK
dynamodb = boto3.resource('dynamodb')

# Use the DynamoDB object to select our table
table_flow = dynamodb.Table('WaterFlowSourceTable')
table_leakage = dynamodb.Table('WaterLeakageTable')

# Define the handler function that the Lambda service will use
def lambda_handler(event, context):
    message = event['uplink_message']
    print(message)
    print()
    payload = message['decoded_payload']
    print(payload)
    print()
    test = payload["myTestValue"]
    print(test)
    print()
    print(test['Id'])

    if (test['Id']=='flow') :
        # Write payload and time to the DynamoDB table using the object we instantiated and save response in a variable
        response = table_flow.put_item(
            Item={
                'Datetime': test['Datetime'],
                'Flow': str(event['Flow'])
                })
        response = table_flow.scan()
        output = response['Items']

        print(response['Items'])

        return {
           'statusCode': 200,
           'body': json.dumps(output)
        }

    else:
        # Write payload and time to the DynamoDB table using the object we instantiated and save response in a variable
        response = table_leakage.put_item(
            Item={
                'Datetime': test["Datetime"],
                'Child': test["Child"],
                'Father': test["Father"],
                'Leakage': test["Leakage"]
                })

        response = table_leakage.scan()
        output = response['Items']

        print(response['Items'])

        return {
           'statusCode': 200,
           'body': json.dumps(output)

        }
