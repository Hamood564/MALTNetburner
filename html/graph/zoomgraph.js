/*2017-09-21 Version using 'svg.viewport' to create a responsive graph.
	The viewport fills its container as the browser window resizes.
	
	This is easy, but you lose control of the font-size: becomes unreadable.
	Alternative is to re-render the graph to the new container size.
	 
	See discussion at: https://stackoverflow.com/questions/16265123/resize-svg-when-window-is-resized-in-d3-js
*/
var data = [];

var xdomain = 18;
//var ydomain = 500; //500;  	//redundant 2020-03-06
//var yrdomain = 30;//50; 		//redundant 2020-03-06
//var colors = ["blue", "red", "green","green"]; //["steelblue", "green", "red"];

//need to work on dynamic resizing - updateDimensions() function. 
var maxWidth = 700; //Math.min(760, window.innerWidth*0.925); //600;
//var maxHeight = Math.min(400, window.innerWidth*2/3); //400; 	   
var maxHeight = 450; //maxWidth*0.7; //Math.min(400, maxWidth*2/3); //400;
 
var margin = {
	top: 20, //maxHeight*0.05, //40,
	right: maxWidth*0.12, //80,
	bottom: 70,// maxHeight*0.2, //70,
	left: maxWidth*0.12 //80
}
var width = maxWidth - margin.left - margin.right;
var height = maxHeight - margin.top - margin.bottom;

/*
console.log("w="+width + " h=" + height);

window.onresize = function(){
	myAlert();
	updateDimensions();
	console.log(width + "," + height);
//	x.range([0,width]);
//	y.range([height,0]);
//	y1.range([height,0]);
//	svg.select("svg")
//					.attr("width", maxWidth) //width + margin.left + margin.right)
//					.attr("height", maxHeight); //height + margin.top + margin.bottom)
	redrawLines();};

function myAlert() {
	console.log("r="+window.innerWidth);
}

function updateDimensions() {
	maxWidth = Math.min(760, window.innerWidth*0.925); //600;
	maxHeight = Math.min(400, window.innerWidth*2/3); //400; 	   
 
	 margin = {
		top: maxHeight*0.05, //40,
		right: maxWidth*0.15, //80,
		bottom: maxHeight*0.2, //70,
		left: maxWidth*0.15 //80
	}
	width = maxWidth - margin.left - margin.right;
	height = maxHeight - margin.top - margin.bottom;
	
}
*/	

//data
var d1 = sessionStorage.getItem('d1')?JSON.parse(sessionStorage.getItem('d1')):[]; //[]; //test pressure
var d2 = sessionStorage.getItem('d2')?JSON.parse(sessionStorage.getItem('d2')):[]; //[]; //diff pressure
//console.log(d1.length + "," + d2.length);

var tpmin = sessionStorage.getItem('tpmin')?JSON.parse(sessionStorage.getItem('tpmin')):[]; //[];	//min tolerance for pressure
var tpmax = sessionStorage.getItem('tpmax')?JSON.parse(sessionStorage.getItem('tpmax')):[]; //[];	//max tolerance for pressure
var maxT = xdomain; //maximum time value on x-axis

//define end points of tolerance lines (test pressure)
//alert( document.getElementById("tp").innerHTML + ", " + document.getElementById("tptol").innerHTML);
var testprogg = 0; //document.getElementById("testprog").innerHTML; //0;
//alert ("testprog="+testprogg);

var tp,tptol,tptolmin,tptolmax,ydomainMin,ydomainMax, yrdomainMin,yrdomainMax;
/*
var tp = 100;
var tptol = 10;
var tptolmin = 150; 
var tptolmax = 50;
var ydomainMin = 0;
var ydomainMax = 200;
*/
//**** For LZ attention - elements "tp" and "tptol" not found first time through - solved? - check
tp = parseInt(document.getElementById("tp").innerHTML,10); 
tptol = parseInt(document.getElementById("tptol").innerHTML,10);

