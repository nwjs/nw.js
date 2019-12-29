/******/ (function(modules) { // webpackBootstrap
/******/ 	// install a JSONP callback for chunk loading
/******/ 	function webpackJsonpCallback(data) {
/******/ 		var chunkIds = data[0];
/******/ 		var moreModules = data[1];
/******/ 		var executeModules = data[2];
/******/
/******/ 		// add "moreModules" to the modules object,
/******/ 		// then flag all "chunkIds" as loaded and fire callback
/******/ 		var moduleId, chunkId, i = 0, resolves = [];
/******/ 		for(;i < chunkIds.length; i++) {
/******/ 			chunkId = chunkIds[i];
/******/ 			if(Object.prototype.hasOwnProperty.call(installedChunks, chunkId) && installedChunks[chunkId]) {
/******/ 				resolves.push(installedChunks[chunkId][0]);
/******/ 			}
/******/ 			installedChunks[chunkId] = 0;
/******/ 		}
/******/ 		for(moduleId in moreModules) {
/******/ 			if(Object.prototype.hasOwnProperty.call(moreModules, moduleId)) {
/******/ 				modules[moduleId] = moreModules[moduleId];
/******/ 			}
/******/ 		}
/******/ 		if(parentJsonpFunction) parentJsonpFunction(data);
/******/
/******/ 		while(resolves.length) {
/******/ 			resolves.shift()();
/******/ 		}
/******/
/******/ 		// add entry modules from loaded chunk to deferred list
/******/ 		deferredModules.push.apply(deferredModules, executeModules || []);
/******/
/******/ 		// run deferred modules when all chunks ready
/******/ 		return checkDeferredModules();
/******/ 	};
/******/ 	function checkDeferredModules() {
/******/ 		var result;
/******/ 		for(var i = 0; i < deferredModules.length; i++) {
/******/ 			var deferredModule = deferredModules[i];
/******/ 			var fulfilled = true;
/******/ 			for(var j = 1; j < deferredModule.length; j++) {
/******/ 				var depId = deferredModule[j];
/******/ 				if(installedChunks[depId] !== 0) fulfilled = false;
/******/ 			}
/******/ 			if(fulfilled) {
/******/ 				deferredModules.splice(i--, 1);
/******/ 				result = __webpack_require__(__webpack_require__.s = deferredModule[0]);
/******/ 			}
/******/ 		}
/******/
/******/ 		return result;
/******/ 	}
/******/
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// object to store loaded and loading chunks
/******/ 	// undefined = chunk not loaded, null = chunk preloaded/prefetched
/******/ 	// Promise = chunk loading, 0 = chunk loaded
/******/ 	var installedChunks = {
/******/ 		"app": 0
/******/ 	};
/******/
/******/ 	var deferredModules = [];
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, { enumerable: true, get: getter });
/******/ 		}
/******/ 	};
/******/
/******/ 	// define __esModule on exports
/******/ 	__webpack_require__.r = function(exports) {
/******/ 		if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 			Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 		}
/******/ 		Object.defineProperty(exports, '__esModule', { value: true });
/******/ 	};
/******/
/******/ 	// create a fake namespace object
/******/ 	// mode & 1: value is a module id, require it
/******/ 	// mode & 2: merge all properties of value into the ns
/******/ 	// mode & 4: return value when already ns object
/******/ 	// mode & 8|1: behave like require
/******/ 	__webpack_require__.t = function(value, mode) {
/******/ 		if(mode & 1) value = __webpack_require__(value);
/******/ 		if(mode & 8) return value;
/******/ 		if((mode & 4) && typeof value === 'object' && value && value.__esModule) return value;
/******/ 		var ns = Object.create(null);
/******/ 		__webpack_require__.r(ns);
/******/ 		Object.defineProperty(ns, 'default', { enumerable: true, value: value });
/******/ 		if(mode & 2 && typeof value != 'string') for(var key in value) __webpack_require__.d(ns, key, function(key) { return value[key]; }.bind(null, key));
/******/ 		return ns;
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "/";
/******/
/******/ 	var jsonpArray = window["webpackJsonp"] = window["webpackJsonp"] || [];
/******/ 	var oldJsonpFunction = jsonpArray.push.bind(jsonpArray);
/******/ 	jsonpArray.push = webpackJsonpCallback;
/******/ 	jsonpArray = jsonpArray.slice();
/******/ 	for(var i = 0; i < jsonpArray.length; i++) webpackJsonpCallback(jsonpArray[i]);
/******/ 	var parentJsonpFunction = oldJsonpFunction;
/******/
/******/
/******/ 	// add entry module to deferred list
/******/ 	deferredModules.push([0,"chunk-vendors"]);
/******/ 	// run deferred modules when ready
/******/ 	return checkDeferredModules();
/******/ })
/************************************************************************/
/******/ ({

/***/ 0:
/***/ (function(module, exports, __webpack_require__) {

module.exports = __webpack_require__("56d7");


/***/ }),

/***/ "1fa3":
/***/ (function(module) {

module.exports = JSON.parse("{\"home\":{\"welcome\":\"Willkommen in Deiner Vue.js / NW.js App\"}}");

/***/ }),

/***/ "56d7":
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.array.iterator.js
var es_array_iterator = __webpack_require__("e260");

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.promise.js
var es_promise = __webpack_require__("e6cf");

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.object.assign.js
var es_object_assign = __webpack_require__("cca6");

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.promise.finally.js
var es_promise_finally = __webpack_require__("a79d");

// EXTERNAL MODULE: ./node_modules/vue/dist/vue.runtime.esm.js
var vue_runtime_esm = __webpack_require__("2b0e");

// EXTERNAL MODULE: ./node_modules/bootstrap-vue/esm/index.js + 240 modules
var esm = __webpack_require__("5f5b");

// CONCATENATED MODULE: ./node_modules/cache-loader/dist/cjs.js?{"cacheDirectory":"node_modules/.cache/vue-loader","cacheIdentifier":"7bdce744-vue-loader-template"}!./node_modules/vue-loader/lib/loaders/templateLoader.js??vue-loader-options!./node_modules/cache-loader/dist/cjs.js??ref--0-0!./node_modules/vue-loader/lib??vue-loader-options!./src/App.vue?vue&type=template&id=d0113f76&
var Appvue_type_template_id_d0113f76_render = function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('div',{attrs:{"id":"app"}},[_c('nav-bar'),_c('keep-alive',{attrs:{"include":"home"}},[_c('router-view')],1)],1)}
var staticRenderFns = []


// CONCATENATED MODULE: ./src/App.vue?vue&type=template&id=d0113f76&

// CONCATENATED MODULE: ./node_modules/cache-loader/dist/cjs.js?{"cacheDirectory":"node_modules/.cache/vue-loader","cacheIdentifier":"7bdce744-vue-loader-template"}!./node_modules/vue-loader/lib/loaders/templateLoader.js??vue-loader-options!./node_modules/cache-loader/dist/cjs.js??ref--0-0!./node_modules/vue-loader/lib??vue-loader-options!./src/components/NavBar.vue?vue&type=template&id=8e64b3cc&
var NavBarvue_type_template_id_8e64b3cc_render = function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('b-navbar',{attrs:{"type":"dark","variant":"success"}},[_c('b-navbar-nav',[_c('b-nav-item',{attrs:{"to":"/","vatiant":"link"}},[_vm._v("Home")]),_c('b-nav-item',{attrs:{"to":"/about","vatiant":"link"}},[_vm._v("About")])],1),_c('b-navbar-nav',{staticClass:"ml-auto"},[_c('b-nav-item-dropdown',{attrs:{"text":_vm.dropDownLabel,"variant":"success","right":""}},_vm._l((_vm.locales),function(locale){return _c('b-dropdown-item',{key:locale,on:{"click":_vm.changeLocale}},[_vm._v(" "+_vm._s(locale.toUpperCase())+" ")])}),1)],1)],1)}
var NavBarvue_type_template_id_8e64b3cc_staticRenderFns = []


