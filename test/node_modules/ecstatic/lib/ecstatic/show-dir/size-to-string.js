// given a file's stat, return the size of it in string
// humanReadable: (boolean) whether to result is human readable
// si: (boolean) whether to use si (1k = 1000), otherwise 1k = 1024
// adopted from http://stackoverflow.com/a/14919494/665507
module.exports = function sizeToString(stat, humanReadable, si) {
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
};
