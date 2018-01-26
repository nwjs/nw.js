# EventEmitter3

[![Version npm](https://img.shields.io/npm/v/eventemitter3.svg?style=flat-square)](http://browsenpm.org/package/eventemitter3)[![Build Status](https://img.shields.io/travis/primus/eventemitter3/master.svg?style=flat-square)](https://travis-ci.org/primus/eventemitter3)[![Dependencies](https://img.shields.io/david/primus/eventemitter3.svg?style=flat-square)](https://david-dm.org/primus/eventemitter3)[![Coverage Status](https://img.shields.io/coveralls/primus/eventemitter3/master.svg?style=flat-square)](https://coveralls.io/r/primus/eventemitter3?branch=master)[![IRC channel](https://img.shields.io/badge/IRC-irc.freenode.net%23primus-00a8ff.svg?style=flat-square)](https://webchat.freenode.net/?channels=primus)

[![Sauce Test Status](https://saucelabs.com/browser-matrix/eventemitter3.svg)](https://saucelabs.com/u/eventemitter3)

EventEmitter3 is a high performance EventEmitter. It has been micro-optimized
for various of code paths making this, one of, if not the fastest EventEmitter
available for Node.js and browsers. The module is API compatible with the
EventEmitter that ships by default with Node.js but there are some slight
differences:

- Domain support has been removed.
- We do not `throw` an error when you emit an `error` event and nobody is
  listening.
- The `newListener` event is removed as the use-cases for this functionality are
  really just edge cases.
- No `setMaxListeners` and it's pointless memory leak warnings. If you want to
  add `end` listeners you should be able to do that without modules complaining.
- No `listenerCount` function. Use `EE.listeners(event).length` instead.
- Support for custom context for events so there is no need to use `fn.bind`.
- `listeners` method can do existence checking instead of returning only arrays.

It's a drop in replacement for existing EventEmitters, but just faster. Free
performance, who wouldn't want that? The EventEmitter is written in EcmaScript 3
so it will work in the oldest browsers and node versions that you need to
support.

## Installation

```bash
$ npm install --save eventemitter3        # npm
$ component install primus/eventemitter3  # Component
$ bower install eventemitter3             # Bower
```

## Usage

After installation the only thing you need to do is require the module:

```js
var EventEmitter = require('eventemitter3');
```

And you're ready to create your own EventEmitter instances. For the API
documentation, please follow the official Node.js documentation:

http://nodejs.org/api/events.html

### Contextual emits

We've upgraded the API of the `EventEmitter.on`, `EventEmitter.once` and
`EventEmitter.removeListener` to accept an extra argument which is the `context`
or `this` value that should be set for the emitted events. This means you no
longer have the overhead of an event that required `fn.bind` in order to get a
custom `this` value.

```js
var EE = new EventEmitter()
  , context = { foo: 'bar' };

function emitted() {
  console.log(this === context); // true
}

EE.once('event-name', emitted, context);
EE.on('another-event', emitted, context);
EE.removeListener('another-event', emitted, context);
```

### Existence

To check if there is already a listener for a given event you can supply the
`listeners` method with an extra boolean argument. This will transform the
output from an array, to a boolean value which indicates if there are listeners
in place for the given event:

```js
var EE = new EventEmitter();
EE.once('event-name', function () {});
EE.on('another-event', function () {});

EE.listeners('event-name', true); // returns true
EE.listeners('unknown-name', true); // returns false
```

## License

[MIT](LICENSE)
