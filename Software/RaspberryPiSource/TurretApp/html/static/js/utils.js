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
  return ((x - in_min) * ((out_max - out_min) / (in_max - in_min))) + out_min;
}

