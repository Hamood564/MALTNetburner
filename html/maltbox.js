/* Single_Malt_v1 2016-11-03 */

//XMLHttpRequest object variables
var graphReq;
//var interval=[];
/*
//When the page loads...
window.onload=function(){
	//alert("loading");
	//..instruct Javascript to run the processPage functions at the given intervals.
	interval[0]=setInterval(function(){processPage('maltbox.html', maltReq, maltStatus, true)}, 200); //200 millis
	interval[1]=setInterval(function(){processPage('indicators.html', indicatorsReq, indicatorStatus, true)}, 500); 
//	interval[2]=setInterval(function(){processPage('resultTableA.html', resultReqA, updateResultTableA, true)}, 1000); 
//	interval[3]=setInterval(function(){processPage('passfail.html', passfailReq, updatePassFail, true)}, 500); 

	//..and process the the dataTable page immediately    
	processPage('maltbox.html', maltReq, maltStatus, false);
	processPage('indicators.html', indicatorsReq, indicatorStatus, false);

	restoreStateFromStorage(10); //seed value for this page.
	//setScroll();
}
*/

window.onload=function(){
	//clearAllIntervals(); //remove any running intervals
	//alert("loading");
	//..instruct Javascript to run the processPage functions at the given intervals.
	setInterval(function(){processPage('graphdata.html', graphReq, graphStatus, true)}, 200); //200 millis

	//..and process the the dataTable page immediately    
	processPage('graphdata.html', graphReq, graphStatus, true); //false);
}
/*
function clearAllIntervals() {
	alert('clearing intervals:'+interval.length);
	for	(i=0; i<interval.length; i++) {
   	clearInterval(interval[i]);
	} 
}
*/
/*
function processPage(url, req, fn, async) {
	...one copy in moved to frame.js
*/

/*
function dataPoint(millis, testp, diffp){
	this.m=millis;
	this.t=testp;
	this.d=diffp;
//	this.toString = function() {return "{"+this.m+","+this.t+","+this.d +"}";};
}
*/
function dataPoint(x){
	this.t=x.t;
	this.d=x.d;
	this.m=x.m;
	this.toString = function() {return "{"+this.t+","+this.d+","+this.m+"}";};
}

var printds = function(ds) {
	var str='{[<br>'+ ds.rv[0]; 
	for(i=1; i<ds.rv.length; i++) {
		str+=',<br> '+ ds.rv[i]; 
	}
	str+='<br>],' + ds.i + ',' + ds.p +',' + ds.l + '}';
	
	return str;
}

var testStarted=false;
var maxpVal = 0; //max value of  progress bar

function graphStatus(req) {
	if (req.readyState == 4) { //  request finished and response is ready
		if (req.status == 200) { // OK response
			document.getElementById("readings").innerHTML = req.responseText;

			
			var testprog = document.getElementById("testprog").innerHTML;
			var stage = document.getElementById("stage").innerHTML;
			//document.getElementById("stage").innerHTML = currentState.trim();
			document.getElementById("currentState").innerHTML = testprog + ": " + stage.trim();
			//alert('state:' + currentState.trim() +':'+currentState.trim().endsWith('Ready')+':');
			if (stage.trim().endsWith('Waiting')) {
				//testStarted=false;
				console.log(d1.length + "," + d2.length);
				if (testStarted) {
					testStarted=false;
					//save graph data at end of test..so that it can be restored if page refreshed or navigated to.
					sessionStorage.setItem('d1', JSON.stringify(d1));
					sessionStorage.setItem('d2', JSON.stringify(d2));
					sessionStorage.setItem('tpmin', JSON.stringify(tpmin));
					sessionStorage.setItem('tpmax', JSON.stringify(tpmax));
					console.log("saved: "+ d1.length + "," + d2.length + "," + tpmin.length + "," + tpmax.length);
				}
			}
			else {
				//alert("test started=" + testStarted);

				//get most recent n running values in JSON format, i.e. with n=5
				/* {"rv":[
							{"m":400, "t":3000, "d":197},
							{"m":440, "t":3000, "d":197},
							{"m":480, "t":3000, "d":197},
							{"m":520, "t":3000, "d":197},
							{"m":560, "t":3000, "d":197}]
							"i":2, "p":300, "l":20}
				*/	
				var json = document.getElementById("runningValues").innerHTML;
				//alert(json);
				//console.log(json);
				var dataset=JSON.parse(json,function(k,v) {
					if (k=='rv') {
						//console.log(v.length + ' ' + JSON.stringify(v[0]));
						return v.map(function(x){return new dataPoint(x);});
					}
					else return v;
					});
				//dataset = dataset.map(function(x){return new dataPoint(x);});
				//alert("["+dataset.rv+"] "+dataset.rv.length);
				//alert("--"+printds(dataset)+"--");
				if (!testStarted) { //first time through
					//alert("test started");
					initGData();//clearData(); //data=[]; //clear previous graph data
					testStarted=true;
					//remove 'old' running values from before start of test
					//alert(printds(dataset) + " " +dataset.rv.length);					
					while (dataset.rv.length>0 && dataset.rv[0].m>1000) dataset.rv.shift();
					//alert(printds(dataset) + " " +dataset.rv.length);
				}
				//alert(dataset.map(printdp));
				appendData(dataset); //dataset is appended to graph. See zoomchart_dual.js
				//document.getElementById("graphdata").innerHTML = printds(dataset);// + "<br>" + data.length;
				
			}
					
			//document.getElementById("msg").innerHTML = document.getElementById("statusmsg").innerHTML;
			//document.getElementById("clock").innerHTML = document.getElementById("datetime").innerHTML;
		
		}
	}
}



