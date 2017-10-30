/*!
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

*/
/*!
Copyright (C) 2015 by WebReflection

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

*/
/*! a zero hassle wrapper for sqlite by Andrea Giammarchi !*/
var
  isArray = Array.isArray,
  // used to generate unique "end of the query" identifiers
  crypto = require('crypto'),
  // relative, absolute, and db paths are normalized anyway
  path = require('path'),
  // each dblite(fileName) instance is an EventEmitter
  EventEmitter = require('events').EventEmitter,
  // used to perform some fallback
  WIN32 = process.platform === 'win32',
  // what kind of Path Separator we have here ?
  PATH_SEP = path.sep || (
    WIN32 ? '\\' : '/'
  ),
  // each dblite instance spawns a process once
  // and interact with that shell for the whole session
  // one spawn per database and no more (on avg 1 db is it)
  spawn = require('child_process').spawn,
  // use to re-generate Date objects
  DECIMAL = /^[1-9][0-9]*$/,
  // verify if it's a select or not
  SELECT = /^(?:select|SELECT|pragma|PRAGMA) /,
  // for simple query replacements: WHERE field = ?
  REPLACE_QUESTIONMARKS = /\?/g,
  // for named replacements: WHERE field = :data
  REPLACE_PARAMS = /(?:\:|\@|\$)([a-zA-Z0-9_$]+)/g,
  // the way CSV threats double quotes
  DOUBLE_DOUBLE_QUOTES = /""/g,
  // to escape strings
  SINGLE_QUOTES = /'/g,
  // to use same escaping logic for double quotes
  // except it makes escaping easier for JSON data
  // which usually is full of "
  SINGLE_QUOTES_DOUBLED = "''",
  // to verify there are named fields/parametes
  HAS_PARAMS = /(?:\?|(?:(?:\:|\@|\$)[a-zA-Z0-9_$]+))/,
  // shortcut used as deafault notifier
  log = console.log.bind(console),
  // the default binary as array of paths
  bin = ['sqlite3'],
  // private shared variables
  // avoid creation of N functions
  // keeps memory low and improves performance
  paramsIndex,  // which index is the current
  paramsArray,  // which value when Array
  paramsObject, // which value when Object (named parameters)
  IS_NODE_06 = false, // dirty things to do there ...
  // defned later on
  EOL, EOL_LENGTH,
  SANITIZER, SANITIZER_REPLACER,
  waitForEOLToBeDefined = [],
  createProgram = function () {
    for (var args = [], i = 0; i < arguments.length; i++) {
      args[i] = arguments[i];
    }
    return spawn(
      // executable only, folder needs to be specified a part
      bin.length === 1 ? bin[0] : ('.' + PATH_SEP + bin[bin.length - 1]),
      // normalize file path if not :memory:
      normalizeFirstArgument(
        // it is possible to eventually send extra sqlite3 args
        // so all arguments are passed
        args
      ).concat('-csv') // but the output MUST be csv
       .reverse(),     // see https://github.com/WebReflection/dblite/pull/12
      // be sure the dir is the right one
      {
        // the right folder is important or sqlite3 won't work
        cwd: bin.slice(0, -1).join(PATH_SEP) || process.cwd(),
        env: process.env, // same env is OK
        encoding: 'utf8', // utf8 is OK
        detached: true,   // asynchronous
        stdio: ['pipe', 'pipe', 'pipe'] // handled here
      }
    );
  },
  defineCSVEOL = function (fn) {

    if (waitForEOLToBeDefined.push(fn) === 1) {

      var
        program = createProgram(':memory:', '-noheader'),
        ondata = function (data) {
          setUpAndGo(data.toString().slice(1));
        },
        // save the Mac, SAVE THE MAC!!!
        // error could occur due \r\n
        // so clearly that won't be the right EOL
        onerror = function () {
          setUpAndGo('\n');
        },
        setUpAndGo = function (eol) {

          // habemus EOL \o/
          EOL = eol;

          // what's EOL length? Used to properly parse data
          EOL_LENGTH = EOL.length;

          // makes EOL safe for strings passed to the shell
          SANITIZER = new RegExp("[;" + EOL.split('').map(function(c) {
            return '\\x' + ('0' + c.charCodeAt(0).toString(16)).slice(-2);
          }).join('') + "]+$");

          // used to mark the end of each line passed to the shell
          SANITIZER_REPLACER = ';' + EOL;

          // once closed, reassign this helper
          // and trigger all queued functions
          program.on('close', function () {
            defineCSVEOL = function (fn) { fn(); };
            waitForEOLToBeDefined
              .splice(0, waitForEOLToBeDefined.length)
              .forEach(defineCSVEOL);
          });

          program.kill();
        }
      ;

      program.stdout.on('data', ondata);
      program.stdin.on('error', onerror);
      program.stdout.on('error', onerror);
      program.stderr.on('error', onerror);

      program.stdin.write('SELECT 1;\r\n');

    }

  }
