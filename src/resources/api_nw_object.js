var forEach = require('utils').forEach;

apiBridge.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('allocateId', function() {
    return bindingUtil.sendRequestSync('nw.Obj.allocateId', $Array.from(arguments), undefined, undefined)[0];
  });
  apiFunctions.setHandleRequest('create', function() {
    return bindingUtil.sendRequestSync('nw.Obj.create', $Array.from(arguments), undefined, undefined);
  });
  apiFunctions.setHandleRequest('destroy', function() {
    return bindingUtil.sendRequestSync('nw.Obj.destroy', $Array.from(arguments), undefined, undefined);
  });
  apiFunctions.setHandleRequest('callObjectMethod', function() {
    return bindingUtil.sendRequestSync('nw.Obj.callObjectMethod', $Array.from(arguments), undefined, undefined);
  });
  // for some reason callObjectMethod is changed to sync behaviour when being ported from nw12
  // in order not to bring side effects before release when fix this for #4593, I introduce the
  // new method as a workaround. Please see removing it later.
  apiFunctions.setHandleRequest('callObjectMethodAsync', function() {
    return bindingUtil.sendRequest('nw.Obj.callObjectMethodAsync', $Array.from(arguments), undefined, undefined);
  });
  apiFunctions.setHandleRequest('callObjectMethodSync', function() {
    return bindingUtil.sendRequestSync('nw.Obj.callObjectMethodSync', $Array.from(arguments), undefined, undefined)[0];
  });
});


