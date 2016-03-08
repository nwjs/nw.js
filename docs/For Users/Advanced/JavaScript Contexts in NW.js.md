# JavaScript Contexts in NW.js {: .doctitle}
---

[TOC]

## Concept of JavaScript Context

Scripts running in different windows live in different JavaScript contexts, i.e. each window has its own global object and its own set of global constructors (such as `Array` or `Object`).

That's common practice among web browsers and it's a good thing because, for example:

* when an object's prototype is replaced or augmented by some library (such as [Prototype](http://prototypejs.org/)) or a simpler script, the analogous objects in other windows are unaffected nevertheless;
* when a programmer makes a mistake (such as [missing `new` before a poorly written constructor](http://ejohn.org/blog/simple-class-instantiation/)) and the bug affects (pollutes) the global scope, it still cannot affect larger areas (several windows);
* malicious applications cannot access confidential data structures in other windows.

When a script accessing to an object / function defined in another context, JS engine will temporarily enter the target context and leave it once finished. 

## Separate Context Mode

Besides the contexts created by browsers, NW.js introduced additional Node context for running Node modules by default. So NW.js has two types of JavaScript contexts: **Browser Context** and **Node Context**.

!!! note "Web Worker"
    Actually Web Workers are running in a separate JavaScript context which is neither browser context nor node context. But you can't access Web or Node.js or NW.js APIs in Web Worker's context.

### Browser Context

#### Load Script in Browser Context

Scripts loaded or embedded by traditional web ways, such as using `<script>` element or jQuery's [`$.getScript()`](http://api.jquery.com/jQuery.getScript/) or [RequireJS](http://requirejs.org/), are running in browser context.

#### Global Objects in Browser Context

In browser context, there are some global objects including [JS builtin objects](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects) (such as `Date` or `Error` or `TypedArray`) and [Web API](https://developer.mozilla.org/en-US/docs/Web/Reference/API) (such as DOM API).

#### Create New Browser Context

Different windows and frames have different contexts. So when you create a new frame or window, you will get a new browser context.

#### Access Node.js and NW.js API in Browser Context

Some objects of Node context are copied to Browser context so that scripts running in Browser context can access Node.js objects:

* `nw` -- the object of all [NW.js APIs](../../References)
* `global` -- the global object of Node Context; same as `nw.global`
* `require` -- the `require()` function for loading Node.js modules; similar to `nw.require()`, but it also supports `require('nw.gui')` to load NW.js API module.
* `process` -- the [process module](https://nodejs.org/api/globals.html#globals_process) of Node.js; same as `nw.process`
* `Buffer` -- the [Buffer class](https://nodejs.org/api/globals.html#globals_class_buffer) of Node.js

#### Relative Path Resolving of `require()` in Browser Context

Relative paths in Browser context are resolved according to path of main HTML file (like all browsers do).

### Node Context

#### Load Script in Node Context
Scripts loaded with following ways are running in Node context:

* Scripts loaded by `require()` of Node.js API
* Scripts loaded by [`node-main` in Manifest file](../../References/Manifest Format.md#node-main)

#### Global Objects in Node Context
Scripts running in the Node context can use [JS builtin objects]() like browser context. In addition, you can also use [global objects defined by Node.js](https://nodejs.org/api/globals.html), such as `__dirname`, `process`, `Buffer` etc.

!!! note
    Web APIs are not available in Node context. See [Access Browser and NW.js API in Node Context](#access-browser-and-nwjs-api-in-node-context) below to find out how to use them.

#### Create New Node Context
**All node modules shares a same Node context in separate context mode**. But you have several ways to create new Node contexts:

* Set `new_instance` option to `true` when creating window with [`Window.open()`](../../References/Window.md#windowopenurl-options-callback)
* Start NW.js with `--mixed-context` CLI option to turn NW.js into [Mixed Context mode](#mixed-context)

#### Access Browser and NW.js API in Node Context

In Node context, there are no browser side or NW.js APIs, such as `alert()` or `document.*` or `nw.Clipboard` etc. To access browser APIs, you have to pass the corresponding objects, such as `window` object, to functions in Node context.

See following example for how to achive this.

Following script are running in Node context (myscript.js):
```javascript
// `el` should be passed from browser context
exports.setText = function(el) {
    el.innerHTML = 'hello';
};
```

In the browser side (index.html):
```html
<div id="el"></div>
<script>
var myscript = require('./myscript');
// pass the `el` element to the Node function
myscript.setText(document.getElementbyId('el'));
// you will see "hello" in the element
</script>
```

!!! note "`window` in Node Context"
    There is a `window` object in Node context pointing to the main window as set in `main` field of Manifest file.

#### Relative Paths Resolving of `require()` in Node Context

Relative paths in node modules are resolved according to path of that module (like Node.js always do).

## Mixed Context Mode

Mixed context is introduced in NW.js 0.13. When running NW.js with [`--mixed-context` CLI option](../../References/Command Line Options.md#mixedcontext), a new Node context is created at the time of each browser context creation and running in a same context as browser context, a.k.a. the Mixed context.

### Load Script in Mixed Context Mode

To enable Mixed context, add `--mixed-context` when starting NW.js or add it to [`chromium-args` in Manifest file](../../References/Manifest Format.md#chromium-args).

Any scripts loaded using web ways or `require()` in Node.js are running in a same context.

### Global Objects in Mixed Context

In Mixed context, you can use all browser and NW.js API in Node modules, and vice versa.

`package.json`
```javascript
{
    "name": "test-context",
    "main": "index.html",
    "chromium-args": "--mixed-context"
}
```

`myscript.js`
```javascript
exports.createDate = function() {
    return new Date();
};

exports.showAlert = function() {
    alert("I'm running in Node module!");
};
```

Then following comparison will success in Mixed context:
`index.html`
```html
<script>
var myscript = require('./myscript');

console.log(myscript.createDate() instanceof Date); // true
myscript.showAlert(); // I'm running in Node module!
</script>
```

### Comparing with Separate Context

The advantage of MultiContext mode is that you will not encounter many [type checking issue](#working-with-multiple-contexts) as below.

The cons is that in Mixed context mode, you can't share variable easily as before. To share variables among contexts, you should put variables in a common context that can be accessed from the contexts you want to share with. Or you can use [`window.postMessage()` API](https://developer.mozilla.org/en-US/docs/Web/API/Window/postMessage) to send and receive messages between contexts.

## Working with Multiple Contexts

While differences of contexts are generally benefitial, sometimes they may constitute a problem in your (or some other person's) code, and a need for a workaround arises.

For example, in different browser contexts, the global objects are not identical and some type checking tricks will fail with multiple contexts.

```html
<iframe id="myframe" src="myframe.html"></iframe>
<script>
// `window` is the global object of current browser context
// `myframe.contentWindow` is the global object of the `<iframe>`'s browser context
var currentContext = window;
var iframeContext = document.getElementById('myframe').contentWindow;

// `myfunc` is defined in current context
function myfunc() {

}

console.log(currentContext.Date === iframeContext.Date); // false
console.log(currentContext.Function === iframeContext.Function); // false
console.log(myfunc instanceof currentContext.Function); // true
console.log(myfunc instanceof iframeContext.Function); // false
console.log(myfunc.constructor === currentContext.Function); // true
console.log(myfunc.constructor === iframeContext.Function); // false
</script>
```

### Problem with `instanceOf`

The most common cause for such problems is the behaviour of the `instanceof` operator in JavaScript. As you may [see in MDN](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/instanceof), the operation `someValue instanceof someConstructor` tests whether an object has in its prototype chain the `prototype` property of the given constructor. However, if `someValue` is passed from a different JavaScript context, then it has its own line of ancestor objects, and the `someValue instanceof someConstructor` check fails inevitably.

For example, a simple check `someValue instanceof Array` cannot determine if a variable's value is an array's if it's passed from another context (see [Determining with absolute accuracy whether or not a JavaScript object is an array](http://web.mit.edu/jwalden/www/isArray.html) for details).

### Problem with `obj.constructor`

The same problem arises when the `obj.constructor` property is checked directly (for example, when `someValue.constructor === Array` is used instead of `someValue instanceof Array`).

### Problem of `obj.__proto__`

The legacy `obj.__proto__` gives you access to the prototype of that object directly. Comparing it's constructor with global object or use `instanceof` as above will still lead to the wrong result.

### Problems in 3rd-Party Library

3rd-party libraries may use problematic ways of type checking listed above. That will cause misterious errors. Once it happens, it should be a bug of 3rd-party library. You are recommended to report a bug for the library or fix it your own.

### Reliable Way of Type Checking Across Contexts

A way to prevent context-related problems is to **avoid using `instanceof`** when a value may come from another JavaScript context.

You may use [`Array.isArray`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/isArray) method to check whether a value is an array, and that method works reliably across contexts.

For testing if `someValue` is an object of other context dependent globals, like `Function` or `Date` etc., you may use following tricks to test the actual types:

```javascript
// test a function
Object.prototype.toString.apply(someValue) === "[object Function]"
// test a Date
Object.prototype.toString.apply(someValue) === "[object Date]"
```

However, if such a convenient alternate method is not readily available, or when you face a problem in someone other's (not your own) code and patching that would need a hassle, then another workaround is necessary.

Also you can use [`nwglobal`](https://github.com/Mithgol/nwglobal), which returns the global objects in Node context, to workaround the type checking in some cases.
