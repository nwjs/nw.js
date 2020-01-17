const https = require('https');
const fs = require('fs');

const options = {
  key: fs.readFileSync('key.pem'),
  cert: fs.readFileSync('cert.pem')
};


function setHeaders(res) {
  res.writeHead(200, [
    ['Content-type', "text/html"],
    ['Set-Cookie', 'same-site-cookie=foo; path=/test_cookie; SameSite=Lax; Secure'],
    ['Set-Cookie', 'cross-site-cookie=bar; path=/test_cookie; SameSite=None; Secure'],
    ['Set-Cookie', 'no-samesite-cookie=nee; path=/test_cookie; Secure']
  ]);
}

let svr = https.createServer(options, (req, res) => {
  let cookiestring = req.headers['cookie'];
  let log =  "Cookie sent by client on " + req.url + ": " + (cookiestring || '');
  console.log(log);
  if (req.url == '/test_cookie/index.html') {
    setHeaders(res);
    res.end("<script type='text/javascript'> document.write('<h1 id=\\'result\\'>' + \
          document.cookie + '</h1>'); </script> \
          <img src='1.svg'></img> \
          <p id='svr'>" + log + "</p>");
    fs.writeFileSync('svrlog.txt', log + "\n", {flag: 'a'});
    return;
  }

  if (req.url == '/test_cookie/1.svg') {
    fs.writeFileSync('svrlog.txt', log + "\n", {flag: 'a'});
    res.writeHead(200, [
      ['Content-type', 'image/svg+xml']]);
    res.end('<svg version="1.1" baseProfile="full" height="100" width="100" xmlns="http://www.w3.org/2000/svg"> \
  <circle cx="50" cy="50" r="40" stroke="black" stroke-width="3" fill="red" /></svg>');
    return;
  }
});

svr.listen(() => {
  let port = svr.address().port;
  for (let filename of fs.readdirSync('.')) {
    if (filename.includes(".tpl")) {
      let content = fs.readFileSync(filename, 'utf8');
      content = content.replace(/{port}/g, port);
      console.log(`replace port in template ${filename}`);
      let newfile = filename.replace(".tpl", "");
      fs.writeFileSync(newfile, content);
    }
  }
  fs.writeFileSync('port.txt', port);
});

