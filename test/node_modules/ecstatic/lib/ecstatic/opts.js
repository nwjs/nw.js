// This is so you can have options aliasing and defaults in one place.

var defaults = require('./defaults.json');
var aliases = require('./aliases.json')

module.exports = function (opts) {
  var autoIndex = defaults.autoIndex,
      showDir = defaults.showDir,
      showDotfiles = defaults.showDotfiles,
      humanReadable = defaults.humanReadable,
      si = defaults.si,
      cache = defaults.cache,
      gzip = defaults.gzip,
      defaultExt = defaults.defaultExt,
      handleError = defaults.handleError,
      headers = {},
      serverHeader = defaults.serverHeader,
      contentType = defaults.contentType,
      mimeTypes,
      weakEtags = defaults.weakEtags,
      weakCompare = defaults.weakCompare,
      handleOptionsMethod = defaults.handleOptionsMethod;

  function isDeclared(k) {
    return typeof opts[k] !== 'undefined' && opts[k] !== null;
  }

  if (opts) {
    aliases.autoIndex.some(function (k) {
      if (isDeclared(k)) {
        autoIndex = opts[k];
        return true;
      }
    });

    aliases.showDir.some(function (k) {
      if (isDeclared(k)) {
        showDir = opts[k];
        return true;
      }
    });

    aliases.showDotfiles.some(function (k) {
      if (isDeclared(k)) {
        showDotfiles = opts[k];
        return true;
      }
    });

    aliases.humanReadable.some(function (k) {
      if (isDeclared(k)) {
        humanReadable = opts[k];
        return true;
      }
    });

    aliases.si.some(function (k) {
      if (isDeclared(k)) {
        si = opts[k];
        return true;
      }
    });

    if (opts.defaultExt && typeof opts.defaultExt === 'string') {
      defaultExt = opts.defaultExt;
    }

    if (typeof opts.cache !== 'undefined' && opts.cache !== null) {
      if (typeof opts.cache === 'string') {
        cache = opts.cache;
      }
      else if (typeof opts.cache === 'number') {
        cache = 'max-age=' + opts.cache;
      }
      else if (typeof opts.cache === 'function') {
        cache = opts.cache
      }
    }

    if (typeof opts.gzip !== 'undefined' && opts.gzip !== null) {
      gzip = opts.gzip;
    }

    aliases.handleError.some(function (k) {
      if (isDeclared(k)) {
        handleError = opts[k];
        return true;
      }
    });

    aliases.cors.forEach(function(k) {
      if (isDeclared(k) && k) {
        handleOptionsMethod = true;
        headers['Access-Control-Allow-Origin'] = '*';
        headers['Access-Control-Allow-Headers'] = 'Authorization, Content-Type, If-Match, If-Modified-Since, If-None-Match, If-Unmodified-Since';
      }
    });

    aliases.headers.forEach(function (k) {
      if (!isDeclared(k)) return;
      if (Array.isArray(opts[k])) {
        opts[k].forEach(setHeader);
      }
      else if (opts[k] && typeof opts[k] === 'object') {
        Object.keys(opts[k]).forEach(function (key) {
          headers[key] = opts[k][key];
        });
      }
      else setHeader(opts[k]);

      function setHeader (str) {
        var m = /^(.+?)\s*:\s*(.*)$/.exec(str)
        if (!m) headers[str] = true
        else headers[m[1]] = m[2]
      }
    });

    aliases.serverHeader.some(function (k) {
      if (isDeclared(k)) {
        serverHeader = opts[k];
        return true;
      }
    });

    aliases.contentType.some(function (k) {
      if (isDeclared(k)) {
        contentType = opts[k];
        return true;
      }
    });

    aliases.mimeType.some(function (k) {
      if (isDeclared(k)) {
        mimeTypes = opts[k];
        return true;
      }
    });

    aliases.weakEtags.some(function (k) {
      if (isDeclared(k)) {
        weakEtags = opts[k];
        return true;
      }
    });

    aliases.weakCompare.some(function (k) {
      if (isDeclared(k)) {
        weakCompare = opts[k];
        return true;
      }
    });

    aliases.handleOptionsMethod.some(function (k) {
      if (isDeclared(k)) {
        handleOptionsMethod = handleOptionsMethod || opts[k];
        return true;
      }
    });
  }

  return {
    cache: cache,
    autoIndex: autoIndex,
    showDir: showDir,
    showDotfiles: showDotfiles,
    humanReadable: humanReadable,
    si: si,
    defaultExt: defaultExt,
    baseDir: (opts && opts.baseDir) || '/',
    gzip: gzip,
    handleError: handleError,
    headers: headers,
    serverHeader: serverHeader,
    contentType: contentType,
    mimeTypes: mimeTypes,
    weakEtags: weakEtags,
    weakCompare: weakCompare,
    handleOptionsMethod: handleOptionsMethod
  };
};