// CONCATENATED MODULE: ./src/components/NavBar.vue?vue&type=template&id=8e64b3cc&

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.symbol.js
var es_symbol = __webpack_require__("a4d3");

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.array.filter.js
var es_array_filter = __webpack_require__("4de4");

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.array.for-each.js
var es_array_for_each = __webpack_require__("4160");

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.object.get-own-property-descriptor.js
var es_object_get_own_property_descriptor = __webpack_require__("e439");

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.object.get-own-property-descriptors.js
var es_object_get_own_property_descriptors = __webpack_require__("dbb4");

// EXTERNAL MODULE: ./node_modules/core-js/modules/es.object.keys.js
var es_object_keys = __webpack_require__("b64b");

// EXTERNAL MODULE: ./node_modules/core-js/modules/web.dom-collections.for-each.js
var web_dom_collections_for_each = __webpack_require__("159b");

// EXTERNAL MODULE: ./node_modules/@babel/runtime-corejs3/helpers/esm/defineProperty.js
var defineProperty = __webpack_require__("2fa7");

// EXTERNAL MODULE: ./node_modules/vuex/dist/vuex.esm.js
var vuex_esm = __webpack_require__("2f62");

// CONCATENATED MODULE: ./src/store/mutation-types.js
var CHANGE_LOCALE = 'CHANGE_LOCALE';
// CONCATENATED MODULE: ./node_modules/cache-loader/dist/cjs.js??ref--12-0!./node_modules/thread-loader/dist/cjs.js!./node_modules/babel-loader/lib!./node_modules/cache-loader/dist/cjs.js??ref--0-0!./node_modules/vue-loader/lib??vue-loader-options!./src/components/NavBar.vue?vue&type=script&lang=js&









function ownKeys(object, enumerableOnly) { var keys = Object.keys(object); if (Object.getOwnPropertySymbols) { var symbols = Object.getOwnPropertySymbols(object); if (enumerableOnly) symbols = symbols.filter(function (sym) { return Object.getOwnPropertyDescriptor(object, sym).enumerable; }); keys.push.apply(keys, symbols); } return keys; }

function _objectSpread(target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i] != null ? arguments[i] : {}; if (i % 2) { ownKeys(source, true).forEach(function (key) { Object(defineProperty["a" /* default */])(target, key, source[key]); }); } else if (Object.getOwnPropertyDescriptors) { Object.defineProperties(target, Object.getOwnPropertyDescriptors(source)); } else { ownKeys(source).forEach(function (key) { Object.defineProperty(target, key, Object.getOwnPropertyDescriptor(source, key)); }); } } return target; }

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//


/* harmony default export */ var NavBarvue_type_script_lang_js_ = ({
  name: 'NavBar',
  data: function data() {
    return {
      locales: this.$i18n.locales()
    };
  },
  computed: {
    dropDownLabel: function dropDownLabel() {
      return 'Language: ' + this.$i18n.locale().toUpperCase();
    }
  },
  methods: _objectSpread({}, Object(vuex_esm["b" /* mapMutations */])({
    changeLocale: CHANGE_LOCALE
  }))
});
// CONCATENATED MODULE: ./src/components/NavBar.vue?vue&type=script&lang=js&
 /* harmony default export */ var components_NavBarvue_type_script_lang_js_ = (NavBarvue_type_script_lang_js_); 