tptolmin = tptol?tp*(1-tptol/100.0):200;
tptolmax = tptol?tp*(1+tptol/100.0):300;
//alert( tp + ", " + tptolmin + ", " + tptolmax);

//define end points of tolerance lines (test pressure)	   
if (tpmin.length==0) tpmin.push([0, tptolmin],[maxT, tptolmin]); 
if (tpmax.length==0) tpmax.push([0, tptolmax],[maxT, tptolmax]);

ydomainMin = 0; //Math.floor(tp*0.5);
ydomainMax = Math.floor(tptolmax*1.1); //Math.floor(tp*1.5);

yrdomainMin = -10;
yrdomainMax = parseInt(document.getElementById("alarmdp").innerHTML, 10)*1.1;  //2020-03-10 should work now on first run through? Check

data.push(d1);
data.push(d2);
data.push(tpmin);
data.push(tpmax);

//		alert ("testprog="+testprogg + ",len="+ tpmin[tpmin.length-1][0]);
//--------------------------------------------
//initGData() is used to clear previous graph when a new test starts - see maltbox.js)
var initGData = function() {
	xdomain = 18;
	d1.length=0;
	d2.length=0;
	tpmin.length=0;
	tpmax.length=0;
	maxT = xdomain;

//define end points of tolerance lines (test pressure)
//alert( document.getElementById("tp").innerHTML + ", " + document.getElementById("tptol").innerHTML);
	testprogg = 0; //document.getElementById("testprog").innerHTML;
//alert ("testprog="+testprogg);
	tp = parseInt(document.getElementById("tp").innerHTML,10);
	tptol = parseInt(document.getElementById("tptol").innerHTML,10);
	tptolmin = tptol?tp*(1-tptol/100.0):200;
	tptolmax = tptol?tp*(1+tptol/100.0):300;
//alert( tp + ", " + tptolmin + ", " + tptolmax);

//define end points of tolerance lines (test pressure)	   
	tpmin.push([0, tptolmin],[maxT, tptolmin]); 
	tpmax.push([0, tptolmax],[maxT, tptolmax]);

	/*lz nov 19*/ //ydomainMin = 0; //Math.floor(tp*0.5);
	/*lz nov 19*/ ydomainMax = Math.floor(tptolmax*1.1); //Math.floor(tp*1.5);
	/*lz mar 20*/ yrdomainMax = parseInt(document.getElementById("alarmdp").innerHTML) * 1.1;

//	data.push(d1);
//	data.push(d2);
//	data.push(tpmin);
//	data.push(tpmax);
}

var redrawLines = function() {
	
	var t = svg.transition().ease(d3.easeLinear).duration(500); //750
	t.select(".xaxis.axis").call(xAxis);
	t.select(".leftaxis.axis").call(yAxis);
	t.select(".rightaxis.axis").call(yAxis1);
	//console.log(x(maxT));

	for(idx in data) {
		t.select(".line" + idx).attr("d", line[idx](data[idx]));
	}

}

