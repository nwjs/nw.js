var styles = require('./styles'),
    supportedIcons = styles.icons,
    css = styles.css,
    permsToString = require('./perms-to-string'),
    sizeToString = require('./size-to-string'),
    sortFiles = require('./sort-files'),
    fs = require('fs'),
    path = require('path'),
    he = require('he'),
    etag = require('../etag'),
    url = require('url'),
    status = require('../status-handlers');

module.exports = function (opts, stat) {
  // opts are parsed by opts.js, defaults already applied
  var cache = opts.cache,
      root = path.resolve(opts.root),
      baseDir = opts.baseDir,
      humanReadable = opts.humanReadable,
      handleError = opts.handleError,
      showDotfiles = opts.showDotfiles,
      si = opts.si,
      weakEtags = opts.weakEtags;

  return function middleware (req, res, next) {

    // Figure out the path for the file from the given url
    var parsed = url.parse(req.url),
        pathname = decodeURIComponent(parsed.pathname),
        dir = path.normalize(
          path.join(root,
            path.relative(
              path.join('/', baseDir),
              pathname
            )
          )
        );

    fs.stat(dir, function (err, stat) {
      if (err) {
        return handleError ? status[500](res, next, { error: err }) : next();
      }

      // files are the listing of dir
      fs.readdir(dir, function (err, files) {
        if (err) {
          return handleError ? status[500](res, next, { error: err }) : next();
        }

        // Optionally exclude dotfiles from directory listing.
        if (!showDotfiles) {
          files = files.filter(function(filename){
            return filename.slice(0,1) !== '.';
          });
        }

        res.setHeader('content-type', 'text/html');
        res.setHeader('etag', etag(stat, weakEtags));
        res.setHeader('last-modified', (new Date(stat.mtime)).toUTCString());
        res.setHeader('cache-control', cache);

        sortFiles(dir, files, function (lolwuts, dirs, files) {
          // It's possible to get stat errors for all sorts of reasons here.
          // Unfortunately, our two choices are to either bail completely,
          // or just truck along as though everything's cool. In this case,
          // I decided to just tack them on as "??!?" items along with dirs
          // and files.
          //
          // Whatever.

          // if it makes sense to, add a .. link
          if (path.resolve(dir, '..').slice(0, root.length) == root) {
            return fs.stat(path.join(dir, '..'), function (err, s) {
              if (err) {
                return handleError ? status[500](res, next, { error: err }) : next();
              }
              dirs.unshift([ '..', s ]);
              render(dirs, files, lolwuts);
            });
          }
          render(dirs, files, lolwuts);
        });

        function render(dirs, files, lolwuts) {
          // each entry in the array is a [name, stat] tuple

          var html = [
            '<!doctype html>',
            '<html>',
            '  <head>',
            '    <meta charset="utf-8">',
            '    <meta name="viewport" content="width=device-width">',
            '    <title>Index of ' + he.encode(pathname) +'</title>',
            '    <style type="text/css">' + css + '</style>',
            '  </head>',
            '  <body>',
            '<h1>Index of ' + he.encode(pathname) + '</h1>'
          ].join('\n') + '\n';

          html += '<table>';

          var failed = false;
          var writeRow = function (file, i) {
            // render a row given a [name, stat] tuple
            var isDir = file[1].isDirectory && file[1].isDirectory();
            var href = parsed.pathname.replace(/\/$/, '') + '/' + encodeURIComponent(file[0]);

            // append trailing slash and query for dir entry
            if (isDir) {
              href += '/' + he.encode((parsed.search)? parsed.search:'');
            }

            var displayName = he.encode(file[0]) + ((isDir)? '/':'');

            var ext = file[0].split('.').pop();

            var iconClass = 'icon-' + (isDir ? '_blank' : (supportedIcons[ext] ? ext : '_page'));

            // TODO: use stylessheets?
            html += '<tr>' +
              '<td class="icon-parent"><i class="' + iconClass + '"></i></td>' + 
              '<td class="perms"><code>(' + permsToString(file[1]) + ')</code></td>' +
              '<td class="file-size"><code>' + sizeToString(file[1], humanReadable, si) + '</code></td>' +
              '<td class="display-name"><a href="' + href + '">' + displayName + '</a></td>' +
              '</tr>\n';
          };

          dirs.sort(function (a, b) { return a[0].toString().localeCompare(b[0].toString()); }).forEach(writeRow);
          files.sort(function (a, b) { return a.toString().localeCompare(b.toString()); }).forEach(writeRow);
          lolwuts.sort(function (a, b) { return a[0].toString().localeCompare(b[0].toString()); }).forEach(writeRow);

          html += '</table>\n';
          html += '<br><address>Node.js ' +
            process.version +
            '/ <a href="https://github.com/jfhbrook/node-ecstatic">ecstatic</a> ' +
            'server running @ ' +
            he.encode(req.headers.host || '') + '</address>\n' +
            '</body></html>'
          ;

          if (!failed) {
            res.writeHead(200, { "Content-Type": "text/html" });
            res.end(html);
          }
        }
      });
    });
  };
};

// given a file's stat, return the size of it in string
// humanReadable: (boolean) whether to result is human readable
// si: (boolean) whether to use si (1k = 1000), otherwise 1k = 1024
// adopted from http://stackoverflow.com/a/14919494/665507
function sizeToString(stat, humanReadable, si) {
    if (stat.isDirectory && stat.isDirectory()) {
      return '';
    }

    var sizeString = '';
    var bytes = stat.size;
    var threshold = si ? 1000 : 1024;

    if (!humanReadable || bytes < threshold) {
      return bytes + 'B';
    }

    var units = [ 'k','M','G','T','P','E','Z','Y' ];
    var u = -1;
    do {
        bytes /= threshold;
        ++u;
    } while (bytes >= threshold);

    var b = bytes.toFixed(1);
    if (isNaN(b)) b = '??';

    return b + units[u];
}