// EXTERNAL MODULE: ./src/components/NavBar.vue?vue&type=style&index=0&lang=scss&
var NavBarvue_type_style_index_0_lang_scss_ = __webpack_require__("ed6b");

// EXTERNAL MODULE: ./node_modules/vue-loader/lib/runtime/componentNormalizer.js
var componentNormalizer = __webpack_require__("2877");

// CONCATENATED MODULE: ./src/components/NavBar.vue






/* normalize component */

var component = Object(componentNormalizer["a" /* default */])(
  components_NavBarvue_type_script_lang_js_,
  NavBarvue_type_template_id_8e64b3cc_render,
  NavBarvue_type_template_id_8e64b3cc_staticRenderFns,
  false,
  null,
  null,
  null
  
)

/* harmony default export */ var NavBar = (component.exports);
// EXTERNAL MODULE: ./src/scss/main.scss
var main = __webpack_require__("b107");

// CONCATENATED MODULE: ./node_modules/cache-loader/dist/cjs.js??ref--12-0!./node_modules/thread-loader/dist/cjs.js!./node_modules/babel-loader/lib!./node_modules/cache-loader/dist/cjs.js??ref--0-0!./node_modules/vue-loader/lib??vue-loader-options!./src/App.vue?vue&type=script&lang=js&
//
//
//
//
//
//
//
//
//
//
//


/* harmony default export */ var Appvue_type_script_lang_js_ = ({
  name: 'app',
  components: {
    NavBar: NavBar
  },
  created: function created() {
    console.log('$isNw => ', this.$isNw(), this.$nwWindow);
  }
});
// CONCATENATED MODULE: ./src/App.vue?vue&type=script&lang=js&
 /* harmony default export */ var src_Appvue_type_script_lang_js_ = (Appvue_type_script_lang_js_); 
// CONCATENATED MODULE: ./src/App.vue





/* normalize component */

var App_component = Object(componentNormalizer["a" /* default */])(
  src_Appvue_type_script_lang_js_,
  Appvue_type_template_id_d0113f76_render,
  staticRenderFns,
  false,
  null,
  null,
  null
  
)

/* harmony default export */ var App = (App_component.exports);
// EXTERNAL MODULE: ./node_modules/vuex-i18n/dist/vuex-i18n.es.js
var vuex_i18n_es = __webpack_require__("f2f3");

// EXTERNAL MODULE: ./node_modules/vue-router/dist/vue-router.esm.js
var vue_router_esm = __webpack_require__("8c4f");

// CONCATENATED MODULE: ./node_modules/cache-loader/dist/cjs.js?{"cacheDirectory":"node_modules/.cache/vue-loader","cacheIdentifier":"7bdce744-vue-loader-template"}!./node_modules/vue-loader/lib/loaders/templateLoader.js??vue-loader-options!./node_modules/cache-loader/dist/cjs.js??ref--0-0!./node_modules/vue-loader/lib??vue-loader-options!./src/views/Home.vue?vue&type=template&id=0769414e&
var Homevue_type_template_id_0769414e_render = function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('div',{staticClass:"home"},[_c('img',{attrs:{"alt":"Vue.js logo","src":__webpack_require__("cf05")}}),_c('HelloWorld',{attrs:{"msg":_vm.$t('home.welcome')}}),_c('div',{staticClass:"version"},[_vm._v("Version: "+_vm._s(_vm.version))])],1)}
var Homevue_type_template_id_0769414e_staticRenderFns = []


// CONCATENATED MODULE: ./src/views/Home.vue?vue&type=template&id=0769414e&

