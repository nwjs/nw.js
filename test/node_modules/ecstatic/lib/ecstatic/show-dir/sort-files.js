var fs = require('fs'),
    path = require('path');

module.exports = function sortByIsDirectory(dir, paths, cb) {
  // take the listing file names in `dir`
  // returns directory and file array, each entry is
  // of the array a [name, stat] tuple
  var pending = paths.length,
      errs = [],
      dirs = [],
      files = [];

  if (!pending) {
    return cb(errs, dirs, files);
  }

  paths.forEach(function (file) {
    fs.stat(path.join(dir, file), function (err, s) {
      if (err) {
        errs.push([file, err]);
      }
      else if (s.isDirectory()) {
        dirs.push([file, s]);
      }
      else {
        files.push([file, s]);
      }

      if (--pending === 0) {
        cb(errs, dirs, files);
      }
    });
  });
};

