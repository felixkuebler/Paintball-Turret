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