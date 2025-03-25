/* MALT_DEV_V1_7 2016-04-21 */

//XMLHttpRequest object variables
var dataReq;

//When the page loads...
window.onload=function(){
	//check authentication flag in hidden element
	var b=Number(document.getElementById("authentication").innerHTML);
	if (b==0) { //not authenticated - send to password page
		window.location.href = 'password.html';  
	}	
	//update diffP and testP counts and values every 250ms
	setInterval(function(){processPage('runningValues.html', dataReq, updateRunningValues, true)}, 250); 
	//..and process this page immediately
	processPage('runningValues.html', dataReq, updateRunningValues, false);
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

function updateRunningValues(req) {
	if (req.readyState == 4) { // request finished and response is ready
		if (req.status == 200) { // OK response
			document.getElementById("runningValues").innerHTML = req.responseText;
		}
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

