(async function OpenWindow() {
    // This crashes
    await nw.Window.open('http://127.0.0.1:{port}/redirectme.html');
})();