//appendData is called from maltbox.js
var appendData = function (dataset) {
	//add more points
	//var dataset created in maltbox.js, updated every 200ms. 5 points
	//dataset elements are dataPoints with fields m(millis), t(pressure), d(diff pressure) 
	for (var i=0; i<dataset.rv.length; i++) {
		d1.push([dataset.rv[i].m/1000,dataset.rv[i].t/10]); //should we floor() these values?)
		//if(document.getElementById("currentState").innerHTML=='Measure') 
		d2.push([dataset.rv[i].m/1000,dataset.rv[i].d/10]);
		//3-3-20, auto-scale diff pressure axis
		yrdomainMax = Math.max(yrdomainMax,dataset.rv[i].d/10+5)
		//yrdomainMin = Math.min(yrdomainMin,dataset.rv[i].d/10-5)
	}
	
	//updateDimensions();
	//alert("d1.length:"+d1.length);
	//console.log("d1.length:"+d1.length);
	//var t = svg.transition().ease(d3.easeLinear); //.ease("linear");//750); //***v4.0
	//alert("max="+Math.ceil(d1[d1.length-1][0])+2);
	xdomain = Math.max(xdomain, d1.length>0?Math.ceil(d1[d1.length-1][0])+2:0);
	x.domain([d1.length>0?d1[0][0]:0, xdomain]); 
//	y.domain([0, ydomain]); //was [0, ydomain]
	y.domain([ydomainMin, ydomainMax]); //was [0, ydomain]

	y1.domain([yrdomainMin, yrdomainMax]);
	
	if(xdomain>maxT) { //tolerance line(s)
		maxT=xdomain;
		tpmin.splice(-1,1,[maxT,tptolmin]); //replace last element with (maxT,180)
		tpmax.splice(-1,1,[maxT,tptolmax]); //replace last element with (maxT,200)
	}

	//var testprog1 = document.getElementById("testprog").innerHTML;
	if (testprogg!=dataset.i){ //chained test --what if chained test is the same one?
		testprogg=dataset.i;
		//alert ("testprog="+testprogg + ",len="+ tpmin.length);
		tpmin.splice(-1,1,[d1[d1.length-1][0], tptolmin]); //truncate to test length.
		tpmax.splice(-1,1,[d1[d1.length-1][0], tptolmax]); //[d1[d1.length-1][0], 420],[maxT, 420]);
//----review 17-9-2019
		tp = dataset.p;//parseInt(document.getElementById("tp").innerHTML,10);

		tptol = dataset.l;//parseInt(document.getElementById("tptol").innerHTML,10);
		tptolmin = tp*(1-tptol/100.0)/10;
		tptolmax = tp*(1+tptol/100.0)/10;
		//alert(tp+','+tptolmin+','+tptolmax+','+maxT);
		//tptolmin = 380;
		//tptolmax = 400;
		tpmin.push([d1[d1.length-1][0], tptolmin],[maxT, tptolmin]); //add next tolerance lines
		tpmax.push([d1[d1.length-1][0], tptolmax],[maxT, tptolmax]);
		//alert("["+tpmin+"]");
		
			//ydomainMin = 0; //Math.floor(tp*0.5);
		ydomainMax = Math.max(ydomainMax, Math.floor(tptolmax*1.1)); //Math.floor(tp*1.5);
		y.domain([ydomainMin, ydomainMax]);
		//console.log(ydomainMin + ", " + tp + ", " + ydomainMax);
//---end of review section...delete comment when done
	}
	//re-draw axes and lines with additional point
	redrawLines();

	//d1.shift(); //remove older points - review
	if (d1.length>2000) {
		d1.splice(0,5);
		d2.splice(0,5);
	}

}

//--------------------------------------------	   
  
var svg = d3.select("#graph")
				.append("svg")
					//.attr("width", maxWidth) //width + margin.left + margin.right)
					//.attr("height", maxHeight) //height + margin.top + margin.bottom)
   				//.attr("preserveAspectRatio", "xMinYMin meet")
   				.attr("viewBox", "0 0 " + maxWidth + " " + maxHeight)
					.append("g")
						.attr("transform", "translate(" + margin.left + "," + margin.top + ")");

 
//alert("width " + window.screen.width + " " + window.innerWidth);
var x = d3.scaleLinear() //d3.scale.linear() ***v4.0
	.range([0, width]).domain([0, xdomain]);
 
var y = d3.scaleLinear() //d3.scaleLinear() //d3.scale.linear() ***v4.0
	.range([height, 0]).domain([ydomainMin, ydomainMax]); //domain([0, ydomain]); // use 1,.. for log scale 

var y1 = d3.scaleLinear() //d3.scale.linear() ***v4.0d3.scale.linear()
	.range([height, 0]).domain([yrdomainMin, yrdomainMax]);
		    
