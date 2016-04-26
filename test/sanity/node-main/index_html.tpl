<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <title>index.html</title>
</head>
<body>
  <script>
    function reqListener () {
      document.getElementById('result').innerHTML = this.responseText;
    }

    function test() {
      var oReq = new XMLHttpRequest();
      oReq.addEventListener("load", reqListener);
      oReq.open("GET", "http://127.0.0.1:{port}/");
      oReq.send();
    }
  </script>
  <h1 id="result">node-main test</h1>
  <button onclick="test()">test</button>
</body>
</html>
