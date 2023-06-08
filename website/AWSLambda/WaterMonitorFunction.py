import json

# import the AWS SDK
import boto3

# Define the handler function that the Lambda service will use
def lambda_handler(event, context):

    # Create a DynamoDB object using the AWS SDK
    dynamodb = boto3.resource('dynamodb')

    # Use the DynamoDB object to select our table
    table_leakage = dynamodb.Table('WaterLeakageTable')
    table_flow = dynamodb.Table('WaterFlowSourceTable')

    response_leakage = table_leakage.scan()
    response_flow = table_flow.scan()

    output_leakage = response_leakage['Items']
    output_flow = response_flow['Items']

    return {
       'statusCode': 200,
       'body_leakage': json.dumps(output_leakage),
       'body_flow': json.dumps(output_flow)
    }
