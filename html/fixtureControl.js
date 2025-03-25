//When the page loads...
window.onload=function(){
	var b=Number(document.getElementById("authentication").innerHTML);
	if (b==0) {
		window.location.href = "password.html";  
	}
	else {
		var s = document.getElementById("selectedConfig").innerHTML;
		var i = parseInt(s);
		//alert("config:" + i + " " + (typeof i));
		//document.getElementById("configField").innerHTML = i;
//		document.getElementById("configField").selectedIndex=i;
//		document.getElementById("configField").disabled=true;
	}
}

function validateIO() {
	if (!document.getElementById("exportAll").checked && !document.getElementById("importAll").checked) {
		alert('Select Export or Import ' + document.getElementById("filename").value);
		return false;
	}
	else if (document.getElementById("filename").value=="") {
		alert('Enter a filename');
		return false;		
	}	
	return true;
}

/*
function updateEnabled(n) {
	var b = document.getElementById("enablecontrol"+n).checked;
	document.getElementById("fixturetime"+n).disabled = !b;
	document.getElementById("enablefeedback"+n).disabled = !b;
}
*/