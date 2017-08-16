var line1 = [0];
var line2 = [0];
var labels = [0];
var counter = 0;
var updateChartChooser = 0;
var chartMaxLength = 100;


for (i = 0 ; i < chartMaxLength ; i++){
  // Set the lables 
  if (i % 5 == 0){
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
  subchart: { show: true, },
  zoom: { enabled: true, },
  transition: { duration: null, },
  legend: { show: false, },
  axis: { x: { tick: { values: labels, }, max: 100, }, },
});
function updateChart(){
  if (updateChartChooser == 0){
    line1[counter] = Math.random() * 10;
    line2[counter] = null;
  }else{
    line1[counter] = null;
    line2[counter] = Math.random() * 10;
  }
  chart.load({
    columns: [
      ['data1'].concat(line1),
      ['data2'].concat(line2),
    ],
  });
  counter += 1;
  if (counter == chartMaxLength) {
    counter = 0;
    updateChartChooser = (updateChartChooser+1) % 2;
  }
}  


setInterval(function() {
  updateChart();
}, 100);
