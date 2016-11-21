(function(){
const http = require('http');
let id = 1;

// Create an HTTP server
var srv = http.createServer( (req, res) => {
  let headers = {'Content-Type': 'text/plain'};
  let cookies = req.headers['cookie'];
  if (!cookies) {
    headers['Set-Cookie'] = ['sid=' + (id++)];
  }
  res.writeHead(200, headers);
  res.end(`Request Cookie: ${cookies}`);
});

srv.listen(nw.App.manifest.port, '127.0.0.1');
})()
