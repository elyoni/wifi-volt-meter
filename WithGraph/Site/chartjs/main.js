var ctx = document.getElementById('myChart').getContext('2d');
var inTxtConnectToWebSocket = document.getElementById('inTxtConnectToWebSocket');
var zoomInOut = document.getElementById('zoomInOut');
var counter = 0
var data01 = [];
var data02 = [];
var labels = [];
var updateChartChooser = 0;
var chartMaxLength = 100;
for (i = 0 ; i < chartMaxLength ; i++){
  if (i % 5 == 0){
    labels[i] = i;
  }else{
    labels[i] = "";
  }
}
// Websocket functions
  /*
function webSocketConnect(url){
  var socket = new WebSocket('ws://' + url + ':81');
  //var socket = new WebSocket('ws://92.168.43.150:81');
  socket.onopen     = fucntion(evt){ onOpen(evt) };
  socket.onclose    = fucntion(evt){ onClose(evt) };
  socket.onmessage  = fucntion(evt){ onMessage(evt) };
  socket.onerror    = fucntion(evt){ onError(evt) };
}

connectToWebSocket.onclick = function(c)
*/
var myChart = new Chart(ctx, {
  type: 'line',
  data:{
    datasets: [{
      data: data01,
      lineTension: 0,
      fill: false,
      spanGaps: true,
      steppedLine: false,
      borderColor: 'rgb(0, 0, 0)',
      pointRadius: 0,
      borderWidth: 1.5,
      label :{  
        display: false,
      },
    },{
      data: data02,
      lineTension: 0,
      fill: false,
      spanGaps: true,
      steppedLine: false,
      borderColor: 'rgb(0, 0, 0)',
      pointRadius: 0,
      borderWidth: 1.5,
    }] ,
    labels: labels,
  },
  options:{
    animation:false,
    legend: {
      display: false
    },
    pan: {
        // Boolean to enable panning
        enabled: true,

        // Panning directions. Remove the appropriate direction to disable 
        // Eg. 'y' would only allow panning in the y direction
        mode: 'xy',
    },

    // Container for zoom options
    zoom: {
        // Boolean to enable zooming
        enabled: true,

        // Zooming directions. Remove the appropriate direction to disable 
        // Eg. 'y' would only allow zooming in the y direction
        mode: 'yx',
    },

  }
});
zoomInOut.onclick = function(){
  for (i = 0 ; i < 10 ; i++){
      labels.remove = lavels[i] ;
  }
  for (i = 10 ; i < chartMaxLength-10 ; i++){
    if (i % 5 == 0){
      labels[i] = i;
    }else{
      labels[i] = "";
    }
  }
  for (i = chartMaxLength-10 ; i < chartMaxLength ; i++){
      labels[i] = null;
  }
};
function updateChart(){
  //myChart.data.datasets[0].data.push(Math.random() * 10);
  if (updateChartChooser == 0){
    myChart.data.datasets[0].data[counter] = Math.random() * 10;
    myChart.data.datasets[1].data[counter] = null;
  }else{
    myChart.data.datasets[1].data[counter] = Math.random() * 10;
    myChart.data.datasets[0].data[counter] = null;
  }
  //myChart.data.labels.push(counter);
  counter += 1;
  if (counter == chartMaxLength) {
    counter = 0;
    updateChartChooser = (updateChartChooser+1) % 2;
  }
  myChart.update();
}  

setInterval(function() {
  updateChart();
}, 40);

