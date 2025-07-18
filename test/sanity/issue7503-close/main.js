function addButton(win) {
    let button = win.window.document.createElement("button");
    button.id = "close";
    button.innerHTML = "close NW.js";

    button.onclick = function() {
        win.close();
    };

    win.window.document.getElementsByTagName("body")[0].appendChild(button);
}

var mainWindow;

nw.Window.open("main.html", {}, function (win) {
	mainWindow = win;
});

nw.Window.open("local.html", {}, function (win) {
    win.on("close", function () {
        mainWindow.window.document.getElementById("result_local").textContent = "local";
        this.close(true);
    });

    win.on("loaded", function () {
        addButton(win);
    });
});

nw.Window.open("https://nwjs.io", {}, function (win) {
    win.on("close", function () {
        mainWindow.window.document.getElementById("result_remote").textContent = "remote";
        this.close(true);
    });

    win.on("loaded", function () {
        addButton(win);
    });
});
