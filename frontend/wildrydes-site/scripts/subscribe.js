window.onload = function () {
  AWS.config.region = 'us-west-2';
  AWS.config.credentials = new AWS.CognitoIdentityCredentials({
    IdentityPoolId: 'us-west-2:49d54372-d7a2-4ebb-a90e-692a29fad464',
  });

  var dynamodb = new AWS.DynamoDB({});

  var emailForm = document.querySelector('form');
  var emailInput = document.querySelector('form input');
  var thankYouDiv = document.querySelector('div#thank-you');
  var formDiv = document.querySelector('div#form');

  emailForm.addEventListener('submit', function (event) {
    event.preventDefault();

    params = {
      TableName: 'Wildrydes_Emails',
      Item: {
        Email: {
          S: emailInput.value
        }
      }
    };

    dynamodb.putItem(params, function (e) {
      formDiv.style.display = 'none';
      thankYouDiv.style.display = 'block';
    });
  });
};
