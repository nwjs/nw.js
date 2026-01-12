import http from 'http';
import fs from 'fs';
import path from 'path';

let svr = http.createServer(function (req, res) {
  console.log(`${req.method} ${req.url}`);

  if (req.url === '/redirectme.html') {
    // Note: svr.address() might be null if server is closing, but safe here.
    res.writeHead(308, { 'Location': `/redirected.html` });
    res.end();
    return;
  }

  // Parse URL using the modern WHATWG API (Fixes [DEP0169] warning)
  // We use a dummy base because req.url is relative (e.g. "/file.js")
  const parsedUrl = new URL(req.url, `http://${req.headers.host || 'localhost'}`);
  
  // extract URL path
  let pathname = `.${parsedUrl.pathname}`;
  
  // based on the URL path, extract the file extension. e.g. .js, .doc, ...
  let ext = path.parse(pathname).ext;
  
  // maps file extension to MIME type
  const map = {
    '.ico': 'image/x-icon',
    '.html': 'text/html',
    '.js': 'text/javascript',
    '.json': 'application/json',
    '.css': 'text/css',
    '.png': 'image/png',
    '.jpg': 'image/jpeg',
    '.wav': 'audio/wav',
    '.mp3': 'audio/mpeg',
    '.svg': 'image/svg+xml',
    '.pdf': 'application/pdf',
    '.doc': 'application/msword'
  };

  fs.exists(pathname, function (exist) {
    if(!exist) {
      // if the file is not found, return 404
      res.statusCode = 404;
      res.end(`File ${pathname} not found!`);
      return;
    }

    // if is a directory search for index file matching the extension
    if (fs.statSync(pathname).isDirectory()) {ext = '.html'; pathname += '/index.html';}

    // read file from file system
    fs.readFile(pathname, function(err, data){
      if(err){
        res.statusCode = 500;
        res.end(`Error getting the file: ${err}.`);
      } else {
        const normalizedPathname = path.normalize(pathname);
        const targetFile = path.normalize('./redirected.html');

        if (normalizedPathname === targetFile) {
          res.setHeader('Cross-Origin-Embedder-Policy', 'require-corp');
          res.setHeader('Cross-Origin-Opener-Policy', 'same-origin');
          res.setHeader('Cross-Origin-Resource-Policy', 'same-origin');
          console.log(`Added COOP/COEP/CORP headers for ${pathname}`);
        }
        // if the file is found, set Content-type and send data
        res.setHeader('Content-type', map[ext] || 'text/plain' );
        res.end(data);
      }
    });
  });
});

svr.listen(() => {
  let port = svr.address().port;
  console.log(`Server listening on port ${port}`);

  for (let filename of fs.readdirSync('.')) {
    if (filename.includes(".tpl")) {
      let content = fs.readFileSync(filename, 'utf8');
      content = content.replace(/{port}/g, port);
      console.log(`replace port in template ${filename}`);
      let newfile = filename.replace(".tpl", "");
      fs.writeFileSync(newfile, content);
    }
  }
  fs.writeFileSync('port.txt', port.toString());
});

// Handle kill signals to exit gracefully with code 0.
// This prevents the Python driver from seeing exit code -15 (SIGTERM) as a failure.
process.on('SIGTERM', () => {
  console.log('SIGTERM received, exiting gracefully.');
  process.exit(0);
});

process.on('SIGINT', () => {
  console.log('SIGINT received, exiting gracefully.');
  process.exit(0);
});
