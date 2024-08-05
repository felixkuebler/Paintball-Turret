function displayFullScreenElements(container, image, canvas, deviceType) {

    if(deviceType == 0 && document.pointerLockElement !== canvas && document.mozPointerLockElement !== canvas) {
        canvas.requestPointerLock = canvas.requestPointerLock || canvas.mozRequestPointerLock;
        canvas.requestPointerLock();
    }

    container.style.display="block";

    canvas.style.display="block";
    canvas.style.position = "absolute";
    canvas.style.left = 0;
    canvas.style.top = 0;
    canvas.width = window.screen.width;
    canvas.height = window.screen.height;

    var scale = window.screen.width / image.width;
    image.style.display="block";
    image.style.position = "absolute";
    image.width = window.screen.width;
    image.height = image.width / 4 * 3;
    image.style.top = (window.screen.height-image.height)/2 + "px";
}


function hideFullScreenElements(container, image, canvas) {
    if(document.pointerLockElement === canvas || document.mozPointerLockElement === canvas) {
        document.exitPointerLock = document.exitPointerLock || document.mozExitPointerLock;
        document.exitPointerLock();
    }

    container.style.display="none";
}


function displayJoyStick(joystickBase, joystickKnob, deviceType) {

    if(deviceType==0) {
        joystickBase.style.display="none";
        joystickKnob.style.display="none";

    }
    else if (deviceType==1) {
        joystickBase.style.display="block";
        joystickKnob.style.display="block";

        var width = window.screen.width;
        var height = window.screen.height;

        var baseSize = 16;
        var knobSize = baseSize/2;

        joystickBase.style.width = width/100*baseSize + "px";
        joystickKnob.style.width = width/100*knobSize + "px";

        joystickBase.style.left = width/100*80 + "px";
        joystickKnob.style.left = width/100*(80+baseSize/2-knobSize/2) + "px";


        joystickBase.style.top = height/100*65 + "px";
        joystickKnob.style.top = height/100*(65+baseSize/2-knobSize/2) + 9 + "px";

    }
}


function httpGetAsync(url, request, data, callback){
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
            callback(xmlHttp.responseText);
    }
    xmlHttp.open("GET", url + "?" + request, true); // true for asynchronous 
    xmlHttp.send(data);
}


function httpPostAsync(url, data){
    var xhr = new XMLHttpRequest();
    xhr.open("POST", url, true);
    xhr.setRequestHeader('Content-Type', 'text/plain');
    xhr.send(data);
}


function map(x, in_min, in_max, out_min, out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

