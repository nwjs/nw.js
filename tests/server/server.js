var http = require("http"),
url = require("url"),
path = require("path"),
fs = require("fs"), 
config = require('./config');
ports = new Array(8123,8124),
servers = new Array(ports.length);

var res_save = new Array();

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
    console.log("Receive request");
    if (path.extname(pathname)=="") {
	pathname+="/";
    }

    if (pathname.charAt(pathname.length-1)=="/"){
	pathname+="index.html";
    }

    path.exists(pathname,function(exists){
	if(exists){
	    var ext = path.extname(pathname);
	    var content_type = types[ext] || "text/plain";

	    res.setHeader("Content-Type", content_type);

	    fs.stat(pathname, function (err, stat) {
		var last_modified = stat.mtime.toUTCString();
		var if_modified_since = "If-Modified-Since".toLowerCase();
		res.setHeader("Last-Modified", last_modified);
		
		if (req.headers[if_modified_since] && last_modified == req.headers[if_modified_since]) {
		    res_save.push({"status": 304, 'pathname':req.url.slice(1)});
		    res.writeHead(304, "Not Modified");
		    res.end();
		} else {
		    res_save.push({"status": 200, 'pathname':req.url.slice(1)});
		    fs.readFile(pathname,function (err,data){
			res.end(data);
		    });
		}
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

exports.res_save = res_save;
