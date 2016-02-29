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

	/* global chrome */

	var ports = {};

	chrome.runtime.onConnect.addListener(function (port) {
	  var tab = null;
	  var name = null;
	  if (isNumeric(port.name)) {
	    tab = port.name;
	    name = 'devtools';
	    installContentScript(+port.name);
	  } else {
	    tab = port.sender.tab.id;
	    name = 'content-script';
	  }

	  if (!ports[tab]) {
	    ports[tab] = {
	      devtools: null,
	      'content-script': null
	    };
	  }
	  ports[tab][name] = port;

	  if (ports[tab].devtools && ports[tab]['content-script']) {
	    doublePipe(ports[tab].devtools, ports[tab]['content-script']);
	  }
	});

	function isNumeric(str) {
	  return +str + '' === str;
	}

	function installContentScript(tabId) {
	  chrome.tabs.executeScript(tabId, { file: '/build/contentScript.js' }, function () {});
	}

	function doublePipe(one, two) {
	  one.onMessage.addListener(lOne);
	  function lOne(message) {
	    // console.log('dv -> rep', message);
	    two.postMessage(message);
	  }
	  two.onMessage.addListener(lTwo);
	  function lTwo(message) {
	    // console.log('rep -> dv', message);
	    one.postMessage(message);
	  }
	  function shutdown() {
	    one.onMessage.removeListener(lOne);
	    two.onMessage.removeListener(lTwo);
	    one.disconnect();
	    two.disconnect();
	  }
	  one.onDisconnect.addListener(shutdown);
	  two.onDisconnect.addListener(shutdown);
	}

/***/ }
/******/ ]);