// CONCATENATED MODULE: ./node_modules/cache-loader/dist/cjs.js?{"cacheDirectory":"node_modules/.cache/vue-loader","cacheIdentifier":"7bdce744-vue-loader-template"}!./node_modules/vue-loader/lib/loaders/templateLoader.js??vue-loader-options!./node_modules/cache-loader/dist/cjs.js??ref--0-0!./node_modules/vue-loader/lib??vue-loader-options!./src/components/HelloWorld.vue?vue&type=template&id=3a19755f&scoped=true&
var HelloWorldvue_type_template_id_3a19755f_scoped_true_render = function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('div',{staticClass:"hello"},[_c('h1',[_vm._v(_vm._s(_vm.msg))]),_vm._m(0),_c('h3',[_vm._v("NW.js")]),_vm._m(1),_c('h3',[_vm._v("Installed CLI Plugins")]),_vm._m(2),_c('h3',[_vm._v("Essential Links")]),_vm._m(3),_c('h3',[_vm._v("Ecosystem")]),_vm._m(4)])}
var HelloWorldvue_type_template_id_3a19755f_scoped_true_staticRenderFns = [function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('p',[_vm._v(" For a guide and recipes on how to configure / customize this project,"),_c('br'),_vm._v(" check out the "),_c('a',{attrs:{"href":"https://cli.vuejs.org","target":"_blank","rel":"noopener"}},[_vm._v("vue-cli documentation")]),_vm._v(". ")])},function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('ul',[_c('li',[_c('a',{attrs:{"href":"http://docs.nwjs.io/en/latest/","target":"_blank","rel":"noopener"}},[_vm._v("Documentation")])])])},function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('ul',[_c('li',[_c('a',{attrs:{"href":"https://github.com/vuejs/vue-cli/tree/dev/packages/%40vue/cli-plugin-babel","target":"_blank","rel":"noopener"}},[_vm._v("babel")])]),_c('li',[_c('a',{attrs:{"href":"https://github.com/vuejs/vue-cli/tree/dev/packages/%40vue/cli-plugin-eslint","target":"_blank","rel":"noopener"}},[_vm._v("eslint")])])])},function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('ul',[_c('li',[_c('a',{attrs:{"href":"https://vuejs.org","target":"_blank","rel":"noopener"}},[_vm._v("Core Docs")])]),_c('li',[_c('a',{attrs:{"href":"https://forum.vuejs.org","target":"_blank","rel":"noopener"}},[_vm._v("Forum")])]),_c('li',[_c('a',{attrs:{"href":"https://chat.vuejs.org","target":"_blank","rel":"noopener"}},[_vm._v("Community Chat")])]),_c('li',[_c('a',{attrs:{"href":"https://twitter.com/vuejs","target":"_blank","rel":"noopener"}},[_vm._v("Twitter")])]),_c('li',[_c('a',{attrs:{"href":"https://news.vuejs.org","target":"_blank","rel":"noopener"}},[_vm._v("News")])])])},function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('ul',[_c('li',[_c('a',{attrs:{"href":"https://router.vuejs.org","target":"_blank","rel":"noopener"}},[_vm._v("vue-router")])]),_c('li',[_c('a',{attrs:{"href":"https://vuex.vuejs.org","target":"_blank","rel":"noopener"}},[_vm._v("vuex")])]),_c('li',[_c('a',{attrs:{"href":"https://github.com/dkfbasel/vuex-i18n","target":"_blank","rel":"noopener"}},[_vm._v("vuex-i18n")])]),_c('li',[_c('a',{attrs:{"href":"https://github.com/vuejs/vue-devtools#vue-devtools","target":"_blank","rel":"noopener"}},[_vm._v("vue-devtools")])]),_c('li',[_c('a',{attrs:{"href":"https://vue-loader.vuejs.org","target":"_blank","rel":"noopener"}},[_vm._v("vue-loader")])]),_c('li',[_c('a',{attrs:{"href":"https://github.com/vuejs/awesome-vue","target":"_blank","rel":"noopener"}},[_vm._v("awesome-vue")])]),_c('li',[_c('a',{attrs:{"href":"https://bootstrap-vue.js.org","target":"_blank","rel":"noopener"}},[_vm._v("bootstrap-vue")])])])}]


// CONCATENATED MODULE: ./src/components/HelloWorld.vue?vue&type=template&id=3a19755f&scoped=true&

// CONCATENATED MODULE: ./node_modules/cache-loader/dist/cjs.js??ref--12-0!./node_modules/thread-loader/dist/cjs.js!./node_modules/babel-loader/lib!./node_modules/cache-loader/dist/cjs.js??ref--0-0!./node_modules/vue-loader/lib??vue-loader-options!./src/components/HelloWorld.vue?vue&type=script&lang=js&
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
/* harmony default export */ var HelloWorldvue_type_script_lang_js_ = ({
  name: 'HelloWorld',
  props: {
    msg: String
  }
});
// CONCATENATED MODULE: ./src/components/HelloWorld.vue?vue&type=script&lang=js&
 /* harmony default export */ var components_HelloWorldvue_type_script_lang_js_ = (HelloWorldvue_type_script_lang_js_); 
// EXTERNAL MODULE: ./src/components/HelloWorld.vue?vue&type=style&index=0&id=3a19755f&scoped=true&lang=css&
var HelloWorldvue_type_style_index_0_id_3a19755f_scoped_true_lang_css_ = __webpack_require__("fc36");

// CONCATENATED MODULE: ./src/components/HelloWorld.vue






/* normalize component */

var HelloWorld_component = Object(componentNormalizer["a" /* default */])(
  components_HelloWorldvue_type_script_lang_js_,
  HelloWorldvue_type_template_id_3a19755f_scoped_true_render,
  HelloWorldvue_type_template_id_3a19755f_scoped_true_staticRenderFns,
  false,
  null,
  "3a19755f",
  null
  
)

/* harmony default export */ var HelloWorld = (HelloWorld_component.exports);
// CONCATENATED MODULE: ./node_modules/cache-loader/dist/cjs.js??ref--12-0!./node_modules/thread-loader/dist/cjs.js!./node_modules/babel-loader/lib!./node_modules/cache-loader/dist/cjs.js??ref--0-0!./node_modules/vue-loader/lib??vue-loader-options!./src/views/Home.vue?vue&type=script&lang=js&
//
//
//
//
//
//
//
//
// @ is an alias to /src

/* harmony default export */ var Homevue_type_script_lang_js_ = ({
  name: 'home',
  components: {
    HelloWorld: HelloWorld
  },
  data: function data() {
    return {
      version: "0.1.7"
    };
  }
});
// CONCATENATED MODULE: ./src/views/Home.vue?vue&type=script&lang=js&
 /* harmony default export */ var views_Homevue_type_script_lang_js_ = (Homevue_type_script_lang_js_); 
// CONCATENATED MODULE: ./src/views/Home.vue





/* normalize component */

var Home_component = Object(componentNormalizer["a" /* default */])(
  views_Homevue_type_script_lang_js_,
  Homevue_type_template_id_0769414e_render,
  Homevue_type_template_id_0769414e_staticRenderFns,
  false,
  null,
  null,
  null
  
)

