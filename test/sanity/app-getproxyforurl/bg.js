(function(){
const http = require('http');
let id = 1;

// Create an HTTP server
var srv = http.createServer( (req, res) => {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.write('function FindProxyForURL(url, host) {\n');
  res.write('if (host == \'www.port3128.com\')\n');
  res.write('  return \'PROXY localhost:3128\'\n');
  res.write(' else if (host == \'www.port4040.com\')\n');
  res.write('  return \'PROXY localhost:4040\'\n');
  res.write('}');
  res.end();
});

srv.listen(nw.App.manifest.port, '127.0.0.1');
})()
