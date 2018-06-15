<html>
  <body>
    <script>window.name="index"</script>
    <h1 id="rsu1">Index</h1>
    <script>
      nw.Window.get().showDevTools();
      nw.Window.open("http://localhost:{port}/test.html", {}, function(w) {
        w.on('close', function() {
          console.log('onclose');
          this.close(true);
        });

        w.on('loaded', function() {
          console.log('onloaded');
        });

        w.on('closed', function() {
          console.log('onclosed');
          setTimeout(function(){
            nw.Window.get().closeDevTools();
          }, 5000);
        });
      });
    </script>
    <button id="btn1" onclick="nw.Window.get().reload();">Click Me</button>
  </body>
</html>