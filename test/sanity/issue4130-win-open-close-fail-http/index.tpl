<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <title>index.html</title>
  <link rel="stylesheet" href="">
</head>
<body>
  <h1>Index.html</h1>
  <button id="open-win" onclick="openWindow()">Open window</button>
  <button id="close-win" onclick="closeWindow()">Close window</button>
  <button id="test-request" onclick="testRequest('result')">test request</button>
  <script>
  var win;

  function openWindow() {
    nw.Window.open('index.html', {id: 'new-win'}, function(w) {
      win = w;
    });
  }

  function closeWindow() {
    win.close();
    win = null;
  }

  function out(id, msg) {
    var h1 = document.createElement('h1');
    h1.setAttribute('id', id);
    h1.innerHTML = msg;
    document.body.appendChild(h1);
  }

  function testRequest(id) {
    var http = require('http');
    var ret = http.get('http://127.0.0.1:{port}/', function (response) {
      if (response.statusCode == 200) {
        out(id, 'success');
      } else {
        out(id, 'failed');
      }
      clearTimeout(timer);
    }).on('error', function(err) {
      out(id, 'failed');
      clearTimeout(timer);
    });
    var timer = setTimeout(function() {
      alert('timeout');
    }, 5000);
  }
  </script>
</body>
</html>
