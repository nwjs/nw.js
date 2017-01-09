chrome.app.runtime.onLaunched.addListener(function() {
    chrome.app.window.create('window.html', {
        'outerBounds': {
            'width': 800,
            'height': 600,
            'minWidth': 320,
            'minHeight': 320
        }
    });
});
