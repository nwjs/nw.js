## Prerequisite

* node-webkit didn't ship third party node modules required for testing, you
need to install them.
* We use `nw-gyp` to build native modules.
* We have test cases that need a http server. And we have set up a simple nodejs server with port 8123 and 8124 opened. The http server document root is `tests/server`. You can add ports in `tests/server/server.js`

So please run this:

````bash
$ npm install -g nw-gyp
$ cd tests
$ npm install -d
````

## How to run tests

The test suits is indeed a node-webkit app, so use node-webkit to run it:

````bash
$ /path-to-node-webkit tests
````

## Command line options

    $ /path-to-node-webkit tests --help

      Usage: nw-test [options]

      Options:

        -h, --help             output usage information
        -V, --version          output the version number
        -S, --silent           hide the browser window and quit when done (run silently)
        -R, --reporter <name>  specify the reporter to use
        -g, --grep <pattern>   only run tests matching <pattern>
        -i, --invert           inverts --grep matches
        -t, --timeout <ms>     set test-case timeout in milliseconds [2000]
        -s, --slow <ms>        "slow" test threshold in milliseconds [75]
        -b, --bail             bail after first test failure
        -A, --async-only       force all tests to take a callback (async)

### -S, --silent
Hide the browser window (run silently), and quit the app when tests are done,
suitable for automatic tests system.

### -o, --output <name>
Output result to a file, this is usually used together with `json`
reporter, so we can write the results of test to the file in the format of
JSON.

### -R, --reporter <name>
The `--reporter` option allows you to specify the reporter that will be used,
defaulting to `html`. This flag may also be used to utilize third-party
reporters. For example if you `npm install mocha-lcov-reporter` you may then
do `--reporter mocha-lcov-reporter`.

### -g, --grep <pattern>

The `--grep` option when specified will trigger mocha to only run tests matching
the given pattern which is internally compiled to a RegExp.

Suppose for example you have `api` related tests, as well as `app` related
tests, as shown in the following snippet; One could use `--grep api` or
`--grep app` to run one or the other. The same goes for any other part of a
suite or test-case title, `--grep users` would be valid as well, or even
`--grep GET`.

````javascript
describe('api', function(){
  describe('GET /api/users', function(){
    it('respond with an array of users')
  })
})

describe('app', function(){
  describe('GET /users', function(){
    it('respond with an array of users')
  })
})
````

### -t, --timeout <ms>
Specifies the test-case timeout, defaulting to 2 seconds. To override you
may pass the timeout in milliseconds, or a value with the s suffix, ex:
`--timeout 2s` or `--timeout 2000` would be equivalent.

### -s, --slow <ms>
Specify the `slow` test threshold, defaulting to 75ms. Mocha uses this to
highlight test-cases that are taking too long.

### -b, --bail
Only interested in the first exception? use `--bail` !

## Automatic test

To make the testing automatic, we need to:

1. Write results to a file.
2. Print the results in a machine-readable format, like JSON.
3. Run tests silently and quit when done.

In node-webkit's test, we can do:

````bash
$ /path-to-node-webkit tests --silent --output result.json --reporter json
````

## Ignore tests that take too long to run

Some tests like visiting html5test.com and getting its score would take too
long to run for normal tasks, these tests are usually marked as `long-to-run`,
so you can use following command to filter out them:

````bash
$ /path-to-node-webkit tests --grep long-to-run -i
````

## Tips
in test case like `node-remote`, we need to open a http server 
to be the remote site. It is automatically opened when we run tests. We use port 8123, 8124 for test.
The http server document root is `tests/server`. We have put some resources there e.g. `node_remote_test.html`.

## Native Modules
When there is a new nw release, We hope you to rebuild native modules.

We use `tools/build_native_modules.py` to build our native modules:

````bash
$ cd src/content/nw
$ python tools/build_native_modules.py`
````
Alao, please update `node_modules` using `npm install -d`.