/* harmony default export */ var Home = (Home_component.exports);
// CONCATENATED MODULE: ./node_modules/cache-loader/dist/cjs.js?{"cacheDirectory":"node_modules/.cache/vue-loader","cacheIdentifier":"7bdce744-vue-loader-template"}!./node_modules/vue-loader/lib/loaders/templateLoader.js??vue-loader-options!./node_modules/cache-loader/dist/cjs.js??ref--0-0!./node_modules/vue-loader/lib??vue-loader-options!./src/views/About.vue?vue&type=template&id=1ae8a7be&
var Aboutvue_type_template_id_1ae8a7be_render = function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _vm._m(0)}
var Aboutvue_type_template_id_1ae8a7be_staticRenderFns = [function () {var _vm=this;var _h=_vm.$createElement;var _c=_vm._self._c||_h;return _c('div',{staticClass:"about"},[_c('h1',[_vm._v("This is an about page")])])}]


// CONCATENATED MODULE: ./src/views/About.vue?vue&type=template&id=1ae8a7be&

// CONCATENATED MODULE: ./src/views/About.vue

var script = {}


/* normalize component */

var About_component = Object(componentNormalizer["a" /* default */])(
  script,
  Aboutvue_type_template_id_1ae8a7be_render,
  Aboutvue_type_template_id_1ae8a7be_staticRenderFns,
  false,
  null,
  null,
  null
  
)

/* harmony default export */ var About = (About_component.exports);
// CONCATENATED MODULE: ./src/router.js




vue_runtime_esm["a" /* default */].use(vue_router_esm["a" /* default */]);
/* harmony default export */ var router = (new vue_router_esm["a" /* default */]({
  mode: 'history',
  base: "/",
  routes: [{
    path: '/',
    name: 'home',
    component: Home
  }, {
    path: '/about',
    name: 'about',
    component: About // route level code-splitting
    // this generates a separate chunk (about.[hash].js) for this route
    // which is lazy-loaded when the route is visited.
    // component: () => import(/* webpackChunkName: "about" */ './views/About.vue')

  }, {
    path: '*',
    redirect: '/'
  }]
}));
// EXTERNAL MODULE: ./node_modules/core-js/modules/es.function.name.js
var es_function_name = __webpack_require__("b0c0");

// EXTERNAL MODULE: ./node_modules/@babel/runtime-corejs3/helpers/esm/typeof.js
var esm_typeof = __webpack_require__("bf2d");

// CONCATENATED MODULE: ./src/store/utils.js


var STORAGE_PREFIX = 'com.hrzg.vue.nw';
/**
 * Local storage helper
 * @type {{fetch: localStorage.fetch, save: localStorage.save}}
 */

var localStorage = {
  fetch: function fetch(key) {
    var value = window.localStorage.getItem(STORAGE_PREFIX + '.' + key);

    try {
      return JSON.parse(value);
    } catch (error) {
      if (error.name === 'SyntaxError') {
        return value;
      } else {
        console.error('localStorage fetch: ', error);
      }
    }
  },
  save: function save(key, value) {
    try {
      var data = Object(esm_typeof["a" /* default */])(value) === 'object' ? JSON.stringify(value) : value;
      window.localStorage.setItem(STORAGE_PREFIX + '.' + key, data);
    } catch (error) {
      console.error('localStorage save: ', error);
    }
  }
};
// EXTERNAL MODULE: ./node_modules/core-js/modules/es.string.trim.js
var es_string_trim = __webpack_require__("498a");

// CONCATENATED MODULE: ./src/store/mutations.js





var mutations = Object(defineProperty["a" /* default */])({}, CHANGE_LOCALE, function (state, event) {
  var locale = event.target.text.trim().toLowerCase();
  localStorage.save('locale', locale);
  this.commit('i18n/SET_LOCALE', {
    locale: locale
  });
});

/* harmony default export */ var store_mutations = (mutations);
// CONCATENATED MODULE: ./src/store/actions.js
var actions = {};
/* harmony default export */ var store_actions = (actions);
// CONCATENATED MODULE: ./src/store/index.js




 // import api from './modules/api'

