<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <title>no opener</title>
</head>
<body>
  <button id="winopen" type="button" onclick="winopen()">Open</button>
  <button id="showopener" type="button" onclick="showopener('result')">Show Opener</button>
  <script>
    function out(id, msg) {
      var result = document.createElement('h1');
      result.setAttribute('id', id);
      result.innerHTML = msg;
      document.body.appendChild(result);
    }

    function winopen() {
      window.open('https://127.0.0.1:{port}/index.html');
    }

    function showopener(id) {
      out(id, window.opener ? 'success': 'false');
    }
  </script>
</body>
</html>