;

/**
 * var db = dblite('filename.sqlite'):EventEmitter;
 *
 * db.query(              thismethod has **many** overloads where almost everything is optional
 *
 *  SQL:string,           only necessary field. Accepts a query or a command such `.databases`
 *
 *  params:Array|Object,  optional, if specified replaces SQL parts with this object
 *                        db.query('INSERT INTO table VALUES(?, ?)', [null, 'content']);
 *                        db.query('INSERT INTO table VALUES(:id, :value)', {id:null, value:'content'});
 *
 *  fields:Array|Object,  optional, if specified is used to normalize the query result with named fields.
 *                        db.query('SELECT table.a, table.other FROM table', ['a', 'b']);
 *                        [{a:'first value', b:'second'},{a:'row2 value', b:'row2'}]
 *
 *                        
 *                        db.query('SELECT table.a, table.other FROM table', ['a', 'b']);
 *                        [{a:'first value', b:'second'},{a:'row2 value', b:'row2'}]
 *  callback:Function
 * );
 */
function dblite() {
  var
    args = arguments,
    // the current dblite "instance"
    self = new EventEmitter(),
    // the incrementally concatenated buffer
    // cleaned up as soon as the current command has been completed
    selectResult = '',
    // sqlite3 shell can produce one output per time
    // every operation performed through this wrapper
    // should not bother the program until next
    // available slot. This queue helps keeping
    // requests ordered without stressing the system
    // once things will be ready, callbacks will be notified
    // accordingly. As simple as that ^_^
    queue = [],
    // set as true only once db.close() has been called
    notWorking = false,
    // marks the shell busy or not
    // initially we need to wait for the EOL
    // so it's busy by default
    busy = true,
    // tells if current output needs to be processed
    wasSelect = false,
    wasNotSelect = false,
    wasError = false,
    // forces the output not to be processed
    // might be handy in some case where it's passed around
    // as string instread of needing to serialize/unserialize
    // the list of already arrays or objects
    dontParseCSV = false,
    // one callback per time will be notified
    $callback,
    // recycled variable for fields operation
    $fields,
    // this will be the delimiter of each sqlite3 shell command
    SUPER_SECRET,
    SUPER_SECRET_SELECT,
    SUPER_SECRET_LENGTH,
    // the spawned program
    program
  ;

  defineCSVEOL(function () {

    // this is the delimiter of each sqlite3 shell command
    SUPER_SECRET =  '---' +
                    crypto.randomBytes(64).toString('base64') +
                    '---';
    // ... I wish .print was introduced before SQLite 3.7.10 ...
    // this is a weird way to get rid of the header, if enabled
    SUPER_SECRET_SELECT = '"' + SUPER_SECRET + '" AS "' + SUPER_SECRET + '";' + EOL;
    // used to check the end of a buffer
    SUPER_SECRET_LENGTH = -(SUPER_SECRET.length + EOL_LENGTH);
    // add the EOL to the secret
    SUPER_SECRET += EOL;

    // define the spawn program
    program = createProgram.apply(null, args);

    // and all its IO handled here
    program.stderr.on('data', onerror);
    program.stdin.on('error', onerror);
    program.stdout.on('error', onerror);
    program.stderr.on('error', onerror);

    // invoked each time the sqlite3 shell produces an output
    program.stdout.on('data', function (data) {
      /*jshint eqnull: true*/
      // big output might require more than a call
      var str, result, callback, fields, headers, wasSelectLocal, rows, dpcsv;
      if (wasError) {
        selectResult = '';
        wasError = false;
        if (self.ignoreErrors) {
          busy = false;
          next();
        }
        return;
      }
      // the whole output is converted into a string here
      selectResult += data;
      // if the end of the output is the serapator
      if (selectResult.slice(SUPER_SECRET_LENGTH) === SUPER_SECRET) {
        // time to move forward since sqlite3 has done
        str = selectResult.slice(0, SUPER_SECRET_LENGTH);
        // drop the secret header if present
        headers = str.slice(SUPER_SECRET_LENGTH) === SUPER_SECRET;
        if (headers) str = str.slice(0, SUPER_SECRET_LENGTH);
        // clean up the outer variabls
        selectResult = '';
        // makes the spawned program not busy anymore
        busy = false;
        // if it was a select
        if (wasSelect || wasNotSelect) {
          wasSelectLocal = wasSelect;
          dpcsv = dontParseCSV;
          // set as false all conditions
          // only here dontParseCSV could have been true
          // set to false that too
          wasSelect = wasNotSelect = dontParseCSV = busy;
          // which callback should be invoked?
          // last expected one for this round
          callback = $callback;
          // same as fields
          fields = $fields;
          // parse only if it was a select/pragma
          if (wasSelectLocal) {
            // unless specified, process the string
            // converting the CSV into an Array of rows
            result = dpcsv ? str : parseCSV(str);
            // if there were headers/fields and we have a result ...
            if (headers && isArray(result) && result.length) {
              //  ... and fields is not defined
              if (fields == null) {
                // fields is the row 0
                fields = result[0];
              } else if(!isArray(fields)) {
                // per each non present key, enrich the fields object
                // it is then possible to have automatic headers
                // with some known field validated/parsed
                // e.g. {id:Number} will be {id:Number, value:String}
                // if the query was SELECT id, value FROM table
                // and the fields object was just {id:Number}
                // but headers were active
                result[0].forEach(enrichFields, fields);
              }
              // drop the first row with headers
              result.shift();
            }
          }
          // but next query, should not have
          // previously set callbacks or fields
          $callback = $fields = null;
          // the spawned program can start a new job without current callback
          // being able to push another job as soon as executed. This makes
          // the queue fair for everyone without granting priority to anyone.
          next();
          // if there was actually a callback to call
          if (callback) {
            rows = fields ? (
                // and if there was a need to parse each row
                isArray(fields) ?
                  // as object with properties
                  result.map(row2object, fields) :
                  // or object with validated properties
                  result.map(row2parsed, parseFields(fields))
              ) :
              // go for it ... otherwise returns the result as it is:
              // an Array of Arrays
              result
            ;
            // if there was an error signature
            if (1 < callback.length) {
              callback.call(self, null, rows);
            } else {
              // invoke it with the db object as context
              callback.call(self, rows);
            }
          }
        } else {
          // not a select, just a special command
          // such .databases or .tables
          next();
          // if there is something to notify
          if (str.length) {
            // and if there was an 'info' listener
            if (self.listeners('info').length) {
              // notify
              self.emit('info', EOL + str);
            } else {
              // otherwise log
              log(EOL + str);
            }
          }
        }
      }
    });

    // detach the program from this one
    // node 0.6 has not unref
    if (program.unref) {
      program.on('close', close);
      program.unref();
    } else {
      IS_NODE_06 = true;
      program.stdout.on('close', close);
    }

    // let's begin
    busy = false;
    next();

  });

  // when program is killed or closed for some reason
  // the dblite object needs to be notified too
  function close(code) {
    if (self.listeners('close').length) {
      self.emit('close', code);
    } else {
      log('bye bye');
    }
  }

  // as long as there's something else to do ...
  function next() {
    if (queue.length) {
      // ... do that and wait for next check
      self.query.apply(self, queue.shift());
    }
  }

  // common error helper
  function onerror(data) {
    if($callback && 1 < $callback.length) {  
      // there is a callback waiting
      // and there is more than an argument in there
      // the callback is waiting for errors too
      var callback = $callback;
      wasSelect = wasNotSelect = dontParseCSV = false;
      $callback = $fields = null;
      wasError = true;
      // should the next be called ? next();
      callback.call(self, new Error(data.toString()), null);
    } else if(self.listeners('error').length) {
      // notify listeners
      self.emit('error', '' + data);
    } else {
      // log the output avoiding exit 1
      // if no listener was added
      console.error('' + data);
    }
  }

  // WARNING: this can be very unsafe !!!
  // if there is an error and this
  // property is explicitly set to false
  // it keeps running queries no matter what
  self.ignoreErrors = false;
  // - - - - - - - - - - - - - - - - - - - -

  // safely closes the process
  // will emit 'close' once done
  self.close = function() {
    // close can happen only once
    if (!notWorking) {
      // this should gently terminate the program
      // only once everything scheduled has been completed
      self.query('.exit');
      notWorking = true;
      // the hardly killed version was like this:
      //  program.stdin.end();
      //  program.kill();
    }
  };

  // SELECT last_insert_rowid() FROM table might not work as expected
  // This method makes the operation atomic and reliable
  self.lastRowID = function(table, callback) {
    self.query(
      'SELECT ROWID FROM `' + table + '` ORDER BY ROWID DESC LIMIT 1',
      function(result){
        var row = result[0], k;
        // if headers are switched on
        if (!(row instanceof Array)) {
          for (k in row) {
            if (row.hasOwnProperty(k)) {
              row = [row[k]];
              break;
            }
          }
        }
        (callback || log).call(self, row[0]);
      }
    );
    return self;
  };

  // Handy if for some reason data has to be passed around
  // as string instead of being serialized and deserialized
  // as Array of Arrays. Don't use if not needed.
  self.plain = function() {
    dontParseCSV = true;
    return self.query.apply(self, arguments);
  };

  // main logic/method/entry point
  self.query = function(string, params, fields, callback) {
    if (
      // notWorking is set once .close() has been called
      // it does not make sense to execute anything after
      // the program is being closed
      notWorking &&
      // however, since at that time the program could also be busy
      // let's be sure than this is either the exit call
      // or that the last call is still the exit one
      !(string === '.exit' || queue[queue.length - 1][0] === '.exit')
    ) return onerror('closing'), self;
    // if something is still going on in the sqlite3 shell
    // the progcess is flagged as busy. Just queue other operations
    if (busy) return queue.push(arguments), self;
    // if a SELECT or a PRAGMA ...
    wasSelect = SELECT.test(string);
    if (wasSelect) {
      // SELECT and PRAGMA makes `dblite` busy
      busy = true;
      switch(arguments.length) {
        // all arguments passed, nothing to do
        case 4:
          $callback = callback;
          $fields = fields;
          string = replaceString(string, params);
          break;
        // 3 arguments passed ...
        case 3:
          // is the last one the callback ?
          if (typeof fields == 'function') {
            // assign it
            $callback = fields;
            // has string parameters to repalce
            // such ? or :id and others ?
            if (HAS_PARAMS.test(string)) {
              // no objectification and/or validation needed
              $fields = null;
              // string replaced wit parameters
              string = replaceString(string, params);
            } else {
              // no replacement in the SQL needed
              // objectification with validation
              // if specified, will manage the result
              $fields = params;
            }
          } else {
            // no callback specified at all, probably in "dev mode"
            $callback = log;  // just log the result
            $fields = fields; // use objectification
            string = replaceString(string, params); // replace parameters
          }
          break;
        // in this case ...
        case 2:
          // simple query with a callback
          if (typeof params == 'function') {
            // no objectification
            $fields = null;
            // callback is params argument
            $callback = params;
          } else {
            // "dev mode", just log
            $callback = log;
            // if there's something to replace
            if (HAS_PARAMS.test(string)) {
              // no objectification
              $fields = null;
              string = replaceString(string, params);
            } else {
              // nothing to replace
              // objectification with eventual validation
              $fields = params;
            }
          }
          break;
        default:
          // 1 argument, the SQL string and nothing else
          // "dev mode" log will do
          $callback = log;
          $fields = null;
          break;
      }
      // ask the sqlite3 shell ...
      program.stdin.write(
        // trick to always know when the console is not busy anymore
        // specially for those cases where no result is shown
        sanitize(string) + 'SELECT ' + SUPER_SECRET_SELECT
      );
    } else {
      // if db.plain() was used but this is not a SELECT or PRAGMA
      // something is wrong with the logic since no result
      // was expected anyhow
      if (dontParseCSV) {
        dontParseCSV = false;
        throw new Error('not a select');
      } else if (string[0] === '.') {
        // .commands are special queries .. so
        // .commands make `dblite` busy
        busy = true;
        // same trick with the secret to emit('info', resultAsString)
        // once everything is done
        program.stdin.write(string + EOL + 'SELECT ' + SUPER_SECRET_SELECT);
      } else {
        switch(arguments.length) {
          case 1:
            /* falls through */
          case 2:
            if (typeof params !== 'function') {
              // no need to make the shell busy
              // since no output is shown at all (errors ... eventually)
              // sqlite3 shell will take care of the order
              // same as writing in a linux shell while something else is going on
              // who cares, will show when possible, after current job ^_^
              program.stdin.write(sanitize(HAS_PARAMS.test(string) ?
                replaceString(string, params) :
                string
              ));
              // keep checking for possible following operations
              process.nextTick(next);
              break;
            }
            fields = params;
            // not necessary but guards possible wrong replaceString
            params = null;
            /* falls through */
          case 3:
            // execute a non SELECT/PRAGMA statement
            // and be notified once it's done.
            // set state as busy
            busy = wasNotSelect = true;
            $callback = fields;
            program.stdin.write(
              (sanitize(
                HAS_PARAMS.test(string) ?
                  replaceString(string, params) :
                  string
              )) +
              EOL + 'SELECT ' + SUPER_SECRET_SELECT
            );
        }
      }
    }
    // chainability just useful here for multiple queries at once
    return self;
  };
  return self;
}

