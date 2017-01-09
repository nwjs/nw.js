window.onload = function() {
  var vm = require('vm');
  var c = new vm.createContext({e: document.body});
  var script = new vm.Script('e.style.background = "#4682B4"; e.innerHTML += "<p id=\'ret\'>success</p>"');
  script.runInContext(c);
};
