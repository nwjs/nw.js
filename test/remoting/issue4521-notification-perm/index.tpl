<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <title>issue4521-notification-perm</title>
</head>
<body>
  <button id="show-perm" onclick="showPerm('perm')">Notification Permission</button>
  <button id="show-perm-open-inside" onclick="showPermWin('perm-open-inside', 'test.html')">Notification Permission in window</button>
  <button id="show-perm-open-remote" onclick="showPermWin('perm-open-remote', 'http://localhost:{port}/test.html')">Notification Permission in window remote</button>
  <button id="show-perm-frame-inside" onclick="showPermFrame('perm-frame-inside', 'frame-inside')">Notification Permission in frame</button>
  <button id="show-perm-frame-remote" onclick="showPermFrame('perm-frame-remote', 'frame-remote')">Notification Permission in frame remote</button>
  <iframe id="frame-inside" src="test.html">
  </iframe>
  <iframe id="frame-remote" src="http://localhost:{port}/test.html">
  </iframe>
  <script>
  function out(id, msg) {
    var h1 = document.createElement('h1');
    h1.setAttribute('id', id);
    h1.innerHTML = msg;
    document.body.appendChild(h1);
  }
  function showPerm(id, win) {
    win = win || window;
    out(id, id + ' : ' + win.Notification.permission);
  }
  function showPermWin(id, url) {
    nw.Window.open(url, function(w) {
      if (w && w.window)
        showPerm(id, w.window);
    });
  }
  function showPermFrame(id, frameId) {
    var frm = document.getElementById(frameId);
    showPerm(id, frm.contentWindow);
  }
  </script>
</body>
</html>