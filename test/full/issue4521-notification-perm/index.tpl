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
  <button id="set-perm-block" onclick="setPerm('set-perm-block-result', 'http://localhost:{port}/*', 'block')">Block permission on http://localhost:{port}/*</button>
  <button id="set-perm-ask" onclick="setPerm('set-perm-ask-result', 'http://localhost:{port}/*', 'ask')">Ask permission on http://localhost:{port}/*</button>
  <button id="set-perm-allow" onclick="setPerm('set-perm-allow-result', 'http://localhost:{port}/*', 'allow')">Allow permission on http://localhost:{port}/*</button>
  <button id="clear-results" onclick="clearResults()">Clear results</button>
  <iframe id="frame-inside" src="test.html">
  </iframe>
  <iframe id="frame-remote" src="http://localhost:{port}/test.html">
  </iframe>
  <div id="output"></div>
  <script>
  function out(id, msg) {
    var h1 = document.createElement('h1');
    h1.setAttribute('id', id);
    h1.innerHTML = msg;
    document.getElementById('output').appendChild(h1);
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
  function setPerm(id, pattern, setting) {
    chrome.contentSettings.notifications.set({primaryPattern: pattern, setting: setting}, function(){
      out(id, 'success');
    });
  }
  function clearResults() {
    document.getElementById('output').innerHTML = 'cleared';
  }
  </script>
</body>
</html>