// Configuration of the API endpoint 
const config = {
   // AWS API Gateway URL to be inserted here
    api_url: 'https://ll9tfou6n5.execute-api.us-east-1.amazonaws.com/dev' 
  };

/* Global Data */

 // Data for flow chart
 var x_values_flow = [];
 var y_values_flow = [];
 // Data for leakage chart
 var x_values_leakage = [];
 var y_values_leakage = [];

/* Statistics */

// Average measured source water flow
var avg_measured_flow = 0;
// Average daily flow
var avg_daily_flow = 0;


// Average detected leakage
var avg_leakage = 0;

function dataLoading(parsed_data){

  // Extracting body_flow section of the response
  const flow_data = parsed_data.body_flow;

  // Extracting body_leakage section of the response
  const leakage_data = parsed_data.body_leakage;

  // Parsing contents
  const parsed_flow_data = JSON.parse(flow_data);
  const parsed_leakage_data = JSON.parse(leakage_data);

  const n = Math.min(parsed_flow_data.length, 10);

  // Flow data processing
  for (let i = 0; i < n; i++) {

    const datetime = parsed_flow_data[i]["Datetime"];
    // Convert sample time to Locale format
    var value = datetime.toLocaleString(); 
    
    // Add value to values_x array
    x_values_flow.push(value); 

    value = parsed_flow_data[i]["Flow"];
    y_values_flow.push(value);

  }

  var processed_children = [];

  // Leakage data processing
  for (let i = 0; i < parsed_leakage_data.length; i++) {
    const child = parsed_leakage_data[i]["Child"];

    if (!(processed_children.includes(child))) {
      
      processed_children.push(child);

      const father = parsed_leakage_data[i]["Father"]; 

      const datetime = new Date(parsed_leakage_data[i]["Datetime"]);
      // Convert sample time to Locale format
      var date = datetime.toLocaleString(); 

      var today = new Date();
      today = today.toLocaleString().substring(0,10);
      var yesterday = new Date(Date.now() - 86400000);
      yesterday = yesterday.toLocaleString().substring(0,10);

      if (date.substring(0,10) != today && date.substring(0,10) != yesterday) {
        continue;
      }
      else {
        // Father-child pair generation
        value = ("Pair: ").concat(father.slice(-2)).concat("-").concat(child.slice(-2)).concat(" Time: ").concat(date);
        
        // Add value to values_x array
        x_values_leakage.push(value); 
    
        value = parsed_leakage_data[i]["Leakage"];
        y_values_leakage.push(value);

      }
    }
  }
}

/* Source water flow statistics */
async function getFlowStatistics() {

  // Reset statistics
  avg_measured_flow = 0;
  avg_daily_flow = 0;

  const now = new Date();

  const day = ("0" + now.getDate()).slice(-2);
  const month = ("0" + (now.getMonth() + 1)).slice(-2);
  const year = now.getFullYear();

  let today = day + '/' + month + '/' + year;

  var avg_dict = {};

  var total_flow = 0;

  for (let i = 0; i < y_values_flow.length; i++) {

    total_flow += parseFloat(y_values_flow[i]);
    var current_datetime = (x_values_flow[i].toLocaleString()).substring(0,10);

    if (today != current_datetime) {
      if (typeof avg_dict[current_datetime] === "undefined") {
        avg_dict[current_datetime] = [parseFloat(y_values_flow[i])];
      }
      else {
        avg_dict[current_datetime].push(parseFloat(y_values_flow[i]));
      }
    }

  }

  avg_measured_flow = parseFloat(total_flow / y_values_flow.length);

  var avgs = [];

  for (var date in avg_dict) {
    var values = avg_dict[date];
    var sum = 0;
    for (let i = 0; i < values.length; i++) {
      sum += parseFloat(values[i]);
    }
    var avg = parseFloat(sum/values.length);
    avgs.push(avg);
  }


  sum = 0;
  for (let i = 0; i < avgs.length; i++) {
    sum += parseFloat(avgs[i]);
  }

  avg_daily_flow = parseFloat(sum/avgs.length);

}


 /* Water leakage statistics */
async function getLeakageStatistics() {

  // Reset statistics 
  var total_leakage = 0;

  for (let i = 0; i < y_values_leakage.length; i++) {

    total_leakage += parseFloat(y_values_leakage[i]);

  }

  avg_leakage = parseFloat(total_leakage/y_values_leakage.length);

}

    
function plot(id, x_values, y_values) {
  var color = "black";

  if (id === "flowchart") {
    color = "blue";
  }
  else if (id === "leakagechart") {
    color = "red";
  }

  const ctx = document.getElementById(id).getContext('2d');
  new Chart(ctx, {
    type: "bar",
    data: {
      labels: x_values,
      datasets: [{
        backgroundColor: color,
        data: y_values
      }]
    },
    options: {
      legend: {display: false},
      title: {display: false},
      scales: {
        yAxes: [{
            ticks: { beginAtZero: true }
        }]
      }
    }
  });
}


async function display() {
  await callAPI()

  plot("flowchart", x_values_flow, y_values_flow);
  plot("leakagechart", x_values_leakage, y_values_leakage);

  getFlowStatistics();
  document.getElementById("avg-measured-flow").innerHTML =  avg_measured_flow;
  document.getElementById("avg-daily-flow").innerHTML = avg_daily_flow;

  getLeakageStatistics();
  document.getElementById("avg-leakage").innerHTML = avg_leakage;
  if (!Array.isArray(y_values_leakage) || !y_values_leakage.length) {
    document.getElementById("leakage-info").innerHTML = "No recent leakage detected";
  }
  else {
    document.getElementById("leakage-info").innerHTML = "ALERT: recent leakage detected!";
  }

}


async function callAPI() {

  // Instantiate a headers object
  var headers = new Headers();
  
  // Set the options of the request
  var requestOptions = {
      method: 'GET',
      headers: headers 
  };
  
  // Fetch content from API 
  const response = await fetch(config.api_url, requestOptions);
  const parsed_data = await response.json();
  dataLoading(parsed_data);

}
