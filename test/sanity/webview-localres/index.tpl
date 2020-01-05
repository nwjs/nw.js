<!DOCTYPE html>
<html>
  <head>
    <script>
      window.name = 'main';
    </script>
  <script type="text/javascript">
    window.addEventListener('load', () => {
      setTimeout(() => {
      var fs = require('fs')
          var webview = document.getElementById('wv');
          var thePath = '{localfile}';
          var html = fs.readFileSync(thePath).toString();
          webview.loadDataWithBaseUrl(`data:text/html;utf-8,${encodeURIComponent(html)}`,  '{localdirurl}/');
      },300)
    })
  </script>
  </head>
  <body>
    <webview id="wv" partition="trusted" src="about:blank" autosize="on" minwidth="576" minheight="432" style="width:640px; height:480px"></webview>
  </body>
</html>
