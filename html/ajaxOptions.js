/* Single_Malt_v1 2015-01-13 */

//XMLHttpRequest object variables
var statusReq;

//When the page loads...
window.onload=function(){
	//check authentication flag in hidden element
	var b=Number(document.getElementById("authentication").innerHTML);
	if (b==0) { //not authenticated - send to password page
		window.location.href = 'password.html';  
	}	
	
	setRTCFields();
	
	setInterval(function(){processPage('status.html', statusReq, getStatus, true)}, 10000); //10 secs

	processPage('status.html', statusReq, getStatus, false);
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

	function setRTCFields() {
		var d = new Date();
		document.getElementById("dayOfWeek").selectedIndex = d.getDay();
		document.getElementById("dayOfMonth").selectedIndex = d.getDate()-1;
		document.getElementById("month").selectedIndex = d.getMonth();
		document.getElementById("year").selectedIndex = d.getFullYear()-2014;
		document.getElementById("hh").value = ("0" + d.getHours()).slice(-2);
		document.getElementById("mm").value = ("0" + d.getMinutes()).slice(-2);
		document.getElementById("ss").value = ("0" + d.getSeconds()).slice(-2);
	}	

