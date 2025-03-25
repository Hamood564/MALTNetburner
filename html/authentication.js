//When the page loads...
window.onload=function(){
	var b=Number(document.getElementById("authentication").innerHTML);
	if (b==0) {
		window.location.href = 'password.html';  
	}	
}

