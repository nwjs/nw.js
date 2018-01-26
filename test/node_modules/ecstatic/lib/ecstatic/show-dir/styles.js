var fs = require('fs'),
    icons = require('./icons.json'),
    path = require('path');

var IMG_SIZE = 16;

var css = 'td.icon-parent { height: ' + IMG_SIZE + 'px; width: ' + IMG_SIZE + 'px; }\n';
css += 'td.perms {}\n';
css += 'td.file-size { text-align: right; padding-left: 1em; }\n';
css += 'td.display-name { padding-left: 1em; }\n';

Object.keys(icons).forEach(function(key) {
  css += 'i.icon-' + key + ' {\n';
  css += '  display: block; width: 100%; height: 100%; background-repeat: no-repeat;\n';
  css += '  background: url("data:image/png;base64,' + icons[key] + '");\n';
  css += '}\n\n';
});

exports.icons = icons;
exports.css = css;
