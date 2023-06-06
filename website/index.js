// Configuration of the API endpoint 
const config = {
   // AWS API Gateway URL to be inserted here
    api_endpoint: 'https://62mgg7in5j.execute-api.us-east-1.amazonaws.com/dev' 
  };

  var values_x = [];
  var columns_colors = []; 
  var colors = ["blue", "red", "black"]
  var current_color = 0;
  var values_y = [];
  var ch1;
  var ch2;
  var values_y_flow = [];
  var columns_colors_flow = [];
  var input1 = [];
  var input2 = [];
  
  function displayDataOnChart(data_input){

    console.log(data_input);
    // Extracting body section of the response
    const body = JSON.parse(data_input).body
    // Parsing body content
    const parsed_data = JSON.parse(body);

    console.log(parsed_data);

    for (let i = 0; i < parsed_data.length; i++) {
      var object = parsed_data[i];
      for (var property in object) {
        console.log('item ' + i + ': ' + property + '=' + object[property]);
      }

      columns_colors.push(colors[1]);
      const datetime = new Date(parsed_data[i]["Datetime"]);
      // Convert sample time to Locale format
      var label = datetime.toLocaleString(); 
      
      // Add label to values_x array
      values_x.push(label); 

      label = parsed_data[i]["Flow"];
      values_y.push(label);
  
      //ch1.data.datasets[0].data.push(parsed_data[i]["Flow"]);
      //ch1.update();
      
      //ch2.data.datasets[0].data.push(parsed_data[i]["Flow"]);
      //ch2.update();
      
      //input1.push([parsed_data[i]["Datetime"], parsed_data[i]["Flow"]]);
      //input2.push([parsed_data[i]["Datetime"], parsed_data[i]["Flow"]]); 
    }

    console.log(values_x);
    console.log(values_y);

  }
  
     
  function plot(id , val_y) {

    var chartData = {
      labels: values_x,
      datasets: [{
        backgroundColor: columns_colors,
        data: val_y
      }]
    };
  
    if (id === "flowchart") {

      chart = new Chart(id, {
        type: "bar",
        data: chartData,
        options: {
          legend: { display: false },
          title: { display: true },
          scales: {
            yAxes: [{
                ticks: { beginAtZero: true }
            }]
          }
        }
      });

      return chart;
    }
    
    else if (id === "leakagechart") {

      var chartDataFlow = {
        labels: values_x,
        datasets: [{
          // Use a different color
          backgroundColor: colors[0],
          data: val_y
        }]
      };

      chart = new Chart(id, {
        type: "bar",
        data: chartDataFlow,
        options: {
          legend: { display: false },
          title: { display: true },
          scales: {
            yAxes: [{
              ticks: { beginAtZero: true }
            }]
          }
        }
      });
      return chart;
    }
  }
  
  
  function callAPI(base,exponent){
    // Instantiate a headers object
    var headers = new Headers();
   
    // Set the options of the request
    var requestOptions = {
        method: 'GET',
        headers: headers 
    };
    
    // Fetch content from API 
    fetch(config.api_endpoint, requestOptions)
    .then(response => response.text()).then(result => displayDataOnChart(result))
    
  }

 
  function display(){   
    
    // Plot the two charts
    flowchart = plot("flowchart", values_y);
    leakagechart = plot("leakagechart", values_y_flow);

    // Call the API to retrieve values 
    callAPI();

  }