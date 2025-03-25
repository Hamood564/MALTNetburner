/* Single_Malt_v1 2015-01-13 */

//XMLHttpRequest object variables
var graphReq;
var passfailReq; 
var statusReq;

//When the page loads...
window.onload=function(){
	setInterval(function(){processPage('graphdata.html', graphReq, graphStatus, true)}, 200); //200 millis
	setInterval(function(){processPage('../passfailB.html', passfailReq, updatePassFail, true)}, 500); //10 secs
	setInterval(function(){processPage('../status.html', statusReq, getStatus, true)}, 500); //200 millis

	processPage('graphdata.html', graphReq, graphStatus, true); //false);
	processPage('../passfailB.html', passfailReq, updatePassFail, true);
	processPage('../status.html', statusReq, getStatus, true);

	var s = document.getElementById("appliedConfig").innerHTML;
	var i = parseInt(s);
	document.getElementById("selectedTest").selectedIndex=i;

}

function processPage(url, req, fn, async) {
	// Most browser support
	if (window.XMLHttpRequest) {
   	req = new XMLHttpRequest();
	}
	// IE5, IE6 support 
   else if (window.ActiveXObject) {
		req = new ActiveXObject("Microsoft.XMLHTTP");
   }
   
	if (req != null) {
		//specify the function to be called automatically each time the readyState property changes
      req.onreadystatechange = function() {fn(req); };		
		
		//Specify the type of request, and URL, 
		req.open("GET", url, async); 
		// Send to server
		req.send(null);
	}
	else {
		alert("Browser not supported");
	}
}

//Update pass fail circles
function updatePassFail(req) {
	if (req.readyState == 4) { //  request finished and response is ready
		if (req.status == 200) { // OK response
			document.getElementById("PassFailB").innerHTML = req.responseText;
		}
	}
}

//Update time display
function getStatus(req) {
	if (req.readyState == 4) { //  request finished and response is ready
		if (req.status == 200) { // OK response
			document.getElementById("status").innerHTML = req.responseText;
		}
	}
}

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
//var maxpVal = 0; //max value of  progress bar

function graphStatus(req) {
	if (req.readyState == 4) { //  request finished and response is ready
		if (req.status == 200) { // OK response
			document.getElementById("readings").innerHTML = req.responseText;

			var tp = parseInt(document.getElementById("tp").innerHTML,10);			
			
			var testprog = document.getElementById("testprog").innerHTML; //+ "---" + tp;
			var stage = document.getElementById("stage").innerHTML;
			var stepCount = parseInt(document.getElementById("stepCount").innerHTML,10);
			
			//document.getElementById("stage").innerHTML = currentState.trim();
			document.getElementById("currentState").innerHTML =  stage.trim();
			//alert(testprog + ': state:' + stage.trim());
			//document.getElementById("selectedTest").disabled = !stage.trim().endsWith('Waiting');
			document.getElementById("selectedTest").disabled = (stepCount!=0);

			if (stepCount == 0 || stepCount > 20) {//RESETSTEP  or > EVACSTEP
				//console.log(d1.length + "," + d2.length);
				if (testStarted) { //end of test
					testStarted=false;
					//save graph data at end of test..so that it can be restored if page refreshed or navigated to.
					sessionStorage.setItem('d1', JSON.stringify(d1));
					sessionStorage.setItem('d2', JSON.stringify(d2));
					sessionStorage.setItem('tpmin', JSON.stringify(tpmin));
					sessionStorage.setItem('tpmax', JSON.stringify(tpmax));
					//console.log("saved: "+ d1.length + "," + d2.length + "," + tpmin.length + "," + tpmax.length);
				}
			}
								
			else { //stage != 'Waiting' and before Evac i.e. test is running
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
					//remove any 'old' running values from before start of test
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