// enrich possible fields object with extra headers
function enrichFields(key) {
  var had = this.hasOwnProperty(key),
      callback = had && this[key];
  delete this[key];
  this[key] = had ? callback : String;
}

// if not a memory database
// the file path should be resolved as absolute
function normalizeFirstArgument(args) {
  var file = args[0];
  if (file !== ':memory:') {
    args[0] = path.resolve(args[0]);
  }
  return args.indexOf('-header') < 0 ? args.concat('-noheader') : args;
}

// assuming generated CSV is always like
// 1,what,everEOL
// with double quotes when necessary
// 2,"what's up",everEOL
// this parser works like a charm
function parseCSV(output) {
  /*jshint eqnull: true*/
  if (EOL == null) throw new Error(
    'SQLite EOL not found. Please connect to a database first'
  );
  for(var
    fields = [],
    rows = [],
    index = 0,
    rindex = 0,
    length = output.length,
    i = 0,
    j, loop,
    current,
    endLine,
    iNext,
    str;
    i < length; i++
  ) {
    switch(output[i]) {
      case '"':
        loop = true;
        j = i;
        do {
          iNext = output.indexOf('"', current = j + 1);
          switch(output[j = iNext + 1]) {
            case EOL[0]:
              if (EOL_LENGTH === 2 && output[j + 1] !== EOL[1]) {
                break;
              }
              /* falls through */
            case ',':
              loop = false;
          }
        } while(loop);
        str = output.slice(i + 1, iNext++).replace(DOUBLE_DOUBLE_QUOTES, '"');
        break;
      default:
        iNext = output.indexOf(',', i);
        endLine = output.indexOf(EOL, i);
        if (iNext < 0) iNext = length - EOL_LENGTH;
        str = output.slice(i, endLine < iNext ? (iNext = endLine) : iNext);
        break;
    }
    fields[index++] = str;
    if (output[i = iNext] === EOL[0] && (
        EOL_LENGTH === 1 || (
          output[i + 1] === EOL[1] && ++i
        )
      )
    ) {
      rows[rindex++] = fields;
      fields = [];
      index = 0;
    }
  }
  return rows;
}

