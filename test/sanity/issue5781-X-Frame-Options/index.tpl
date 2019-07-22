<html>
 <body>
  <script>
      function out(id, msg) {
        var result = document.createElement('h1');
        result.setAttribute('id', id);
        result.innerHTML = msg;
        document.body.appendChild(result);
      }
     window.addEventListener('message', function(e) {
       console.log("==> SUCCESS");
       out('result', e.data);
     });
  </script>
  <iframe src="http://localhost:{port}" nwfaketop id="iframe_a"></iframe>
  <script>
      document.write('<h1 id="res1">Node is ' + (typeof nw === 'undefined' ? 'DISABLED': 'ENABLED') + '</h1>');
    </script>
 </body>
</html>
