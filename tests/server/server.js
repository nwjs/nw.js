var http = require("http"),
url = require("url"),
path = require("path"),
fs = require("fs"), 
ports = new Array(8123,8124),
servers = new Array(ports.length);

types = {
    ".css": "text/css",
    ".gif": "image/gif",
    ".html": "text/html",
    ".ico": "image/x-icon",
    ".jpeg": "image/jpeg",
    ".jpg": "image/jpeg",
    ".js": "text/javascript",
    ".json": "application/json",
    ".pdf": "application/pdf",
    ".png": "image/png",
    ".svg": "image/svg+xml",
    ".swf": "application/x-shockwave-flash",
    ".tiff": "image/tiff",
    ".txt": "text/plain",
    ".wav": "audio/x-wav",
    ".wma": "audio/x-ms-wma",
    ".wmv": "video/x-ms-wmv",
    ".xml": "text/xml"
};

function request_listener(req, res) {
    var pathname=__dirname+url.parse(req.url).pathname;

    if (path.extname(pathname)=="") {
	pathname+="/";
    }

    if (pathname.charAt(pathname.length-1)=="/"){
	pathname+="index.html";
    }

    path.exists(pathname,function(exists){
	if(exists){
	    var content_type = types[path.extname(pathname)] || "text/plain";
	    res.writeHead(200, {"Content-Type": content_type});

	    fs.readFile(pathname,function (err,data){
		res.end(data);
	    });
	} else {
	    res.writeHead(404, {"Content-Type": "text/html"});
	    res.end("<h1>404 Not Found</h1>");
	}
    });
}

function error_handler(e) {
    console.log("error code: ", e.code);
}




for (var i = 0; i < ports.length; i++) {
    servers[i] = http.createServer(request_listener);
    servers[i].listen(ports[i], "127.0.0.1");
    servers[i].on('error', error_handler);
    console.log("Server running at http://127.0.0.1:" + ports[i]);
}
