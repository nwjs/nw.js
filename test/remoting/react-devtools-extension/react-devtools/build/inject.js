/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};

/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {

/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId])
/******/ 			return installedModules[moduleId].exports;

/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			exports: {},
/******/ 			id: moduleId,
/******/ 			loaded: false
/******/ 		};

/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);

/******/ 		// Flag the module as loaded
/******/ 		module.loaded = true;

/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}


/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;

/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;

/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";

/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(0);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */
/***/ function(module, exports, __webpack_require__) {

	/**
	 * Copyright (c) 2015-present, Facebook, Inc.
	 * All rights reserved.
	 *
	 * This source code is licensed under the BSD-style license found in the
	 * LICENSE file in the root directory of this source tree. An additional grant
	 * of patent rights can be found in the PATENTS file in the same directory.
	 *
	 * 
	 */
	'use strict';

	// Inject a `__REACT_DEVTOOLS_GLOBAL_HOOK__` global so that React can detect that the
	// devtools are installed (and skip its suggestion to install the devtools).

	var installGlobalHook = __webpack_require__(1);
	var installRelayHook = __webpack_require__(3);

	var saveNativeValues = '\nwindow.__REACT_DEVTOOLS_GLOBAL_HOOK__.nativeObjectCreate = Object.create;\nwindow.__REACT_DEVTOOLS_GLOBAL_HOOK__.nativeMap = Map;\nwindow.__REACT_DEVTOOLS_GLOBAL_HOOK__.nativeWeakMap = WeakMap;\nwindow.__REACT_DEVTOOLS_GLOBAL_HOOK__.nativeSet = Set;\n';

	var js = ';(' + installGlobalHook.toString() + '(window))' + ';(' + installRelayHook.toString() + '(window))' + saveNativeValues;

	// This script runs before the <head> element is created, so we add the script
	// to <html> instead.
	var script = document.createElement('script');
	script.textContent = js + saveNativeValues;
	document.documentElement.appendChild(script);
	script.parentNode.removeChild(script);

/***/ },
/* 1 */
/***/ function(module, exports, __webpack_require__) {

	/**
	 * Copyright (c) 2015-present, Facebook, Inc.
	 * All rights reserved.
	 *
	 * This source code is licensed under the BSD-style license found in the
	 * LICENSE file in the root directory of this source tree. An additional grant
	 * of patent rights can be found in the PATENTS file in the same directory.
	 *
	 * 
	 */
	'use strict';

	var _types = __webpack_require__(2);

	/**
	 * NOTE: This file cannot `require` any other modules. We `.toString()` the
	 *       function in some places and inject the source into the page.
	 */
	function installGlobalHook(window) {
	  if (window.__REACT_DEVTOOLS_GLOBAL_HOOK__) {
	    return;
	  }
	  Object.defineProperty(window, '__REACT_DEVTOOLS_GLOBAL_HOOK__', {
	    value: {
	      _renderers: {},
	      helpers: {},
	      inject: function inject(renderer) {
	        var id = Math.random().toString(16).slice(2);
	        this._renderers[id] = renderer;
	        this.emit('renderer', { id: id, renderer: renderer });
	      },
	      _listeners: {},
	      sub: function sub(evt, fn) {
	        var _this = this;

	        this.on(evt, fn);
	        return function () {
	          return _this.off(evt, fn);
	        };
	      },
	      on: function on(evt, fn) {
	        if (!this._listeners[evt]) {
	          this._listeners[evt] = [];
	        }
	        this._listeners[evt].push(fn);
	      },
	      off: function off(evt, fn) {
	        if (!this._listeners[evt]) {
	          return;
	        }
	        var ix = this._listeners[evt].indexOf(fn);
	        if (ix !== -1) {
	          this._listeners[evt].splice(ix, 1);
	        }
	        if (!this._listeners[evt].length) {
	          this._listeners[evt] = null;
	        }
	      },
	      emit: function emit(evt, data) {
	        if (this._listeners[evt]) {
	          this._listeners[evt].map(function (fn) {
	            return fn(data);
	          });
	        }
	      }
	    }
	  });
	}

	module.exports = installGlobalHook;

/***/ },
/* 2 */
/***/ function(module, exports) {

	/**
	 * Copyright (c) 2015-present, Facebook, Inc.
	 * All rights reserved.
	 *
	 * This source code is licensed under the BSD-style license found in the
	 * LICENSE file in the root directory of this source tree. An additional grant
	 * of patent rights can be found in the PATENTS file in the same directory.
	 *
	 * 
	 */
	'use strict';

/***/ },
/* 3 */
/***/ function(module, exports) {

	/**
	 * Copyright (c) 2015-present, Facebook, Inc.
	 * All rights reserved.
	 *
	 * This source code is licensed under the BSD-style license found in the
	 * LICENSE file in the root directory of this source tree. An additional grant
	 * of patent rights can be found in the PATENTS file in the same directory.
	 *
	 * 
	 */
	'use strict';

	/**
	 * NOTE: This file cannot `require` any other modules. We `.toString()` the
	 *       function in some places and inject the source into the page.
	 */

	function installRelayHook(window) {
	  var TEXT_CHUNK_LENGTH = 500;

	  var hook = window.__REACT_DEVTOOLS_GLOBAL_HOOK__;
	  if (!hook) {
	    return;
	  }

	  function decorate(obj, attr, fn) {
	    var old = obj[attr];
	    obj[attr] = function () {
	      var res = old.apply(this, arguments);
	      fn.apply(this, arguments);
	      return res;
	    };
	  }

	  var _eventQueue = [];
	  var _listener = null;
	  function emit(name, data) {
	    _eventQueue.push({ name: name, data: data });
	    if (_listener) {
	      _listener(name, data);
	    }
	  }

	  function setRequestListener(listener) {
	    if (_listener) {
	      throw new Error('Relay Devtools: Called only call setRequestListener once.');
	    }
	    _listener = listener;
	    _eventQueue.forEach(function (_ref) {
	      var name = _ref.name;
	      var data = _ref.data;

	      listener(name, data);
	    });

	    return function () {
	      _listener = null;
	    };
	  }

	  function recordRequest(type, start, request, requestNumber) {
	    var id = Math.random().toString(16).substr(2);
	    request.then(function (response) {
	      emit('relay:success', {
	        id: id,
	        end: performance.now(),
	        response: response.response
	      });
	    }, function (error) {
	      emit('relay:failure', {
	        id: id,
	        end: performance.now(),
	        error: error
	      });
	    });
	    var textChunks = [];
	    var text = request.getQueryString();
	    while (text.length > 0) {
	      textChunks.push(text.substr(0, TEXT_CHUNK_LENGTH));
	      text = text.substr(TEXT_CHUNK_LENGTH);
	    }
	    return {
	      id: id,
	      name: request.getDebugName(),
	      requestNumber: requestNumber,
	      start: start,
	      text: textChunks,
	      type: type,
	      variables: request.getVariables()
	    };
	  }

	  var requestNumber = 0;

	  function instrumentRelayRequests(relayInternals) {
	    var NetworkLayer = relayInternals.NetworkLayer;

	    decorate(NetworkLayer, 'sendMutation', function (mutation) {
	      requestNumber++;
	      emit('relay:pending', [recordRequest('mutation', performance.now(), mutation, requestNumber)]);
	    });

	    decorate(NetworkLayer, 'sendQueries', function (queries) {
	      requestNumber++;
	      var start = performance.now();
	      emit('relay:pending', queries.map(function (query) {
	        return recordRequest('query', start, query, requestNumber);
	      }));
	    });

	    var instrumented = {};
	    for (var key in relayInternals) {
	      if (relayInternals.hasOwnProperty(key)) {
	        instrumented[key] = relayInternals[key];
	      }
	    }
	    instrumented.setRequestListener = setRequestListener;
	    return instrumented;
	  }

	  var _relayInternals = null;
	  Object.defineProperty(hook, '_relayInternals', {
	    set: function set(relayInternals) {
	      _relayInternals = instrumentRelayRequests(relayInternals);
	    },
	    get: function get() {
	      return _relayInternals;
	    }
	  });
	}

	module.exports = installRelayHook;

/***/ }
/******/ ]);