module.exports = function (stat, weakEtag) {
  var etag = '"' + [stat.ino, stat.size, JSON.stringify(stat.mtime)].join('-') + '"';
  if (weakEtag) {
    etag = 'W/' + etag;
  }
  return etag;
}
