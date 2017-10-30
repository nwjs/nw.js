dblite
======
a zero hassle wrapper for sqlite
```javascript
var dblite = require('dblite'),
    db = dblite('file.name');

// Asynchronous, fast, and ...
db.query('SELECT * FROM table', function(err, rows) {
  // ... that easy!
});
```
More in [the related blogpost](http://webreflection.blogspot.com/2013/07/dblite-sqlite3-for-nodejs-made-easy.html) and here too :-)

[![build status](https://secure.travis-ci.org/WebReflection/dblite.png)](http://travis-ci.org/WebReflection/dblite)

[![NPM](https://nodei.co/npm/dblite.png?downloads=true)](https://www.npmjs.com/package/dblite)


### Updates
Version `0.7.5` forces `-noheader` flag if there is no explicit `-header` flag so that no matter what, headers will **not** be used.

This will eventually overwrite the `.sqliterc` but will make the library behavior more consistent across platforms.

Please check [issue 35](https://github.com/WebReflection/dblite/issues/35) to know more.

- - - - - -

Previously, in **sqlite3** version `3.8.6` you need a "_new line agnostic_" version of `dblite`, used in dblite version `0.6.0`.

This **breaks** compatibility with older version of the database cli but this problem should have been fixed in `0.7.0`.

```js
// old version
var dblite = require('dblite');

// 3.8.6 version
var dblite = require('dblite').withSQLite('3.8.6+');

// new version, same as old one
var dblite = require('dblite');

```

It seems that sqlite3 version `3.8.8+` introduced a new line `\n` on Windows machines too so the whole initialization is now performed asynchronously and through features detection.

This should fix the annoying `EOL` problem "_fore`var`_"


### The What And The Why
I've created `dblite` module because there's still not a simple and straight forward or standard way to have [sqlite](http://www.sqlite.org) in [node.js](http://nodejs.org) without requiring to re-compile, re-build, download sources a part or install dependencies instead of simply `apt-get install sqlite3` or `pacman -S sqlite3` in your \*nix system.

`dblite` has been created with portability, simplicity, and reasonable performance for **embedded Hardware** such ARM boards, Raspberry Pi, Arduino Yun, Atmel MIPS CPUs or Linino boards in mind.

Generally speaking all linux based distributions like [Arch Linux](https://www.archlinux.org), where is not always that easy to `node-gyp` a module and add dependencies that work, can now use this battle tested wrap and perform basic to advanced sqlite operations.


### Bootstrap
To install dblite simply `npm install dblite` then in node:
```javascript
var dblite = require('dblite'),
    db = dblite('/folder/to/file.sqlite');

// ready to go, i.e.
db.query('.databases');
db.query(
  'SELECT * FROM users WHERE pass = ?',
  [pass],
  function (err, rows) {
    var user = rows.length && rows[0];
  }
);
```
By default the `dblite` function uses **sqlite3 as executable**. If you need to change the path simply update `dblite.bin = "/usr/local/bin/sqlite3";` before invoking the function.


### API
Right now a created `EventEmitter` `db` instance has 3 extra methods: `.query()`, `.lastRowID()`, and `.close()`.

The `.lastRowID(table, callback(rowid))` helper simplifies a common operation with SQL tables after inserts, handful as shortcut for the following query:
`SELECT ROWID FROM ``table`` ORDER BY ROWID DESC LIMIT 1`.

The method `.close()` does exactly what it suggests: it closes the database connection.
Please note that it is **not possible to perform other operations once it has been closed**.

Being an `EventEmitter` instance, the database variable will be notified with the `close` listener, if any.


### Understanding The .query() Method
The main role in this module is played by the `db.query()` method, a method rich in overloads all with perfect and natural meaning.

The amount of parameters goes from one to four, left to right, where left is the input going through the right which is the eventual output.

All parameters are optionals except the SQL one.

### db.query() Possible Combinations
```javascript
db.query(SQL)
db.query(SQL, callback:Function)
db.query(SQL, params:Array|Object)
db.query(SQL, fields:Array|Object)
db.query(SQL, params:Array|Object, callback:Function)
db.query(SQL, fields:Array|Object, callback:Function)
db.query(SQL, params:Array|Object, fields:Array|Object)
db.query(SQL, params:Array|Object, fields:Array|Object, callback:Function)
```
All above combinations are [tested properly in this file](test/dblite.js) together with many other tests able to make `dblite` robust enough and ready to be used.

Please note how `params` is always before `fields` and/or `callback` if `fields` is missing, just as reminder that order is left to right accordingly with what we are trying to do.

Following detailed explanation per each parameter.

#### The SQL:string
This string [accepts any query understood by SQLite](http://www.sqlite.org/lang.html) plus it accepts all commands that regular SQLite shell would accept such `.databases`, `.tables`, `.show` and all others passing through the specified `info` listener, if any, using just the console as fallback otherwise.
```javascript
var dblite = require('dblite'),
    db = dblite('./db.sqlite');

// will call the implicit `info` console.log
db.query('.show');
/* will console.log something like:

     echo: off
  explain: off
  headers: off
     mode: csv
nullvalue: ""
   output: stdout
separator: ","
    stats: off
    width:
*/

// normal query
db.query('CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, value TEXT)');
db.query('INSERT INTO test VALUES(null, ?)', ['some text']);
db.query('SELECT * FROM test');
// will implicitly log the following
// [ [ '1', 'some text' ] ]
```

##### Warning!
This library heavily relies on strings and it normalizes them through special escaping which aim is to make passed data safe and secure which should be goal #1 of each db oriented API.

Please **do not pass datamanually** like `INSERT INTO table VALUES (null, 'my@email.com')` and always use, specially for any field that contains strings, the provided API: `INSERT INTO table VALUES (null, '@email')` and `{emaiL: 'my@email.com'}`. These kind of operations are described in the following paragraphs.


#### The params:Array|Object
If the SQL string **contains special chars** such `?`, `:key`, `$key`, or `@key` properties, these will be replaced accordingly with the `params` `Array` or `Object` that, in this case, MUST be present.
```javascript
// params as Array
db.query('SELECT * FROM test WHERE id = ?', [1]);

// params as Object
db.query('SELECT * FROM test WHERE id = :id', {id:1});
// same as
db.query('SELECT * FROM test WHERE id = $id', {id:1});
// same as
db.query('SELECT * FROM test WHERE id = @id', {id:1});
```

#### The fields:Array|Object
By default, results are returned as an `Array` where all rows are the outer `Array` and each single row is another `Array`.
```javascript
db.query('SELECT * FROM test');
// will log something like:
[
  [ '1', 'some text' ],     // row1
  [ '2', 'something else' ] // rowN
]
```
If we specify a fields parameter we can have each row represented by an object, instead of an array.
```javascript
// same query using fields as Array
db.query('SELECT * FROM test', ['key', 'value']);
// will log something like:
[
  {key: '1', value: 'some text'},     // row1
  {key: '2', value: 'something else'} // rowN
]
```

#### Parsing Through The fields:Object
[SQLite Datatypes](http://www.sqlite.org/datatype3.html) are different from JavaScript plus SQLite works via affinity.
This module also parses sqlite3 output which is **always a string** and as string every result will always be returned **unless** we specify `fields` parameter as object, suggesting validation per each field.
```javascript
// same query using fields as Object
db.query('SELECT * FROM test', {
  key: Number,
  value: String
});
// note the key as integer!
[
  {key: 1, value: 'some text'},     // row1
  {key: 2, value: 'something else'} // rowN
]
```
More complex validators/transformers can be passed without problems:
```javascript
// same query using fields as Object
db.query('SELECT * FROM `table.users`', {
  id: Number,
  name: String,
  adult: Boolean,
  skills: JSON.parse,
  birthday: Date,
  cube: function (fieldValue) {
    return fieldValue * 3;
  }
});
```

#### The params:Array|Object AND The fields:Array|Object
Not a surprise we can combine both params, using the left to right order input to output so **params first**!
```javascript
// same query using params AND fields
db.query('SELECT * FROM test WHERE id = :id', {
  id: 1
},{
  key: Number,
  value: String
});

// same as...
db.query('SELECT * FROM test WHERE id = ?', [1], ['key', 'value']);
// same as...
db.query('SELECT * FROM test WHERE id = ?', [1], {
  key: Number,
  value: String
});
// same as...
db.query('SELECT * FROM test WHERE id = :id', {
  id: 1
}, [
  'key', 'value'
]);
```

#### The callback:Function
When a `SELECT` or a `PRAGMA` `SQL` is executed the module puts itself in a *waiting for results* state.

**Update** - Starting from `0.4.0` the callback will be invoked with `err` and `data` if the callback length is greater than one. `function(err, data){}` VS `function(data){}`. However, latter mode will keep working in order to not break backward compatibility.
**Update** - Starting from `0.3.3` every other `SQL` statement will invoke the callback after the operation has been completed.

As soon as results are fully pushed to the output the module parses this result, if any, and send it to the specified callback.

The callback is **always the last specified parameter**, if any, or the implicit equivalent of `console.log.bind(console)`.
Latter case is simply helpful to operate directly via `node` **console** and see results without bothering writing a callback each `.query()` call.

#### Extra Bonus: JSON Serialization With fields:Array|Object
If one field value is not scalar (boolean, number, string, null) `JSON.stringify` is performed in order to save data.
This helps lazy developers that don't want to pre parse every field and let `dblite` do the magic.
```javascript
// test has two fields, id and value
db.query('INSERT INTO test VALUES(?, ?)', [
  123,
  {name: 'dblite', rate: 'awesome'} // value serialized
]);

// use the fields to parse back the object
db.query('SELECT * FROM test WHERE id = ?', [123], {
  id: Number,
  value: JSON.parse // value unserialized
}, function (err, rows) {
  var record = rows[0];
  console.log(record.id); // 123
  console.log(record.value.name); // "dblite"
  console.log(record.value.rate); // "awesome""
});
```

### Automatic Fields Through Headers
Since version `0.3.0` it is possible to enable automatic fields parsing either through initialization (suggested) or at runtime.
```javascript
var dblite = require('dblite'),
    // passing extra argument at creation
    db = dblite('file.name', '-header');

db.query('SELECT * FROM table', function(err, rows) {
  rows[0]; // {header0: value0, headerN: valueN}
});

// at runtime
db
  .query('.headers ON')
  .query('SELECT * FROM table', function(err, rows) {
    rows[0]; // {header0: value0, headerN: valueN}
  })
  .query('.headers OFF')
;
```

In version `0.3.2` a smarter approach for combined _headers/fields_ is used where the right key order is granted by headers but it's possible to validate known fields too.

```javascript
var db = require('dblite')('file.name', '-header');

db.query('SELECT 1 as one, 2 as two', {two:Number}, function(err, rows) {
  rows[0]; // {one: "1", two: 2} // note "1" as String
});
```
In this way these two options can be supplementary when and if necessary.


### Handling Infos And Errors - Listeners
The `EventEmitter` will notify any listener attached to `info`, `error`, or `close` accordingly with the current status.
```javascript
db.on('info', function (data) {
  // show data returned by special syntax
  // such: .databases .tables .show and others
  console.log(data);
  // by default, it does the same
});

db.on('error', function (err) {
  // same as `info` but for errors
  console.error(err.toString());
  // by default, it does the same
});

db.on('close', function (code) {
  // by default, it logs "bye bye"
  // invoked once the database has been closed
  // and every statement in the queue executed
  // the code is the exit code returned via SQLite3
  // usually 0 if everything was OK
  console.log('safe to get out of here ^_^_');
});
```
Please **note** that error is invoked only if the callback is not handling it already via double argument.

The `close` event ensures that all operations have been successfully performed and your app is ready to exit or move next.

Please note that after invoking `db.close()` any other query will be ignored and the instance will be put in a _waiting to complete_ state which will invoke the `close` listener once operations have been completed.


### Raspberry Pi Performance
This is the output generated after a `make test` call in this repo folder within Arch Linux for RPi.
```
npm test

> dblite@0.1.2 test /home/dblite
> node test/.test.js

/home/dblite/dblite.test.sqlite
------------------------------
main
passes: 1, fails: 0, errors: 0
------------------------------
create table if not exists
passes: 1, fails: 0, errors: 0
------------------------------
100 sequential inserts
100 records in 3.067 seconds
passes: 1, fails: 0, errors: 0
------------------------------
1 transaction with 100 inserts
200 records in 0.178 seconds
passes: 1, fails: 0, errors: 0
------------------------------
auto escape
passes: 1, fails: 0, errors: 0
------------------------------
auto field
fetched 201 rows as objects in 0.029 seconds
passes: 1, fails: 0, errors: 0
------------------------------
auto parsing field
fetched 201 rows as normalized objects in 0.038 seconds
passes: 1, fails: 0, errors: 0
------------------------------
many selects at once
different selects in 0.608 seconds
passes: 1, fails: 0, errors: 0
------------------------------
db.query() arguments
[ [ '1' ] ]
[ [ '2' ] ]
[ { id: 1 } ]
[ { id: 2 } ]
passes: 5, fails: 0, errors: 0
------------------------------
utf-8
¥ · £ · € · $ · ¢ · ₡ · ₢ · ₣ · ₤ · ₥ · ₦ · ₧ · ₨ · ₩ · ₪ · ₫ · ₭ · ₮ · ₯ · ₹
passes: 1, fails: 0, errors: 0
------------------------------
erease file
passes: 1, fails: 0, errors: 0

------------------------------
          15 Passes
------------------------------
```
If an SD card can do this good, I guess any other environment should not have problems here ;-)

### F.A.Q.
Here a list of probably common Q&A about this module. Please do not hesitate to ask me more, if necessary, thanks.

  * **How Does It Work?** `dblite` uses a [spawned](http://nodejs.org/api/child_process.html#child_process_child_process_spawn_command_args_options) version of the `sqlite3` executable. It could theoretically work with any other SQL like database but it's tested with `sqlite3-shell` only
  * **Does It Spawn Per Each Query?** this is a quick one: **NO**! `dblite` spawns once per each database file where usually there is only one database file opened per time.
  * **How About Memory And Performance?** Accordingly with `node` manual:
  
    > These child Nodes are still whole new instances of V8.
    > Assume at least 30ms startup and 10mb memory for each new Node.
    > That is, you cannot create many thousands of them.
  
    Since `dblite` spawns only once, there is a little overhead during the database initialization but that's pretty much it, the amount of RAM increases with the amount of data we save or retrieve from the database. The above **Raspberry Pi Benchmark** should ensure that with most common operation, and using transactions where possible, latency and RAM aren't a real issue.
  * **Why Not The Native One?** I had some difficulty installing this [node-sqlite3 module](https://github.com/developmentseed/node-sqlite3#name) due `node-gyp` incompatibilities with some **ARM** based device in both *Debian* and *ArchLinux*. Since I really needed an sqlite manager for the next version of [polpetta](https://github.com/WebReflection/polpetta#က-polpetta) which aim is to have a complete, lightweight, and super fast web server in many embedded hardware such RPi, Cubieboard, and others, and since I needed something able to work with multiple core too, I've decided to try this road wrapping the native, easy to install and update, `sqlite3` shell client and do everything I need. So far, so good I would say ;-)
  * **Isn't `params` and `fields` an ambiguous choice?** At the very beginning I wasn't sure myself if that would have worked as API choice but later on I've changed my mind. First of all, it's very easy to spot special chars in the `SQL` statement. If present, params is mandatory and used, as easy as that. Secondly, if an object has functions as value, it's obviously a `fields` object, 'cause `params` cannot contains functions since these are not compatible with `JSON` serialization, neither meaningful for the database. The only case where `fields` might be confused with `params` is when no `params` has been specified, and `fields` is an `Array`. In this case I believe you are the same one that wrote the SQL too and know upfront if there are fields to retrieve from `params` or not so this is actually a *non real-world* problem and as soon as you try this API you'll realize it feels intuitive and right.
  * **Are Transactions Supported?** ... **YES**, transactions are supported simply performing multiple queries as you would do in *sqlite3* shell:
  ```javascript
    db.query('BEGIN TRANSACTION');
    for(var i = 0; i < 100; i++) {
      db.query('INSERT INTO table VALUES(?, ?)', [null, Math.random()]);
    }
    db.query('COMMIT');
  ```
  The test file has a transaction with 100 records in it, [have a look](test/dblite.js).
  * **Can I Connect To A `:memory:` Database?** well, you can do anything you would do with `sqlite3` shell so **YES**
  ```javascript
  var db = dblite(':memory:'); // that's it!
  ```

### License
The usual Mit Style, thinking about the [WTFPL](http://en.wikipedia.org/wiki/WTFPL) though ... stay tuned for updates.

    Copyright (C) 2013 by WebReflection

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
