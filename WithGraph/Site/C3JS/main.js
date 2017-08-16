var messageField = document.getElementById('message');
var debug = document.getElementById('debug');
webSocketConnect("192.168.1.183");
//------------- Graph ------------------------
var line1 = [];
var line2 = [];
var labels = [0];
var counter = 0;
var updateChartChooser = 0;
var chartMaxLength = 1000;
var sampleBuffer = [];
var sampleMiniBuffer;
var sampleBufferSize = 500;
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
function updateChart(arrToNewSamples){
  for (var i = 0 ; i < sampleBufferSize ; i++){ 
    if (updateChartChooser == 0){
      line1[counter] = arrToNewSamples[i];//Math.random() * 10;
      line2[counter] = null;
    }else{
      line1[counter] = null;
      line2[counter] = arrToNewSamples[i];//Math.random() * 10;
    }
    counter++;
    if (counter == chartMaxLength-1) {
      counter = 0;
      //updateChartChooser = (updateChartChooser+1) % 2;
    }
  } 
    chart.load({
    columns: [
      ['data1'].concat(line1),
      ['data2'].concat(line2),
    ],
  });
  //console.log("line1: " + line1);
  //console.log("line2: " + line2);
}  
  /*
setInterval(function() {
  var newData = []
  for (i=0 ; i < sampleBufferSize+10; i++){
   sampleBufferHandler(parseFloat(Math.random() * 10));
  }
}, 1);
setTimeout(function() {
  //Run one time
  writeToScreen("yoni");
}, 1000);
*/

function writeToScreen(message){
  messageField.innerHTML = message;
}
function writeToDebug(message){
  //  debug.innerHTML += parseCharArr(message);
  debug.innerHTML += message;
  //message.charCodeAt(0) + " " + message + "<br />";
}

function webSocketConnect(url){
  var socket = new WebSocket('ws://' + url + ':81');
  //var socket = new WebSocket('ws://192.168.43.150:81');
  socket.onopen     = function(evt){ onOpen(evt); };
  socket.onclose    = function(evt){ onClose(evt); };
  socket.onmessage  = function(evt){ onMessage(evt); };
  socket.onerror    = function(evt){ onError(evt); };
}

function onOpen(evt){
  writeToScreen("Connect To ESP8266 vi Websocket");
}

function onClose(evt){
  writeToScreen("DISCONNECTED");
}

function onMessage(evt){
  msg = evt.data; 
  if (msg == "Connected"){
    writeToScreen(msg);
  }else{
    //writeToDebug(msg);
    samplesHandler(msg);
  }
}

function onError(evt){
  writeToScreen(evt.data);
  //writeToScreen('<span style="color: red;">ERROR:</span> ' + evt.data);
}

function parseCharArr(arr){
  var number = 0; 
  for(i = 0 ; i < arr.length ; i++){
    //console.log(String.fromCharCode(arr.charCodeAt(i)-1));
    number += ((Math.pow(127,i))*(arr.charCodeAt(i)-1));
  }
  return number;
}

function addToBuffer(newSample){
  sampleBuffer.push(newSample);
  sampleCounter++;
}
function sampleBufferHandler(newSample){ 
  if (sampleCounter < (sampleBufferSize-1)){
    addToBuffer(newSample);
  }else{
    addToBuffer(newSample);
    //console.log(sampleBuffer);
    updateChart(sampleBuffer);  // Print the chart
    sampleBuffer = [];
    sampleCounter = 0;
  }
  
  
  
  
  
  /*if (sampleCounter < (sampleBufferSize-1)){
    addToBuffer(newSample);  
  }else if(sampleCounter == (sampleBufferSize-2)){
    sampleMiniBuffer = newSample;
    sampleCounter++;
  }else{ // sampleCounter == sampleBufferHandler-1
    updateChart(sampleBuffer);  // Print the chart
    console.log(sampleBuffer);
    //    sampleBuffer = [sampleMiniBuffer];//tempList;          // Clear the buffer
    //    sampleBuffer.push(newSample);
    sampleCounter = 0;          // Reset The buffer counter
    //addToBuffer(newSample);     // Add the new sample 
  }*/
}
function splitSamples(samples){
  //return samples.split(String.fromCharCode(127));
  var sampleArr =  samples.split(",");
  sampleArr.pop();
  return sampleArr;
}

function samplesHandler(sampels){
  var samplesArr = splitSamples(sampels);
  //writeToDebug(samplesArr);
  //  console.log(samplesArr);
  for(var i = 0 ; i < samplesArr.length ; i++){
    sampleBufferHandler(parseInt(samplesArr[i]));
    //sampleBufferHandler(samplesArr[i]);
  }
}
