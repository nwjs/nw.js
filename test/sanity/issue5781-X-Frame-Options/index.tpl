<html>
 <body>
  <iframe src="http://localhost:{port}" nwfaketop ></iframe>
  <script>
      document.write('<h1 id="res1">Node is ' + (typeof nw === 'undefined' ? 'DISABLED': 'ENABLED') + '</h1>');
    </script>
 </body>
</html>
