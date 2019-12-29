<!DOCTYPE html>
<head>
  <meta charset="utf-8" />
</head>

<body>
  <div>
    <p>nw.js version: <span id="nw_version">...</span></p>
    <p>Local has nw access: <span id="nw_access">...</span></p>
    <p>Local has remote access: <span id="remote_access">...</span></p>
    <p><a href="http://localhost:{port}/remote.html">Go to remote</a></p>
  </div>
  <script>
    try {
      nw_version.innerHTML = process.versions.nw;
    } catch (e) {
      nw_version.innerHTML = "-";
      console.error(e);
    }

    try {
      nw.Window.title = "Hello From Local :)";
      nw_access.innerHTML = "yes";
    } catch (e) {
      nw_access.innerHTML = "no";
      console.error(e);
    }

    fetch("https://www.google.com")
      .then(() => {
        remote_access.innerHTML = "yes";
      })
      .catch(e => {
        remote_access.innerHTML = "no";
        console.error(e);
      });
  </script>
</body>