// create an object with right validation
// and right fields to simplify the parsing
// NOTE: this is based on ordered key
// which is not specified by old ES specs
// but it works like this in V8
function parseFields($fields) {
  for (var
    current,
    fields = Object.keys($fields),
    parsers = [],
    length = fields.length,
    i = 0; i < length; i++
  ) {
    current = $fields[fields[i]];
    parsers[i] = current === Boolean ?
      $Boolean : (
        current === Date ?
          $Date :
          current || String
      )
    ;
  }
  return {f: fields, p: parsers};
}

// transform SQL strings using parameters
function replaceString(string, params) {
  // if params is an array
  if (isArray(params)) {
    // replace all ? occurence ? with right
    // incremental params[index++]
    paramsIndex = 0;
    paramsArray = params;
    string = string.replace(REPLACE_QUESTIONMARKS, replaceQuestions);
  } else {
    // replace :all @fields with the right
    // object.all or object.fields occurrences
    paramsObject = params;
    string = string.replace(REPLACE_PARAMS, replaceParams);
  }
  paramsArray = paramsObject = null;
  return string;
}

// escape the property found in the SQL
function replaceParams(match, key) {
  return escape(paramsObject[key]);
}

// escape the value found for that ? in the SQL
function replaceQuestions() {
  return escape(paramsArray[paramsIndex++]);
}

