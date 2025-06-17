<html>

<head>
    <title>PDFtest2</title>
    <link rel="stylesheet" href="app.css" />
    <script>
      chrome.test.sendMessage("Launched");
    </script>
</head>

<body>
    <div id="headbar">

  <div id="versions" style="position: fixed;">
      NWjs version - <script>document.write(process.versions['nw']);</script><br />
      IO.js/node.js version - <script>document.write(process.versions['node']);</script><br />
      Chromium version - <script>document.write(process.versions['chromium']);</script><br />
  </div>
    </div>
    <!-- end of headbar div -->

    <script>
      function test(trusted) {
        var webview = document.createElement('webview');
        webview.addEventListener('loadabort', function(e) {
          if (e.reason == 'ERR_DISALLOWED_URL_SCHEME' && !trusted)
            chrome.test.sendMessage("PASSED");
        });
        var loaded = false;
        webview.addEventListener('loadstop', function(e) {
          if (!loaded) {
            loaded = true;
            chrome.test.sendMessage('guest-loaded');
          }
        });
        webview.src = '<pdf_path>';
        if (trusted)
          webview.partition = 'trusted';
        document.body.appendChild(webview);
      }
    </script>
    <div id="container" style="position: absolute; top: 0px; width: 900px; z-index: 99;">
        <ul id="ul" style="position: absolute; top: 90px; left: 30px; width: 90%; margin-left: auto; margin-right: auto; text-align: center;">

          <br />
          <li style="left: 10px; padding: 10px; width: 100%;">
              <table style="border: 1px solid gray; width: 99%; border-collapse: none;">
                  <tr>
                      <td style="border: 1px solid orange; width: 33%; padding-bottom: 10px;">
                          <p style="color: red;">PDF - webview tag using file://</p>
			  <!-- <webview id="wbv1" _partition_ src="<pdf_path>" class="wbv"></webview> -->
                      </td>
                  </tr>
              </table>
          </li>
        </ul>
    </div>
</body>

</html>
