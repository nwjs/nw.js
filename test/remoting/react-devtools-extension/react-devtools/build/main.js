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

	var panelCreated = false;

	function createPanelIfReactLoaded() {
	  if (panelCreated) {
	    return;
	  }
	  chrome.devtools.inspectedWindow.eval('!!(\n    Object.keys(window.__REACT_DEVTOOLS_GLOBAL_HOOK__._renderers).length || window.React\n  )', function (pageHasReact, err) {
	    if (!pageHasReact || panelCreated) {
	      return;
	    }

	    clearInterval(loadCheckInterval);
	    panelCreated = true;
	    chrome.devtools.panels.create('React', '', 'panel.html', function (panel) {
	      var reactPanel = null;
	      panel.onShown.addListener(function (window) {
	        // when the user switches to the panel, check for an elements tab
	        // selection
	        window.panel.getNewSelection();
	        reactPanel = window.panel;
	        reactPanel.resumeTransfer();
	      });
	      panel.onHidden.addListener(function () {
	        if (reactPanel) {
	          reactPanel.hideHighlight();
	          reactPanel.pauseTransfer();
	        }
	      });
	    });
	  });
	}

	chrome.devtools.network.onNavigated.addListener(function () {
	  createPanelIfReactLoaded();
	});

	// Check to see if React has loaded once per second in case React is added
	// after page load
	var loadCheckInterval = setInterval(function () {
	  createPanelIfReactLoaded();
	}, 1000);

	createPanelIfReactLoaded();

/***/ }
/******/ ]);