// objectification: makes an Array an object
// assuming the context is an array of ordered fields
function row2object(row) {
  for (var
    out = {},
    length = this.length,
    i = 0; i < length; i++
  ) {
    out[this[i]] = row[i];
  }
  return out;
}

// objectification with validation:
// makes an Array a validated object
// assuming the context is an object
// produced via parseFields() function
function row2parsed(row) {
  for (var
    out = {},
    fields = this.f,
    parsers = this.p,
    length = fields.length,
    i = 0; i < length; i++
  ) {
    out[fields[i]] = parsers[i](row[i]);
  }
  return out;
}

// escape in a smart way generic values
// making them compatible with SQLite types
// or useful for JavaScript once retrieved back
function escape(what) {
  /*jshint eqnull: true*/
  if (EOL == null) throw new Error(
    'SQLite EOL not found. Please connect to a database first'
  );
  switch (typeof what) {
    case 'string':
      return "'" + what.replace(
        SINGLE_QUOTES, SINGLE_QUOTES_DOUBLED
      ) + "'";
    case 'object':
      return what == null ?
        'null' :
        ("'" + JSON.stringify(what).replace(
          SINGLE_QUOTES, SINGLE_QUOTES_DOUBLED
        ) + "'")
      ;
    // SQLite has no Boolean type
    case 'boolean':
      return what ? '1' : '0'; // 1 => true, 0 => false
    case 'number':
      // only finite numbers can be stored
      if (isFinite(what)) return '' + what;
  }
  // all other cases
  throw new Error('unsupported data');
}

