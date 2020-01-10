nw.Window.open('test.html', {}, function(win) {
	win.on("loaded", function() {
		win.window.document.querySelector("#result").innerText += "NW.js loaded event\n";
	});
});
