var EventEmitter = nw.require('events').EventEmitter;

var nwScreenBinding;
// Hook Sync API calls
apiBridge.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;
  nwScreenBinding = bindingsAPI.compiledApi;
  ['getScreens', 'initEventListeners', 'startMonitor', 'stopMonitor', 'isMonitorStarted', 'registerStream'].forEach(function(nwSyncAPIName) {
    apiFunctions.setHandleRequest(nwSyncAPIName, function() {
      return bindingUtil.sendRequestSync('nw.Screen.' + nwSyncAPIName, $Array.from(arguments), undefined, undefined);
    });
  });
});

var inited = false;
var events = {
  onDisplayAdded: 'displayAdded',
  onDisplayRemoved: 'displayRemoved',
  onDisplayBoundsChanged: 'displayBoundsChanged'
};

var sourceEvents = {
  onSourceAdded: 'added',
  onSourceRemoved: 'removed',
  onSourceOrderChanged: 'orderchanged',
  onSourceNameChanged: 'namechanged',
  onSourceThumbnailChanged: 'thumbnailchanged'
};

var Screen = new EventEmitter();

Screen.Init = function() {
  if (inited) return;
  inited = true;
  // init native event listeners
  nwScreenBinding.initEventListeners();
  // bind native event listeners and re-emit event to JS
  Object.keys(events).forEach(function(eventName) {
    nwScreenBinding[eventName].addListener(function() {
      var args = Array.prototype.concat.apply([events[eventName]], arguments);
      Screen.emit.apply(Screen, args);
    });
  });
  Object.keys(sourceEvents).forEach(function(eventName) {
    nwScreenBinding[eventName].addListener(function() {
      var args = Array.prototype.concat.apply([sourceEvents[eventName]], arguments);
      Screen.DesktopCaptureMonitor.emit.apply(Screen.DesktopCaptureMonitor, args);
    });
  });

  return Screen;
};

Object.defineProperty(Screen, 'screens', {
  get: function() {
    return nwScreenBinding.getScreens();
  },
  enumerable: true
});

Screen.chooseDesktopMedia = chrome.desktopCapture.chooseDesktopMedia;
Screen.cancelChooseDesktopMedia = chrome.desktopCapture.cancelChooseDesktopMedia;

Screen.DesktopCaptureMonitor = new EventEmitter();
Screen.DesktopCaptureMonitor.start = nwScreenBinding.startMonitor;
Screen.DesktopCaptureMonitor.stop = nwScreenBinding.stopMonitor;
Screen.DesktopCaptureMonitor.registerStream = function(id) {
  return nwScreenBinding.registerStream(id)[0];
};

Object.defineProperty(Screen.DesktopCaptureMonitor, 'started', {
  get: function() {
    return nwScreenBinding.isMonitorStarted()[0];
  },
  enumerable: true
});

exports.$set('binding', Screen);
//Object.keys(Screen).forEach(function(member) { nwScreenBinding[member] = Screen[member]; });