// makes an SQL statement OK for dblite <=> sqlite communications
function sanitize(string) {
  return string.replace(SANITIZER, '') + SANITIZER_REPLACER;
}

// no Boolean type in SQLite
// this will replace the possible Boolean validator
// returning the right expected value
function $Boolean(field) {
  switch(field.toLowerCase()) {
    case '0':
    case 'false':
    case 'null':
    case '':
      return false;
  }
  return true;
}

// no Date in SQLite, this will
// take care of validating/creating Dates
// when the field is retrieved with a Date validator
function $Date(field) {
  return new Date(
    DECIMAL.test(field) ? parseInt(field, 10) : field
  );
}

// which sqlite3 executable ?
// it is possible to specify a different
// sqlite3 executable even in relative paths
// be sure the file exists and is usable as executable
Object.defineProperty(
  dblite,
  'bin',
  {
    get: function () {
      // normalized string if was a path
      return bin.join(PATH_SEP);
    },
    set: function (value) {
      var isPath = -1 < value.indexOf(PATH_SEP);
      if (isPath) {
        // resolve the path
        value = path.resolve(value);
        // verify it exists
        if (!require(IS_NODE_06 ? 'path' : 'fs').existsSync(value)) {
          throw 'invalid executable: ' + value;
        }
      }
      // assign as Array in any case
      bin = value.split(PATH_SEP);
    }
  }
);

// starting from v0.6.0 sqlite version shuold be specified
// specially if SQLite version is 3.8.6 or greater
// var dblite = require('dblite').withSQLite('3.8.6')
dblite.withSQLite = function (sqliteVersion) {
    dblite.sqliteVersion = sqliteVersion;
    return dblite;
};

// to manually parse CSV data if necessary
// mainly to be able to use db.plain(SQL)
// without parsing it right away and pass the string
// around instead of serializing and de-serializing it
// all the time. Ideally this is a scenario for clusters
// no need to usually do manually anything otherwise.
dblite.parseCSV = parseCSV;

// how to manually escape data
// might be handy to write directly SQL strings
// instead of using handy paramters Array/Object
// usually you don't want to do this
dblite.escape = escape;

// that's it!
module.exports = dblite;

/** some simple example
var db =
  require('./build/dblite.node.js')('./test/dblite.test.sqlite').
  on('info', console.log.bind(console)).
  on('error', console.error.bind(console)).
  on('close', console.log.bind(console));

// CORE FUNCTIONS: http://www.sqlite.org/lang_corefunc.html

// PRAGMA: http://www.sqlite.org/pragma.html
db.query('PRAGMA table_info(kvp)');

// to test memory database
var db = require('./build/dblite.node.js')(':memory:');
db.query('CREATE TABLE test (key INTEGER PRIMARY KEY, value TEXT)') && undefined;
db.query('INSERT INTO test VALUES(null, "asd")') && undefined;
db.query('SELECT * FROM test') && undefined;
// db.close();
*/