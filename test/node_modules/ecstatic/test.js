var fs = require('fs');

var stat = fs.statSync('./package.json'); // or some other file

console.log(stat.mtime);
console.log('' + stat.mtime);
console.log(JSON.stringify(stat.mtime));