vue_runtime_esm["a" /* default */].use(vuex_esm["a" /* default */]);
/* harmony default export */ var store = (new vuex_esm["a" /* default */].Store({
  state: {
    language: localStorage.fetch('locale') || 'en'
  },
  mutations: store_mutations,
  actions: store_actions,
  getters: {},
  modules: {// api
  }
}));
// CONCATENATED MODULE: ./src/mixins.js
var mixins = {
  methods: {}
};
/* harmony default export */ var src_mixins = (mixins);
// CONCATENATED MODULE: ./src/plugins/VueNw/nw-shortcuts.js
/* global nw */
var Shortcuts = {
  /**
   * Global nw window instance
   */
  _win: typeof nw !== 'undefined' && "function" !== 'undefined' ? nw.Window.get() : null,

  /**
   * Developer Tools state
   */
  _developerToolsOpen: false,

  /**
   * Shortcut instances
   */
  escSc: null,
  // fsSc: null,
  devToolsSc: null,
  kioskSc: null,

  /**
   * Escape key shortcut options
   */
  escScOpts: {
    key: 'Escape',
    active: function active() {
      if (Shortcuts._win.isKioskMode) {
        return false;
      }

      if (Shortcuts._win.isFullscreen) {
        Shortcuts._win.leaveFullscreen();
      } else {
        Shortcuts._win.close(true);
      }
    },
    failed: function failed(msg) {
      Shortcuts.failed(msg);
    }
  },

  /**
   * Fullscreen mode shortcut options
   */

  /* const fsScOpts = {
    key: 'Ctrl+F',
    active() {
      if(!Shortcuts._win.isKioskMode) {
        Shortcuts.toggleFullscreen()
      }
    },
    failed(msg) {
      Shortcuts.failed(msg)
    }
  }, */

  /**
   * Developer tools shortcut options
   */
  devToolsScOpts: {
    key: 'Ctrl+D',
    active: function active() {
      if (!Shortcuts._win.isKioskMode) {
        Shortcuts.toggleDevTools();
      }
    },
    failed: function failed(msg) {
      Shortcuts.failed(msg);
    }
  },

  /**
   * Kiosk mode shortcut options
   */
  kioskScOpts: {
    key: 'Ctrl+K',
    active: function active() {
      Shortcuts._win.toggleKioskMode();
    },
    failed: function failed(msg) {
      Shortcuts.failed(msg);
    }
  },

  /**
   * Shortcut failed handler
   * @param msg
   */
  failed: function failed(msg) {
    console.warn('Failed', msg);
  },

  /**
   * Toggle Developer Tools
   */
  toggleDevTools: function toggleDevTools() {
    if (this._developerToolsOpen) {
      this._win.closeDevTools();

      this._developerToolsOpen = false;
    } else {
      this._win.showDevTools();

      this._developerToolsOpen = true;
    }
  },

  /**
   * Toggle Fullscreen
   */

  /* this.toggleFullscreen: () => {
    if (this._win.isFullscreen) {
      this._win.leaveFullscreen()
    } else {
      this._win.enterFullscreen()
    }
  } */

  /**
   * Create shortcut
   * @param options
   * @returns {nw.Shortcut}
   */
  createShortcut: function createShortcut(options) {
    return new nw.Shortcut(options);
  },

  /**
   * Register global hot keys
   * Shortcuts:
   * - Escape: leave fullscreen mode or close app
   * - Fullscreen: toggle fullscreen mode
   * - DevTools: toggle dev tools
   * - Kiosk: toggle kiosk mode
   */
  registerGlobalHotKeys: function registerGlobalHotKeys() {
    this.escSc = this.createShortcut(this.escScOpts);
    nw.App.registerGlobalHotKey(this.escSc); // this.fsSc = this.createShortcut(this.fsScOpts)
    // nw.App.registerGlobalHotKey(this.fsScOpts)

    this.devToolsSc = this.createShortcut(this.devToolsScOpts);
    nw.App.registerGlobalHotKey(this.devToolsSc);
    this.kioskSc = this.createShortcut(this.kioskScOpts);
    nw.App.registerGlobalHotKey(this.kioskSc);
  },

  /**
   * Unregister global hot keys
   */
  unregisterGlobalHotKeys: function unregisterGlobalHotKeys() {
    nw.App.unregisterGlobalHotKey(this.escSc); // nw.App.unregisterGlobalHotKey(this.fsSc)

    nw.App.unregisterGlobalHotKey(this.devToolsSc);
    nw.App.unregisterGlobalHotKey(this.kioskSc);
  }
};
/* harmony default export */ var nw_shortcuts = (Shortcuts);
// CONCATENATED MODULE: ./src/plugins/VueNw/nw-menubar.js