var xAxis = d3.axisBottom(x); //***v4.0
		/*	 = d3.svg.axis()
					.scale(x)
					.orient("bottom");
		*/	 

var yAxis = d3.axisLeft(y); //.tickArguments([5,"5"]);
//.ticks(5).tickFormat(d3.format(".0s")); //.tickFormat(d3.format(".0s")); //***v4.0
		/*	 = d3.svg.axis()
					.scale(y)
					.orient("left");
		*/
		
var yAxis1 = d3.axisRight(y1); //***v4.0
		/*	 = d3.svg.axis()
					.scale(y1)
					.orient("right");
		*/

var line = new Array(4); 
//test pressure
line[0] = d3.line() // d3.svg.line() ***v4.0
	.curve(d3.curveBasis) //.interpolate("basis") ***v4.0
	.x(function (d) {return x(d[0]);})
	.y(function (d) {return y(d[1]);});

//diff pressure
line[1] = d3.line() // d3.svg.line() ***v4.0
	.curve(d3.curveBasis) //.interpolate("basis") ***v4.0
	.x(function (d) {return x(d[0]);})
	.y(function (d) {return y1(d[1]);});

//tolerance
line[2] = d3.line() // d3.svg.line() ***v4.0 
	.curve(d3.curveLinear) //.interpolate("basis") ***v4.0
	.x(function (d) {return x(d[0]);})
	.y(function (d) {return y(d[1]);});

line[3] = line[2];

var innerg =svg.append("g")
	.attr("class", "innerg");
	
	innerg.append('rect') //zoomable area
		.attr("width", width)
		.attr("height",height)
		.attr("fill-opacity", 0.03);

var gX = innerg.append("g") //was svg.append("g")
	.attr("class", "xaxis axis")
	.call(xAxis.ticks(5))
	.attr("transform", "translate(0," + height + ")");

 // Add the text label for the x axis
svg.append("text")
	.attr("transform", "translate(" + (width / 2) + " ," + height + ")")
	.attr("dy","3em")
	.attr("class", "xaxis")
	.attr("text-anchor", "middle")
	.text("Time (sec)");

 
var gY= innerg.append("g") //svg.append("g")
	.attr("class", "leftaxis axis")
	.call(yAxis);

	// Add the text label for the Y-left axis
svg.append("text")
	.attr("transform", "rotate(-90)")
	.attr("y", -margin.left)
	.attr("x",0 - (height / 2))
	.attr("dy","1.5em")
	.attr("class", "leftaxis")
	.attr("text-anchor", "middle")
	.text("Test Pressure (mbar)");

var gY1 = innerg.append("g") //svg.append("g")
	.attr("class", "rightaxis axis")
	.call(yAxis1)
	.attr("transform", "translate("+(width) + ",0)") ; 

// Add the text label for the Y-right axis
svg.append("text")
  .attr("transform", "rotate(-90)")
  .attr("y", maxWidth - margin.left) //width)
  .attr("x", 0 - (height / 2))
  .attr("dy", "-1.5em")
  .attr("class", "rightaxis")
  .attr("text-anchor", "middle")
  .text("Differential Pressure (Pa)");

innerg.append("clipPath") //was svg.
	.attr("id", "clip")
	.append("rect")
	.attr("width", width)
	.attr("height", height);

for(idx in data) {
	innerg.append("path") //was svg.
		.attr("class", "line line" + idx)
		.attr("clip-path", "url(#clip)")
		.select("line" + idx)
			.data(data[idx]);
			//.attr("d", line[idx](data[idx]));  //lz: not strictly necessary, as data points are added in redrawLines())
}

redrawLines();

