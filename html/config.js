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
		document.getElementById("configField").selectedIndex=i;
	}
}

function editConfig() {
	var t = document.getElementById("configField");
	var i = t.selectedIndex;
	var label = prompt("Label to identify this configuration", t.options[i].text.substr(3));
	if (label != null) {
		if(label.length > 36) {
	   	 label = label.substring(0,36);
		}
		//var t = document.getElementById("configField");
		label = i+": "+label;
		t.options[i].text = label;
		document.getElementById("renamedconfig").value=label;
	}
}