/* global nw */
var Menubar = {
  /**
   * Global nw window instance
   */
  _win: typeof nw !== 'undefined' && "function" !== 'undefined' ? nw.Window.get() : null,

  /**
   * Application manifest data
   */
  _manifest: null,

  /**
   * Developer Tools state
   */
  _developerToolsOpen: false,

  /**
   * Initialize application menubar
   */
  init: function init() {
    this._manifest = this.readManifestFile();
  },

  /**
   * Get main menu
   * @returns {nw.Menu}
   */
  menuBar: function menuBar() {
    return new nw.Menu({
      type: 'menubar'
    });
  },

  /**
   * Get menu separator item
   * @returns {nw.MenuItem}
   */
  menuSeparator: function menuSeparator() {
    return new nw.MenuItem({
      type: 'separator'
    });
  },

  /**
   * Get custom menu item
   * @param label
   * @param fn
   * @param key
   * @param modifers
   * @returns {nw.MenuItem}
   */
  menuItem: function menuItem(label, fn, key, modifers) {
    return new nw.MenuItem({
      label: label,
      click: fn,
      key: key,
      modifiers: modifers
    });
  },

  /**
   * Toggle kiosk mode
   */
  toggleKioskMode: function toggleKioskMode() {
    Menubar._win.toggleKioskMode();
  },

  /**
   * Toggle developer tools
   */
  toggleDevTools: function toggleDevTools() {
    if (Menubar._developerToolsOpen) {
      Menubar._developerToolsOpen = false;

      Menubar._win.closeDevTools();
    } else {
      Menubar._developerToolsOpen = true;

      Menubar._win.showDevTools();
    }
  },

  /**
   * Close application
   */
  closeWindow: function closeWindow() {
    Menubar._win.close(true);
  },

  /**
   * Create default menubar
   */
  createDefaultMenubar: function createDefaultMenubar() {
    var menubar = this.menuBar(); // const fs = this.menuItem('Toggle Fullscreen Mode', toggleFullscreen, 'F', 'Ctrl')

    var quit = this.menuItem('Quit', this.closeWindow, 'Q', 'Ctrl'); // Use language from window navigator
    // const preflabel = (window.navigator.language === 'de') ? 'Einstellungen' : 'Preferences'
    // const preferences = this.menuItem(preflabel, openSettings, ',', 'Ctrl')

    var devTools = this.menuItem('Developer Console', this.toggleDevTools, 'D', 'Ctrl');
    var kiosk = this.menuItem('Toggle Kiosk Mode', this.toggleKioskMode, 'K', 'Ctrl');
    var submenu = new nw.Menu(); // submenu.append(fs)

    submenu.append(quit);
    submenu.append(this.menuSeparator()); // submenu.append(preferences)
    // submenu.append(menuSeparator())

    submenu.append(devTools);
    submenu.append(this.menuSeparator());
    submenu.append(kiosk);
    var appMenu = new nw.MenuItem({
      label: this._manifest.name,
      submenu: submenu
    });
    menubar.append(appMenu);
    this._win.menu = menubar;
  },

  /**
   * Create OSX menubar
   */
  createOsxMenubar: function createOsxMenubar() {
    var menubar = this.menuBar();
    menubar.createMacBuiltin(this._manifest.name, {// hideEdit: true
      // hideWindow: true
    }); // Get the root menu from the default osx menu
    // const rootMenu = menubar.items[0].submenu
    // Insert new separator to root menu
    // rootMenu.insert(this.menuSeparator(), 1)
    // Use language from window navigator
    // const preflabel = (window.navigator.language === 'de') ? 'Einstellungen' : 'Preferences'
    // const preferences = this.menuItem(preflabel, openSettings, ',', 'Ctrl')
    // Insert new item to root menu
    // rootMenu.insert(preferences, 2)
    // Insert new separator to root menu
    // rootMenu.insert(this.menuSeparator(), 4)
    // Append "Developer Tools" item

    var devTools = this.menuItem('Developer Tools', this.toggleDevTools, 'D', 'Ctrl');
    menubar.items[2].submenu.append(this.menuSeparator());
    menubar.items[2].submenu.append(devTools);
    menubar.items[2].submenu.insert(this.menuSeparator()); // Insert "Toggle Kiosk Mode" item

    var kiosk = this.menuItem('Toggle Kiosk Mode', this.toggleKioskMode, 'K', 'Ctrl');
    menubar.items[2].submenu.insert(kiosk);
    menubar.items[2].submenu.insert(this.menuSeparator());
    this._win.menu = menubar;
  },

  /**
   * Read manifest / package file
   */
  readManifestFile: function readManifestFile() {
    var _this = this;

    var fs = __webpack_require__("9b0f");

    fs.readFile('package.json', function (error, data) {
      if (error) throw error;
      _this._manifest = JSON.parse(data);

      if (process.platform !== 'darwin') {// this.createDefaultMenubar()
      } else {
        _this.createOsxMenubar();
      }
    });
  }
};
/* harmony default export */ var nw_menubar = (Menubar);
// CONCATENATED MODULE: ./src/plugins/VueNw/index.js


var IS_NW = typeof nw !== 'undefined' && "function" !== 'undefined';
/* global nw */

/* harmony default export */ var VueNw = ({
  install: function install(Vue) {
    /**
     * Global nw window instance
     * @type {{}}
     * @private
     */
    var _win = {};

    if (IS_NW) {
      _win = nw.Window.get();
      Vue.prototype.$nwWindow = _win;
      nw_shortcuts.registerGlobalHotKeys();
      nw_menubar.init();
      /**
       * Global nw window close handler
       * Unregister global HotKeys & close app
       */

      _win.on('close', function () {
        _win.hide();

        nw_shortcuts.unregisterGlobalHotKeys();
        setTimeout(function () {
          _win.close(true);
        }, 500);
      });
    }
    /**
     * Is nw app
     * @returns {boolean}
     */


    Vue.prototype.$isNw = function () {
      return IS_NW;
    };
  }
});
// CONCATENATED MODULE: ./src/main.js












vue_runtime_esm["a" /* default */].use(esm["a" /* default */]);
vue_runtime_esm["a" /* default */].use(vuex_i18n_es["a" /* default */].plugin, store);
vue_runtime_esm["a" /* default */].use(VueNw);
vue_runtime_esm["a" /* default */].i18n.add('en', __webpack_require__("a90a"));
vue_runtime_esm["a" /* default */].i18n.add('de', __webpack_require__("1fa3"));
vue_runtime_esm["a" /* default */].config.productionTip = true;
vue_runtime_esm["a" /* default */].mixin(src_mixins);
new vue_runtime_esm["a" /* default */]({
  router: router,
  store: store,
  created: function created() {
    var language = this.$store.state.language;
    vue_runtime_esm["a" /* default */].i18n.fallback(language);
    vue_runtime_esm["a" /* default */].i18n.set(language);
  },
  render: function render(h) {
    return h(App);
  }
}).$mount('#app');

/***/ }),

/***/ "5f24":
/***/ (function(module, exports, __webpack_require__) {

// extracted by mini-css-extract-plugin

/***/ }),

/***/ "9b0f":
/***/ (function(module, exports) {

module.exports = require("fs");

/***/ }),

/***/ "a90a":
/***/ (function(module) {

module.exports = JSON.parse("{\"home\":{\"welcome\":\"Welcome to Your Vue.js / NW.js App\"}}");

/***/ }),

/***/ "b107":
/***/ (function(module, exports, __webpack_require__) {

// extracted by mini-css-extract-plugin

/***/ }),

/***/ "cf05":
/***/ (function(module, exports, __webpack_require__) {

module.exports = __webpack_require__.p + "img/logo.8263c9c5.png";

/***/ }),

/***/ "d5a8":
/***/ (function(module, exports, __webpack_require__) {

// extracted by mini-css-extract-plugin

/***/ }),

