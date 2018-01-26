#! /usr/bin/env node

var path = require('path'),
    fs = require('fs'),
    url = require('url'),
    mime = require('mime'),
    urlJoin = require('url-join'),
    showDir = require('./ecstatic/show-dir'),
    version = JSON.parse(
      fs.readFileSync(__dirname + '/../package.json').toString()
    ).version,
    status = require('./ecstatic/status-handlers'),
    generateEtag = require('./ecstatic/etag'),
    optsParser = require('./ecstatic/opts');

var ecstatic = module.exports = function (dir, options) {
  if (typeof dir !== 'string') {
    options = dir;
    dir = options.root;
  }

  var root = path.join(path.resolve(dir), '/'),
      opts = optsParser(options),
      cache = opts.cache,
      autoIndex = opts.autoIndex,
      baseDir = opts.baseDir,
      defaultExt = opts.defaultExt,
      handleError = opts.handleError,
      headers = opts.headers,
      serverHeader = opts.serverHeader,
      weakEtags = opts.weakEtags,
      handleOptionsMethod = opts.handleOptionsMethod;

  opts.root = dir;
  if (defaultExt && /^\./.test(defaultExt)) defaultExt = defaultExt.replace(/^\./, '');

  // Support hashes and .types files in mimeTypes @since 0.8
  if (opts.mimeTypes) {
    try {
      // You can pass a JSON blob here---useful for CLI use
      opts.mimeTypes = JSON.parse(opts.mimeTypes);
    } catch (e) {}
    if (typeof opts.mimeTypes === 'string') {
      mime.load(opts.mimeTypes);
    }
    else if (typeof opts.mimeTypes === 'object') {
      mime.define(opts.mimeTypes);
    }
  }


  return function middleware (req, res, next) {

    // Strip any null bytes from the url
    // This was at one point necessary because of an old bug in url.parse
    //
    // See: https://github.com/jfhbrook/node-ecstatic/issues/16#issuecomment-3039914
    // See: https://github.com/jfhbrook/node-ecstatic/commit/43f7e72a31524f88f47e367c3cc3af710e67c9f4
    //
    // But this opens up a regex dos attack vector! D:
    //
    // Based on some research (ie asking #node-dev if this is still an issue),
    // it's *probably* not an issue. :)
    /*
    while(req.url.indexOf('%00') !== -1) {
      req.url = req.url.replace(/\%00/g, '');
    }
    */

    // Figure out the path for the file from the given url
    var parsed = url.parse(req.url);
    try {
      decodeURIComponent(req.url); // check validity of url
      var pathname = decodePathname(parsed.pathname);
    }
    catch (err) {
      return status[400](res, next, { error: err });
    }

    var file = path.normalize(
          path.join(root,
            path.relative(
              path.join('/', baseDir),
              pathname
            )
          )
        ),
        gzipped = file + '.gz';

    if(serverHeader !== false) {
      // Set common headers.
      res.setHeader('server', 'ecstatic-'+version);
    }
    Object.keys(headers).forEach(function (key) {
      res.setHeader(key, headers[key])
    })

    if (req.method === 'OPTIONS' && handleOptionsMethod) {
      return res.end();
    }

    // TODO: This check is broken, which causes the 403 on the
    // expected 404.
    if (file.slice(0, root.length) !== root) {
      return status[403](res, next);
    }

    if (req.method && (req.method !== 'GET' && req.method !== 'HEAD' )) {
      return status[405](res, next);
    }

    function statFile() {
      fs.stat(file, function (err, stat) {
        if (err && (err.code === 'ENOENT' || err.code === 'ENOTDIR')) {
          if (req.statusCode == 404) {
            // This means we're already trying ./404.html and can not find it.
            // So send plain text response with 404 status code
            status[404](res, next);
          }
          else if (!path.extname(parsed.pathname).length && defaultExt) {
            // If there is no file extension in the path and we have a default
            // extension try filename and default extension combination before rendering 404.html.
            middleware({
              url: parsed.pathname + '.' + defaultExt + ((parsed.search) ? parsed.search : ''),
              headers: req.headers
            }, res, next);
          }
          else {
            // Try to serve default ./404.html
            middleware({
              url: (handleError ? ('/' + path.join(baseDir, '404.' + defaultExt)) : req.url),
              headers: req.headers,
              statusCode: 404
            }, res, next);
          }
        }
        else if (err) {
          status[500](res, next, { error: err });
        }
        else if (stat.isDirectory()) {
          if (!autoIndex && !opts.showDir) {
            status[404](res, next);
            return;
          }

          // 302 to / if necessary
          if (!parsed.pathname.match(/\/$/)) {
            res.statusCode = 302;
            res.setHeader('location', parsed.pathname + '/' +
              (parsed.query? ('?' + parsed.query):'')
            );
            return res.end();
          }

          if (autoIndex) {
            return middleware({
              url: urlJoin(encodeURIComponent(pathname), '/index.' + defaultExt),
              headers: req.headers
            }, res, function (err) {
              if (err) {
                return status[500](res, next, { error: err });
              }
              if (opts.showDir) {
                return showDir(opts, stat)(req, res);
              }

              return status[403](res, next);
            });
          }

          if (opts.showDir) {
            return showDir(opts, stat)(req, res);
          }

        }
        else {
          serve(stat);
        }
      });
    }

    // Look for a gzipped file if this is turned on
    if (opts.gzip && shouldCompress(req)) {
      fs.stat(gzipped, function (err, stat) {
        if (!err && stat.isFile()) {
          hasGzipId12(gzipped, function (err, isGzip) {
            if (isGzip) {
              file = gzipped;
              return serve(stat);
            } else {
              statFile();
            }
          });
        } else {
          statFile();
        }
      });
    } else {
      statFile();
    }

    function serve(stat) {
      // Do a MIME lookup, fall back to octet-stream and handle gzip
      // special case.
      var defaultType = opts.contentType || 'application/octet-stream',
          contentType = mime.lookup(file, defaultType),
          charSet;

      if (contentType) {
        charSet = mime.charsets.lookup(contentType, 'utf-8');
        if (charSet) {
          contentType += '; charset=' + charSet;
        }
      }

      if (file === gzipped) { // is .gz picked up
        res.setHeader('Content-Encoding', 'gzip');

        // strip gz ending and lookup mime type
        contentType = mime.lookup(path.basename(file, ".gz"), defaultType);
      }

      var range = (req.headers && req.headers['range']);
      if (range) {
        var total = stat.size;
        var parts = range.replace(/bytes=/, "").split("-");
        var partialstart = parts[0];
        var partialend = parts[1];
        var start = parseInt(partialstart, 10);
        var end = Math.min(total-1, partialend ? parseInt(partialend, 10) : total-1);
        var chunksize = (end-start)+1;
        if (start > end || isNaN(start) || isNaN(end)) {
          return status['416'](res, next);
        }
        var fstream = fs.createReadStream(file, {start: start, end: end});
        fstream.on('error', function (err) {
          status['500'](res, next, { error: err });
        });
        res.on('close', function () {
           fstream.destroy();
        });
        res.writeHead(206, {
          'Content-Range': 'bytes ' + start + '-' + end + '/' + total,
          'Accept-Ranges': 'bytes',
          'Content-Length': chunksize,
          'Content-Type': contentType
        });
        fstream.pipe(res);
        return;
      }

      // TODO: Helper for this, with default headers.
      var lastModified = (new Date(stat.mtime)).toUTCString(),
          etag = generateEtag(stat, weakEtags);
      res.setHeader('last-modified', lastModified);
      res.setHeader('etag', etag);

      if (typeof cache === 'function') {
        var requestSpecificCache = cache(pathname);
        if (typeof requestSpecificCache === 'number') {
          requestSpecificCache = 'max-age=' + requestSpecificCache;
        }
        res.setHeader('cache-control', requestSpecificCache);
      } else {
        res.setHeader('cache-control', cache);
      }

      // Return a 304 if necessary
      if (shouldReturn304(req, lastModified, etag)) {
        return status[304](res, next);
      }

      res.setHeader('content-length', stat.size);
      res.setHeader('content-type', contentType);

      // set the response statusCode if we have a request statusCode.
      // This only can happen if we have a 404 with some kind of 404.html
      // In all other cases where we have a file we serve the 200
      res.statusCode = req.statusCode || 200;

      if (req.method === "HEAD") {
        return res.end();
      }

      var stream = fs.createReadStream(file);

      stream.pipe(res);
      stream.on('error', function (err) {
        status['500'](res, next, { error: err });
      });
    }

    function shouldReturn304(req, serverLastModified, serverEtag) {
      if (!req || !req.headers) {
        return false;
      }

      var clientModifiedSince = req.headers['if-modified-since'],
          clientEtag = req.headers['if-none-match'];

      if (!clientModifiedSince && !clientEtag) {
        // Client did not provide any conditional caching headers
        return false;
      }

      if (clientModifiedSince) {
        // Catch "illegal access" dates that will crash v8
        // https://github.com/jfhbrook/node-ecstatic/pull/179
        try {
          var clientModifiedDate = new Date(Date.parse(clientModifiedSince));
        }
        catch (err) { return false }

        if (clientModifiedDate.toString() === 'Invalid Date') {
          return false;
        }
        // If the client's copy is older than the server's, don't return 304
        if (clientModifiedDate < new Date(serverLastModified)) {
          return false;
        }
      }

      if (clientEtag) {
        // Do a strong or weak etag comparison based on setting
        // https://www.ietf.org/rfc/rfc2616.txt Section 13.3.3
        if (opts.weakCompare && clientEtag !== serverEtag
          && clientEtag !== ('W/' + serverEtag) && ('W/' + clientEtag) !== serverEtag) {
          return false;
        } else if (!opts.weakCompare && (clientEtag !== serverEtag || clientEtag.indexOf('W/') === 0)) {
          return false;
        }
      }

      return true;
    }
  };
};

