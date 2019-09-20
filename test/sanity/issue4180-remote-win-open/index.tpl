<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <title>window.open doesn't match node-remote</title>
</head>
<body>
    <button id="winopen" onclick="winopen('result')">Open</button>
    <script>
      function out(id, msg) {
        var result = document.createElement('h1');
        result.setAttribute('id', id);
        result.innerHTML = msg;
        document.body.appendChild(result);
      }
    if (window.opener) {
         console.log("==> IN NEW WINDOW");
         window.opener.postMessage('success', '*');
    }
    window.addEventListener('message', function(e) {
       console.log("==> SUCCESS");
       out('result', e.data);
    });
    function winopen(id) {
        window.open('http://localhost:{port}/index.html');
    }
    </script>
</body>
</html>
