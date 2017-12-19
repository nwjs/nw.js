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

	// proxy from main page to devtools (via the background page)

	var port = chrome.runtime.connect({
	  name: 'content-script'
	});

	port.onMessage.addListener(handleMessageFromDevtools);
	port.onDisconnect.addListener(handleDisconnect);
	window.addEventListener('message', handleMessageFromPage);

	window.postMessage({
	  source: 'react-devtools-content-script',
	  hello: true
	}, '*');

	function handleMessageFromDevtools(message) {
	  window.postMessage({
	    source: 'react-devtools-content-script',
	    payload: message
	  }, '*');
	}

	function handleMessageFromPage(evt) {
	  if (evt.data && evt.data.source === 'react-devtools-bridge') {
	    // console.log('page -> rep -> dev', evt.data);
	    port.postMessage(evt.data.payload);
	  }
	}

	function handleDisconnect() {
	  window.removeEventListener('message', handleMessageFromPage);
	  window.postMessage({
	    source: 'react-devtools-content-script',
	    payload: {
	      type: 'event',
	      evt: 'shutdown'
	    }
	  }, '*');
	}

/***/ }
/******/ ]);