ecstatic.version = version;
ecstatic.showDir = showDir;

function hasGzipId12(gzipped, cb) {
  var stream = fs.createReadStream(gzipped, { start: 0, end: 1 }),
      buffer = Buffer(''),
      hasBeenCalled = false;

  stream.on("data", function (chunk) {
    buffer = Buffer.concat([buffer, chunk], 2);
  });

  stream.on("error", function (err) {
    if (hasBeenCalled) {
      throw err;
    }

    hasBeenCalled = true;
    cb(error);
  });

  stream.on("close", function () {
    if (hasBeenCalled) {
      return;
    }

    hasBeenCalled = true;
    cb(null, buffer[0] == 31 && buffer[1] == 139);
  });
}

// Check to see if we should try to compress a file with gzip.
function shouldCompress(req) {
  var headers = req.headers;

  return headers && headers['accept-encoding'] &&
    headers['accept-encoding']
      .split(",")
      .some(function (el) {
        return ['*','compress', 'gzip', 'deflate'].indexOf(el) != -1;
      })
    ;
}

// See: https://github.com/jesusabdullah/node-ecstatic/issues/109
function decodePathname(pathname) {
  var pieces = pathname.replace(/\\/g,"/").split('/');

  return pieces.map(function (piece) {
    piece = decodeURIComponent(piece);

    if (process.platform === 'win32' && /\\/.test(piece)) {
      throw new Error('Invalid forward slash character');
    }

    return piece;
  }).join('/');
}

if (!module.parent) {
  var defaults = require('./ecstatic/defaults.json')
  var http = require('http'),
      opts = require('minimist')(process.argv.slice(2), {
        alias: require('./ecstatic/aliases.json'),
        default: defaults,
        boolean: Object.keys(defaults).filter(function (key) {
          return typeof defaults[key] === 'boolean'
        })
      }),
      envPORT = parseInt(process.env.PORT, 10),
      port = envPORT > 1024 && envPORT <= 65536 ? envPORT : opts.port || opts.p || 8000,
      dir = opts.root || opts._[0] || process.cwd();

  if (opts.help || opts.h) {
    var u = console.error;
    u('usage: ecstatic [dir] {options} --port PORT');
    u('see https://npm.im/ecstatic for more docs');
    return;
  }

  http.createServer(ecstatic(dir, opts))
    .listen(port, function () {
      console.log('ecstatic serving ' + dir + ' at http://0.0.0.0:' + port);
    });
}
