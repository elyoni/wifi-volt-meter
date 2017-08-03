var messageField = document.getElementById('message');
webSocketConnect("192.168.1.183");
//------------- Graph ------------------------
var line1 = [0];
var line2 = [0];
var labels = [0];
var counter = 0;
var updateChartChooser = 0;
var chartMaxLength = 1000;
var sampleBuffer = [1.1];
var sampleBufferSize = 50;
var sampleCounter = 0;
for (i = 0 ; i < chartMaxLength ; i++){
  // Set the lables 
  if (i % 25 == 0){
    labels[i] = i;
  }else{
    labels[i] = null;
  }
}

var chart = c3.generate({
  data: {
    columns: [
      ['data1'],
      ['data2'],
    ]
  },
  color:{ pattern:['#000000','#000000'], },
  point: { show: false, },
  subchart: { show: false, },
  zoom: { enabled: true, },
  transition: { duration: null, },
  legend: { show: false, },
  axis: { x: { tick: { values: labels, }, max: chartMaxLength, }, },
});
function updateChart(newValue){
  for (i = 0 ; i < sampleBufferSize ; i++){ 
    if (updateChartChooser == 0){
      line1[counter] = newValue[i];//Math.random() * 10;
      line2[counter] = null;
    }else{
      line1[counter] = null;
      line2[counter] = newValue[i];//Math.random() * 10;
    }
    counter += 1;
    if (counter == chartMaxLength) {
      counter = 0;
      updateChartChooser = (updateChartChooser+1) % 2;
    }
  }
  chart.load({
    columns: [
      ['data1'].concat(line1),
      ['data2'].concat(line2),
    ],
  });
}  

setInterval(function() {
  var newData = []
  for (i=0 ; i < sampleBufferSize+10; i++){
   sampleBufferHandler(parseFloat(Math.random() * 10));
  }
}, 100);

function writeToScreen(message){
  messageField.innerHTML = message;
}
setTimeout(function() {
  writeToScreen("yoni");
}, 1000);

function webSocketConnect(url){
  var socket = new WebSocket('ws://' + url + ':81');
  //var socket = new WebSocket('ws://192.168.43.150:81');
  socket.onopen     = function(evt){ onOpen(evt); };
  socket.onclose    = function(evt){ onClose(evt); };
  //socket.onmessage  = function(evt){ onMessage(evt); };
  socket.onerror    = function(evt){ onError(evt); };
}

function onOpen(evt){
  writeToScreen("CONNECTED");
}

function onClose(evt){
  writeToScreen("DISCONNECTED");
}

function onMessage(evt){
  //writeToScreen(evt.data);
  updateChart(parseFloat(evt.data));
  //websocket.close();
}

function onError(evt){
  writeToScreen(evt.data);
  //writeToScreen('<span style="color: red;">ERROR:</span> ' + evt.data);
}

function addToBuffer(newSample){
  sampleBuffer.push(newSample);
  sampleCounter++;
}
function sampleBufferHandler(newSample){
  if (sampleCounter < (sampleBufferSize-1)){
    addToBuffer(newSample);  
  }else{ // sampleCounter == sampleBufferHandler-1
    updateChart(sampleBuffer);  // Print the chart
    sampleBuffer = [];          // Clear the buffer
    sampleCounter = 0;          // Reset The buffer counter
    addToBuffer(newSample);     // Add the new sample 
  }
}
//connectToWebSocket.onclick = function(c);
