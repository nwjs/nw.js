nw.Window.open('index.html', {}, function(win) {
  win.on('navigation', function (frame, url, policy) {
    console.log("navigation called: " + url);
    policy.ignore();
    win.window.document.querySelector('#result').innerHTML = 'success: ' + url;
  });
});
