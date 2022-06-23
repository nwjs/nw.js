<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <title>chrome.sockets.tcp</title>
</head>
<body>
  <button id="socket-connect" type="button" onclick="socketConnect('result')">Connect</button>
  <script>
  function out(id, msg) {
    var result = document.createElement('h1');
    result.setAttribute('id', id);
    result.innerHTML = msg;
    document.body.appendChild(result);
  }

  function socketConnect(id) {
    chrome.sockets.tcp.create(function(obj) {
      chrome.sockets.tcp.connect(obj.socketId, '127.0.0.1', {port}, function(r) {
        if (chrome.runtime.lastError) {
          out(id, 'failed');
        } else {
          out(id, 'success');
        }
      });
    });
  }
  </script>
</body>
</html>
