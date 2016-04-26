var http = require('http');

function startServer(port, cb) {

  var server = http.createServer();

  server.on('request', function(req, res) {
    console.log(req.url);
    if (req.url === '/post.html') {
      res.writeHead(200, {'Content-Type': 'text/html'});
      res.write('<html><body>');
      res.write('<h1 id="method">method: ' + req.method + '</h1>');
      req.on('data', function(data) {
        res.write('<div>' + data + '</div>');
      });
      req.on('end', function() {
        res.end('</body></html>');
      });
    } else {
      res.writeHead(404, {'Content-Type': 'text/plain'});
      res.end('Page not found');
    }
  });

  server.listen(port, cb || function() {});
}