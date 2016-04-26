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
    function winopen(id) {
        var win = window.open('http://localhost:{port}/index.html');
        win.onload = function() {
            out(id, 'success');
        };
    }
    </script>
</body>
</html>