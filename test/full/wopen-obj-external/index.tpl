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
  <script>
  var win;
  var close_tested = false;
  function openWindow() {
    nw.Window.open('http://localhost:{port}/remote.html', function(w) {
      win = w;
      win.on('close', function() {
           if (!close_tested) {
              out('result', 'success');
              close_tested = true;
           } else {
              win.close(true);
           }
      });
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

  </script>
</body>
</html>
