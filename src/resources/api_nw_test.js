var nwNatives = requireNative('nw_natives');

apiBridge.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('crashRenderer', function() {
    nwNatives.crashRenderer();
  });
  apiFunctions.setHandleRequest('done', function() {
    nw.process.exit(0);
  });
  apiFunctions.setHandleRequest('fail', function() {
    nw.process.exit(1);
  });
  apiFunctions.setHandleRequest('assert', function(value, message) {
    if (!value) { console.log(message); nw.process.exit(1); }
  });
});

