self.onmessage = function(e) {
  var ab = e.data;
  try {
    importNWBin(ab);
    postMessage(mytest(20));
  } catch (e) {
    postMessage(e.message);
  }
}