//============Zooming==================
//function to create a zoom reset button
function zoomBtn() {

	var zoomResetBtn = svg.append("g").attr("id","z100");
	
	zoomResetBtn.append("text")
		.attr("class", "zoomResetText")
		.attr("x",width)
		.attr("y", height + margin.bottom)
		.attr("dx",20)
		.attr("dy","-1em")
		.style("text-anchor", "end")
		.text("Reset zoom");
	
		//get dimensions of the text provided it is visible
		//if (zoomResetBtn.select("text").node().getBoundingClientRect().height>0) {
			//LZ note: getBBox() returns an error if object is not visible
			var bbox = zoomResetBtn.select("text").node().getBBox();
			//console.log(bbox.x + ',' + bbox.y);
			var em = 3; //padding --could this be calculated using em?
			
			//define a bounding rectangle around the button text.
			zoomResetBtn.insert("rect","text") //place below text element
				.attr("class", "zoomResetBtn")
			   .attr("x", bbox.x-em)
			   .attr("y", bbox.y-em)
			   .attr("width", bbox.width+2*em)
			   .attr("height", bbox.height+2*em)
			   .on("click", zoomReset); //function () { zoomReset();	}
		//}
}



var zoom = d3.zoom()
    .scaleExtent([1, 20])
    .translateExtent([[-margin.left, -margin.bottom], [width*2, height+margin.bottom]])
    //.translateExtent([[0, 0], [width, height]])
    .extent([[0, 0], [width, height]]) //viewport
    .on("zoom", zoomed);
    
//d3.select(".innerg").select('rect')
innerg.select('rect').call(zoom);

//alert(zoom.scaleExtent.length + defaultExtent());

function zoomed() {
	//console.log('zoomed');
	//alert(d3.event.transform.k);
	var t=d3.event.transform; 	//get the zoom transform event
	var xt=t.rescaleX(x);		//copy of the x scale whose domain is transformed
	var yt=t.rescaleY(y);		//copy of the y scale whose domain is transformed
	var y1t=t.rescaleY(y1);		//copy of the y scale whose domain is transformed

//remap lines using the rescaled axis mappings
/* -- some undefined reference error here that I don't understand
	svg.select(".line0").attr("d", line[0].x(function(d) { return xt(d[0]); })); //test pres
	svg.select(".line0").attr("d", line[0].y(function(d) { return yt(d[1]); })); //test pres
	svg.select(".line1").attr("d", line[1].x(function(d) { return xt(d[0]); })); //diff pres
	svg.select(".line1").attr("d", line[1].y(function(d) { return y1t(d[1]); })); //diff pres
	svg.select(".line2").attr("d", line[2].x(function(d) { return xt(d[0]); })); //tolerance min
	svg.select(".line2").attr("d", line[2].y(function(d) { return yt(d[1]); })); //tolerance min
	svg.select(".line3").attr("d", line[3].x(function(d) { return xt(d[0]); })); //tolerance max	
	svg.select(".line3").attr("d", line[3].y(function(d) { return yt(d[1]); })); //tolerance max	
*/

	line[0]
		.x(function (d) {return xt(d[0]);})
		.y(function (d) {return yt(d[1]);});

	line[1]
		.x(function (d) {return xt(d[0]);})
		.y(function (d) {return y1t(d[1]);});

	line[2]
		.x(function (d) {return xt(d[0]);})
		.y(function (d) {return yt(d[1]);});

	//lin2[3]=line[2], so can omit
	//update axes' svg definitions using the rescaled mappings	
	gX.call(xAxis.scale(xt));
	gY.call(yAxis.scale(yt)); //scale(d3.event.transform.rescaleY(y)));//.tickArguments([20,"5"])
	gY1.call(yAxis1.scale(y1t));

	redrawLines();
	if(d3.select("#z100").empty()) zoomBtn(); //show zoom button if not already visible
}

function zoomReset() {
	//innerg.transition()
	//d3.select(".innerg")
		innerg.select('rect').transition()
			.ease(d3.easeLinear)
			.duration(750)
      	.call(zoom.transform, d3.zoomIdentity)
      	.on('end',function() {d3.select("#z100").remove();}); 
}	

//redrawLines();
//zoomReset();
