(function () {
	"use strict";

	let refreshBtn = document.getElementById("refresh");
	let frameInfoDiv = document.getElementById("frameInfo");

	refreshBtn.addEventListener("click", function () {
		frameInfoDiv.style.color = "";
		try {
			let iframeWindow = document.querySelector("iframe").contentWindow;
			frameInfoDiv.innerText = "Iframe top accessible: " + (!!iframeWindow.top ? "true" : "false") + "\n" + 
				"Iframe parent accessible: "  + (!!iframeWindow.parent ? "true" : "false") + "\n" + 
				"Iframe top === Iframe parent: " + ((iframeWindow.parent && iframeWindow.parent != window) ? "true" : "false");
		} catch (err) {
			console.error(err);
			frameInfoDiv.style.color = "red";
			frameInfoDiv.innerText = "It failed!\n\n" + err.stack;
		}
	});
})();
