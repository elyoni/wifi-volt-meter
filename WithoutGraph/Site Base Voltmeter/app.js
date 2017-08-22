//setInterval(addVariable, 5000);

var numberOfDevices = document.getElementById('numberOfDevices');
var DevicesList = [];
var firstMSG = true;

function addToTable(numberOfAnalog,dmmNumber) {
	var table = document.getElementById("dmmRow");
	//var row = table.insertRow(table.rows.length);

	var cell = table.insertCell(table.length);

	for (var i=0 ; i < numberOfAnalog ; i++){
		var text = document.createTextNode("A" + i.toString());
		var x = document.createElement("INPUT");
		var br = document.createElement("BR");
		x.setAttribute('id',dmmNumber.toString() + "A" + i.toString());
		x.setAttribute('type',"text");
		x.setAttribute('value',"0");
		x.setAttribute('readOnly',true);
		console.log(dmmNumber.toString() + "A" + i.toString());
		cell.appendChild(text);
		cell.appendChild(x);
		cell.appendChild(br);
	}
}

function updateValues(msg) {
	// Update values
	var numberOfAnalog = parseInt(msg[1]);
	var id = msg[0] + 'A';
	msg = msg.slice(2); //Remove the first charactes
	var data = msg.split(",");
	for (var i=0 ; i<numberOfAnalog ; i++){
		changeValue(id + i.toString(),data[i]);
	}
}

function changeValue(id,value) {
	var element = document.getElementById(id);
	element.setAttribute('value',value);
}


function webSocketConnect(url){
	var socket = new WebSocket('ws://' + url + ':81');
	//var socket = new WebSocket('ws://192.168.43.150:81');
	socket.onopen     = function(event){ onOpen(event); };
	socket.onclose    = function(event){ onClose(event); };
	socket.onmessage  = function(event){ onMessage(event); };
	socket.onerror    = function(event){ onError(event); };
}

function onOpen(event){
	numberOfDevices.innerHTML = parseInt(numberOfDevices.innerHTML)+1;
	console.log("Device as been conneted");
	console.log(event.source);
	return false;
}

function onClose(event){
	//numberOfDevices.innerHTML = parseInt(numberOfDevices.innerHTML)-1;
	//console.log("Device as been Disconneted");
	return false;
}

function onMessage(event){
	var message = event.data;
	console.log("Got new message " + message);

	if (message[0] == '*'){
		//Got New Device Data
		DevicesList.push(message[1]);
		addToTable(message[2],message[1]);
	}else{
		updateValues(message);
	}
	return false;
}

function onError(event){
	console.log('WebSocket Error: ' + error);
	return false;
}


window.onload = function() {
	// Create a new WebSocket.
	//var socket = new WebSocket('ws://echo.websocket.org');
	//var socket1 = new WebSocket('ws://192.168.1.68:81');
	for(var i=2 ; i < 255 ; i++){
		webSocketConnect("192.168.1." + i.toString());
	}
//	webSocketConnect("192.168.1.39");
};

