(function(){
const http = require('http');
let id = 1;

nw.Window.open(`http://localhost:${nw.App.manifest.port}/`, (w)=>{
  //w.showDevTools();
  //w.closeDevTools();
});

// Create an HTTP server
let srv = http.createServer( (req, res) => {
  let headers = {'Content-Type': 'text/html'};
  let cookies = req.headers['cookie'];
  if (!cookies) {
    headers['Set-Cookie'] = ['sid=' + (id++)];
  }
  res.writeHead(200, headers);
  require('fs').createReadStream('index.html').pipe(res);
});

srv.listen(nw.App.manifest.port, '127.0.0.1');
})()
