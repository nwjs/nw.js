module.exports = function permsToString(stat) {

  if (!stat.isDirectory || !stat.mode) {
    return '???!!!???';
  }

  var dir = stat.isDirectory() ? 'd' : '-',
      mode = stat.mode.toString(8);

  return dir + mode.slice(-3).split('').map(function (n) {
    return [
      '---',
      '--x',
      '-w-',
      '-wx',
      'r--',
      'r-x',
      'rw-',
      'rwx'
    ][parseInt(n, 10)];
  }).join('');
};

