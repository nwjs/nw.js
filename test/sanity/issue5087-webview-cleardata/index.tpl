<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
</head>
<body style="width:800px; height:500px">
  <div style="width:800px; height:500px">
    <webview src="http://localhost:{port}/1.html" id="webview" style="display:block;width:700px; height:400px"></webview>
  </div>
  <button id="clear">clear data</button>
  <p id="result"></p>
  <script type="text/javascript">
  window.name = 'index';
  window.onload = () => {
    var webview = document.getElementById('webview')
    var clear = document.getElementById('clear')

    clear.addEventListener('click', () => {
      var data = {
        appcache: true,
        cache: true,
        fileSystems: true,
        indexedDB: true,
        localStorage: true,
        webSQL: true
      }
      webview.clearData({
        since: 0
      }, data, () => {
        document.getElementById('result').innerHTML = 'WebviewBody.js _clearWebviewData success!'
      })
    })
  }
  </script>
</body>

</html>
