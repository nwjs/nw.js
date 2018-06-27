<html>
  <head>
    <title>6655</title>
  </head>
  <body>
    <h1>index.html window</h1>
    <script>
      console.log('index.html console log');

      // The `new-win-policy` hooks works because it spawns a node frame context
      nw.Window.get().on('new-win-policy', function (frame, url, policy) {
        console.log('nwWindow new-win-policy', frame, url, policy);
        nw.Shell.openExternal(url);
        policy.ignore();
      });

      // The `new-win-policy` will NOT work
      nw.Window.open('http://localhost:{port}/test.html', {
        id: 'external-window'
      }, (win) => {
        win.on('new-win-policy', function (frame, url, policy) {
          console.log('externalWindow new-win-policy', frame, url, policy);
          nw.Shell.openExternal(url);
          policy.ignore();
        });
      });
      nw.Window.get().showDevTools();
    </script>
  </body>
</html>
