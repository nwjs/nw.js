apiBridge.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  apiFunctions.setHandleRequest('getCurrent', function() {
    return bindingUtil.sendRequestSync('nw.currentWindowInternal.getCurrent', $Array.from(arguments), undefined, undefined)[0];
  });
  apiFunctions.setHandleRequest('getZoom', function() {
    return bindingUtil.sendRequestSync('nw.currentWindowInternal.getZoom', $Array.from(arguments), undefined, undefined)[0];
  });
  apiFunctions.setHandleRequest('setZoom', function() {
    return bindingUtil.sendRequestSync('nw.currentWindowInternal.setZoom', $Array.from(arguments), undefined, undefined);
  });
  apiFunctions.setHandleRequest('getTitleInternal', function() {
    return bindingUtil.sendRequestSync('nw.currentWindowInternal.getTitleInternal', $Array.from(arguments), undefined, undefined)[0];
  });
  apiFunctions.setHandleRequest('setTitleInternal', function() {
    return bindingUtil.sendRequestSync('nw.currentWindowInternal.setTitleInternal', $Array.from(arguments), undefined, undefined);
  });
  apiFunctions.setHandleRequest('isKioskInternal', function() {
    return bindingUtil.sendRequestSync('nw.currentWindowInternal.isKioskInternal', $Array.from(arguments), undefined, undefined)[0];
  });
  apiFunctions.setHandleRequest('getWinParamInternal', function() {
    return bindingUtil.sendRequestSync('nw.currentWindowInternal.getWinParamInternal', $Array.from(arguments), undefined, undefined)[0];
  });
  apiFunctions.setHandleRequest('setPrintSettingsInternal', function() {
    return bindingUtil.sendRequestSync('nw.currentWindowInternal.setPrintSettingsInternal', $Array.from(arguments), undefined, undefined)[0];
  });
  apiFunctions.setHandleRequest('setMenu', function() {
    return bindingUtil.sendRequestSync('nw.currentWindowInternal.setMenu', $Array.from(arguments), undefined, undefined)[0];
  });

});
