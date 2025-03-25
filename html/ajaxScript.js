/* Single_Malt_v1 2015-01-13 */

//XMLHttpRequest object variables
var statusReq;
var tableReq, dataReqA; 
var resultReqA;
var passfailReqA;
var interval=[];

//When the page loads...
window.onload=function(){
	//..instruct Javascript to run the processPage functions at the given intervals.
	interval[0]=setInterval(function(){processPage('status.html', statusReq, getStatus, true)}, 500); //200 millis
	interval[1]=setInterval(function(){processPage('dataTableA.html', dataReqA, updateDataTableA, true)}, 333); 
	interval[2]=setInterval(function(){processPage('resultTableA.html', resultReqA, updateResultTableA, true)}, 1000); 
	interval[3]=setInterval(function(){processPage('passfailA.html', passfailReqA, updatePassFailA, true)}, 500); 

	//..and process the the dataTable page immediately    
	processPage('dataTableA.html', dataReqA, updateDataTableA, true); //lz dec2019 sychronous update deprecated
	processPage('resultTableA.html', resultReqA, updateResultTableA, true); //lz dec2019 sychronous update deprecated
	processPage('passfailA.html', passfailReqA, updatePassFailA, true); //lz dec2019 sychronous update deprecated
	
	var s = document.getElementById("appliedConfig").innerHTML;
	var i = parseInt(s);
	document.getElementById("selectedTest").selectedIndex=i;
}

function clearAllIntervals() {
	for	(i=0; i<interval.length; i++) {
    clearInterval(interval[i]);
} 
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

//Update "tick" element
function getStatus(req) {
	if (req.readyState == 4) { //  request finished and response is ready
		if (req.status == 200) { // OK response
			document.getElementById("status").innerHTML = req.responseText;
		}
	}
}

function updateDataTableA(req) {
	if (req.readyState == 4) { // request finished and response is ready
		if (req.status == 200) { // OK response
			document.getElementById("dataA").innerHTML = req.responseText;
			var step = parseInt(document.getElementById("stepCount").innerHTML);
			document.getElementById("selectedTest").disabled = (step!=0);
		}
	}
}

function updateResultTableA(req) {
	if (req.readyState == 4) { // request finished and response is ready
		if (req.status == 200) { // OK response
			document.getElementById("resultA").innerHTML = req.responseText;
		}
	}
}
function updatePassFailA(req) {
	if (req.readyState == 4) { // request finished and response is ready
		if (req.status == 200) { // OK response
			document.getElementById("PassFailA").innerHTML = req.responseText;
		}
	}
}
