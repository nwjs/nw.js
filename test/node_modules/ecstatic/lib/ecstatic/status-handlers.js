var he = require('he');

// not modified
exports['304'] = function (res, next) {
  res.statusCode = 304;
  res.end();
};

// access denied
exports['403'] = function (res, next) {
  res.statusCode = 403;
  if (typeof next === "function") {
    next();
  }
  else {
    if (res.writable) {
      res.setHeader('content-type', 'text/plain');
      res.end('ACCESS DENIED');
    }
  }
};

// disallowed method
exports['405'] = function (res, next, opts) {
  res.statusCode = 405;
  if (typeof next === "function") {
    next();
  }
  else {
    res.setHeader('allow', (opts && opts.allow) || 'GET, HEAD');
    res.end();
  }
};

// not found
exports['404'] = function (res, next) {
  res.statusCode = 404;
  if (typeof next === "function") {
    next();
  }
  else {
    if (res.writable) {
      res.setHeader('content-type', 'text/plain');
      res.end('File not found. :(');
    }
  }
};

exports['416'] = function (res, next) {
  res.statusCode = 416;
  if (typeof next === "function") {
    next();
  }
  else {
    if (res.writable) {
      res.setHeader('content-type', 'text/plain');
      res.end('Requested range not satisfiable');
    }
  }
};

// flagrant error
exports['500'] = function (res, next, opts) {
  res.statusCode = 500;
  res.setHeader('content-type', 'text/html');
  var error = String(opts.error.stack || opts.error || "No specified error"),
      html = [
    '<!doctype html>',
    '<html>',
    '  <head>',
    '    <meta charset="utf-8">',
    '    <title>500 Internal Server Error</title>',
    '  </head>',
    '  <body>',
    '    <p>',
    '      ' + he.encode(error),
    '    </p>',
    '  </body>',
    '</html>'
  ].join('\n') + '\n';
  res.end(html);
};

// bad request
exports['400'] = function (res, next, opts) {
  res.statusCode = 400;
  res.setHeader('content-type', 'text/html');
  var error = opts && opts.error ? String(opts.error) : 'Malformed request.',
      html = [
    '<!doctype html>',
    '<html>',
    '  <head>',
    '    <meta charset="utf-8">',
    '    <title>400 Bad Request</title>',
    '  </head>',
    '  <body>',
    '    <p>',
    '      ' + he.encode(error),
    '    </p>',
    '  </body>',
    '</html>'
  ].join('\n') + '\n';
  res.end(html);
};
