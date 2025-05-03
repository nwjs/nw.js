var http = require('http');

function startServer(cb) {

  var server = http.createServer();
  let requests = 0;
  server.on('request', function(req, res) {
    console.log(req.url);
    if (req.url === '/post.html') {
      res.writeHead(200, {'Content-Type': 'text/html'});
      res.write('<html><body>');
      res.write('<h1 id="method">method: ' + req.method + '</h1>');
      res.write('<h1 id="reqno">reqno: ' + requests + '</h1>');
      requests++;
      req.on('data', function(data) {
        res.write('<div id="data">' + data + '</div>');
      });
      req.on('end', function() {
        res.write('<form method="POST" action="http://localhost:' + server.address().port + '/post.html">');
        res.write('<input type="text" name="myfield" value="myvalue2">');
        res.write('<button id="submit" type="submit">Submit</button>');
        res.write('</form>');
        res.end('</body></html>');
      });
    } else {
      res.writeHead(404, {'Content-Type': 'text/plain'});
      res.end('Page not found');
    }
  });

  server.listen(0, cb || function() {});
}
