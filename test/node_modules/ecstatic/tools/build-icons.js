var fs = require('fs'),
    path = require('path');

var IMG_SIZE = 16;

var iconDir = path.resolve(__dirname, '../teambox-icons/' + IMG_SIZE + 'px');

var icons = {};

fs.readdirSync(iconDir).forEach(function (filename) {
  var tuple = filename.split('.');
  icons[tuple[0]] = fs.readFileSync(path.resolve(iconDir, filename), 'base64');
});

fs.writeFileSync(path.resolve(__dirname, '../lib/ecstatic/show-dir/icons.json'), JSON.stringify(icons, null, 2), 'utf8');
