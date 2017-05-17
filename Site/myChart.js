//setInterval(addVarToChart,1000);

var myChart = new CanvasJS.Chart("chartAnalog",{
	datasets:[{
		label:"First Test",
		data:[1,2,3,4,5]
	}]
});

window.onload = function(){
	myChart.render();
};