/***/ "ed6b":
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
/* harmony import */ var _node_modules_mini_css_extract_plugin_dist_loader_js_ref_8_oneOf_1_0_node_modules_css_loader_dist_cjs_js_ref_8_oneOf_1_1_node_modules_vue_loader_lib_loaders_stylePostLoader_js_node_modules_postcss_loader_src_index_js_ref_8_oneOf_1_2_node_modules_sass_loader_dist_cjs_js_ref_8_oneOf_1_3_node_modules_cache_loader_dist_cjs_js_ref_0_0_node_modules_vue_loader_lib_index_js_vue_loader_options_NavBar_vue_vue_type_style_index_0_lang_scss___WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__("d5a8");
/* harmony import */ var _node_modules_mini_css_extract_plugin_dist_loader_js_ref_8_oneOf_1_0_node_modules_css_loader_dist_cjs_js_ref_8_oneOf_1_1_node_modules_vue_loader_lib_loaders_stylePostLoader_js_node_modules_postcss_loader_src_index_js_ref_8_oneOf_1_2_node_modules_sass_loader_dist_cjs_js_ref_8_oneOf_1_3_node_modules_cache_loader_dist_cjs_js_ref_0_0_node_modules_vue_loader_lib_index_js_vue_loader_options_NavBar_vue_vue_type_style_index_0_lang_scss___WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_node_modules_mini_css_extract_plugin_dist_loader_js_ref_8_oneOf_1_0_node_modules_css_loader_dist_cjs_js_ref_8_oneOf_1_1_node_modules_vue_loader_lib_loaders_stylePostLoader_js_node_modules_postcss_loader_src_index_js_ref_8_oneOf_1_2_node_modules_sass_loader_dist_cjs_js_ref_8_oneOf_1_3_node_modules_cache_loader_dist_cjs_js_ref_0_0_node_modules_vue_loader_lib_index_js_vue_loader_options_NavBar_vue_vue_type_style_index_0_lang_scss___WEBPACK_IMPORTED_MODULE_0__);
/* unused harmony reexport * */
 /* unused harmony default export */ var _unused_webpack_default_export = (_node_modules_mini_css_extract_plugin_dist_loader_js_ref_8_oneOf_1_0_node_modules_css_loader_dist_cjs_js_ref_8_oneOf_1_1_node_modules_vue_loader_lib_loaders_stylePostLoader_js_node_modules_postcss_loader_src_index_js_ref_8_oneOf_1_2_node_modules_sass_loader_dist_cjs_js_ref_8_oneOf_1_3_node_modules_cache_loader_dist_cjs_js_ref_0_0_node_modules_vue_loader_lib_index_js_vue_loader_options_NavBar_vue_vue_type_style_index_0_lang_scss___WEBPACK_IMPORTED_MODULE_0___default.a); 

/***/ }),

/***/ "fc36":
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
/* harmony import */ var _node_modules_mini_css_extract_plugin_dist_loader_js_ref_6_oneOf_1_0_node_modules_css_loader_dist_cjs_js_ref_6_oneOf_1_1_node_modules_vue_loader_lib_loaders_stylePostLoader_js_node_modules_postcss_loader_src_index_js_ref_6_oneOf_1_2_node_modules_cache_loader_dist_cjs_js_ref_0_0_node_modules_vue_loader_lib_index_js_vue_loader_options_HelloWorld_vue_vue_type_style_index_0_id_3a19755f_scoped_true_lang_css___WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__("5f24");
/* harmony import */ var _node_modules_mini_css_extract_plugin_dist_loader_js_ref_6_oneOf_1_0_node_modules_css_loader_dist_cjs_js_ref_6_oneOf_1_1_node_modules_vue_loader_lib_loaders_stylePostLoader_js_node_modules_postcss_loader_src_index_js_ref_6_oneOf_1_2_node_modules_cache_loader_dist_cjs_js_ref_0_0_node_modules_vue_loader_lib_index_js_vue_loader_options_HelloWorld_vue_vue_type_style_index_0_id_3a19755f_scoped_true_lang_css___WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_node_modules_mini_css_extract_plugin_dist_loader_js_ref_6_oneOf_1_0_node_modules_css_loader_dist_cjs_js_ref_6_oneOf_1_1_node_modules_vue_loader_lib_loaders_stylePostLoader_js_node_modules_postcss_loader_src_index_js_ref_6_oneOf_1_2_node_modules_cache_loader_dist_cjs_js_ref_0_0_node_modules_vue_loader_lib_index_js_vue_loader_options_HelloWorld_vue_vue_type_style_index_0_id_3a19755f_scoped_true_lang_css___WEBPACK_IMPORTED_MODULE_0__);
/* unused harmony reexport * */
 /* unused harmony default export */ var _unused_webpack_default_export = (_node_modules_mini_css_extract_plugin_dist_loader_js_ref_6_oneOf_1_0_node_modules_css_loader_dist_cjs_js_ref_6_oneOf_1_1_node_modules_vue_loader_lib_loaders_stylePostLoader_js_node_modules_postcss_loader_src_index_js_ref_6_oneOf_1_2_node_modules_cache_loader_dist_cjs_js_ref_0_0_node_modules_vue_loader_lib_index_js_vue_loader_options_HelloWorld_vue_vue_type_style_index_0_id_3a19755f_scoped_true_lang_css___WEBPACK_IMPORTED_MODULE_0___default.a); 

/***/ })

/******/ });
//# sourceMappingURL=app.6d98adfb.js.map