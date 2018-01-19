chrome.app.runtime.onLaunched.addListener(function(launchData) {
  chrome.app.window.create(
  'index.html',
    {
      id: 'mainWindow',
      bounds: {width: 800, height: 500}
    }
  );
});
