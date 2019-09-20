apiBridge.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('openExternal', function() {
    bindingUtil.sendRequestSync('nw.Shell.openExternal', $Array.from(arguments), undefined, undefined);
  });
  apiFunctions.setHandleRequest('openItem', function() {
    bindingUtil.sendRequestSync('nw.Shell.openItem', $Array.from(arguments), undefined, undefined);
  });
  apiFunctions.setHandleRequest('showItemInFolder', function() {
    bindingUtil.sendRequestSync('nw.Shell.showItemInFolder', $Array.from(arguments), undefined, undefined);
  });
});


