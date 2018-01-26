# node-portfinder [![Build Status](https://api.travis-ci.org/indexzero/node-portfinder.svg)](https://travis-ci.org/indexzero/node-portfinder)

## Installation

``` bash
  $ [sudo] npm install portfinder
```

## Usage
The `portfinder` module has a simple interface:

``` js
  var portfinder = require('portfinder');

  portfinder.getPort(function (err, port) {
    //
    // `port` is guaranteed to be a free port
    // in this scope.
    //
  });
```

Or with promise (if Promise are supported) :

``` js
  const portfinder = require('portfinder');

  portfinder.getPortPromise()
    .then((port) => {
        //
        // `port` is guaranteed to be a free port
        // in this scope.
        //
    })
    .catch((err) => {
        //
        // Could not get a free port, `err` contains the reason.
        //
    });
```

If `portfinder.getPortPromise()` is called on a Node version without Promise (<4), it will throw an Error unless [Bluebird](http://bluebirdjs.com/docs/getting-started.html) or any Promise pollyfill is used.

By default `portfinder` will start searching from `8000`. To change this simply set `portfinder.basePort`.

## Run Tests
``` bash
  $ npm test
```

#### Author: [Charlie Robbins][0]
#### Maintainer: [Erik Trom][1]
#### License: MIT/X11
[0]: http://nodejitsu.com
[1]: https://github.com/eriktrom
