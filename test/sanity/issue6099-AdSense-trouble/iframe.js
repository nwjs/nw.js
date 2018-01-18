(function () {
	"use strict";

	let refreshBtn = document.getElementById("refresh");
	let iframeInfoDiv = document.getElementById("iframeInfo");

	refreshBtn.addEventListener("click", function () {
		iframeInfoDiv.style.color = "";
		try {
			iframeInfoDiv.innerText = "Top accessible: " + (!!window.top ? "true" : "false") + "\n" + 
				"Parent accessible: "  + (!!window.parent ? "true" : "false") + "\n" + 
				"top === parent: " + ((window.parent && window.parent != window) ? "true" : "false");
		} catch (err) {
			console.error(err);
			iframeInfoDiv.style.color = "red";
			iframeInfoDiv.innerText = "It failed!\n\n" + err.stack;
		}
	});
})();

