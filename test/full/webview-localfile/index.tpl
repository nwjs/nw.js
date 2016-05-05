<!DOCTYPE html>
<html>
  <head>
    <script>
      window.name = 'main';
      function test(show) {
        var cdt = document.getElementById('cdt');
        document.getElementById('wv').showDevTools(show, cdt);
      }
    </script>
  </head>
  <body>
    <webview id="wv" partition="trusted" src="{localurl}" autosize="on" minwidth="576" minheight="432" style="width:640px; height:480px"></webview>
    <webview id="wv2" src="1.html" autosize="on" minwidth="576" minheight="432" style="width:640px; height:480px"></webview>
    <webview id="cdt" src="about:blank" autosize="on" minwidth="576" minheight="432" style="width:640px; height:480px"></webview>
    <button onclick="test(true)">show</button>
  </body>
</html>
