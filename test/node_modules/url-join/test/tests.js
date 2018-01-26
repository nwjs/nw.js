var urljoin = require('../lib/url-join');

describe('url join', function () {
  it('should work for simple case', function () {
    urljoin('http://www.google.com/', 'foo/bar', '?test=123')
      .should.eql('http://www.google.com/foo/bar?test=123');
  });

  it('should work for simple case with new syntax', function () {
    urljoin(['http://www.google.com/', 'foo/bar', '?test=123'])
      .should.eql('http://www.google.com/foo/bar?test=123');
  });

  it('should work for hashbang urls', function () {
    urljoin(['http://www.google.com', '#!', 'foo/bar', '?test=123'])
      .should.eql('http://www.google.com/#!/foo/bar?test=123');
  });

  it('should be able to join protocol', function () {
    urljoin('http:', 'www.google.com/', 'foo/bar', '?test=123')
      .should.eql('http://www.google.com/foo/bar?test=123');
  });

  it('should be able to join protocol with slashes', function () {
    urljoin('http://', 'www.google.com/', 'foo/bar', '?test=123')
      .should.eql('http://www.google.com/foo/bar?test=123');
  });

  it('should remove extra slashes', function () {
    urljoin('http:', 'www.google.com///', 'foo/bar', '?test=123')
      .should.eql('http://www.google.com/foo/bar?test=123');
  });

  it('should not remove extra slashes in an encoded URL', function () {
    urljoin('http:', 'www.google.com///', 'foo/bar', '?url=http%3A//Ftest.com')
      .should.eql('http://www.google.com/foo/bar?url=http%3A//Ftest.com');

    urljoin('http://a.com/23d04b3/', '/b/c.html')
      .should.eql('http://a.com/23d04b3/b/c.html')
      .should.not.eql('http://a.com/23d04b3//b/c.html');
  });

  it('should support anchors in urls', function () {
    urljoin('http:', 'www.google.com///', 'foo/bar', '?test=123', '#faaaaa')
      .should.eql('http://www.google.com/foo/bar?test=123#faaaaa');
  });

  it('should support protocol-relative urls', function () {
    urljoin('//www.google.com', 'foo/bar', '?test=123')
      .should.eql('//www.google.com/foo/bar?test=123')
  });

  it('should support file protocol urls', function () {
    urljoin('file:/', 'android_asset', 'foo/bar')
      .should.eql('file://android_asset/foo/bar')

    urljoin('file:', '/android_asset', 'foo/bar')
      .should.eql('file://android_asset/foo/bar')
  });

  it('should support absolute file protocol urls', function () {
    urljoin('file:', '///android_asset', 'foo/bar')
      .should.eql('file:///android_asset/foo/bar')

    urljoin('file:///', 'android_asset', 'foo/bar')
      .should.eql('file:///android_asset/foo/bar')

    urljoin('file:///', '//android_asset', 'foo/bar')
      .should.eql('file:///android_asset/foo/bar')

    urljoin('file:///android_asset', 'foo/bar')
      .should.eql('file:///android_asset/foo/bar')
  });

  it('should merge multiple query params properly', function () {
    urljoin('http:', 'www.google.com///', 'foo/bar', '?test=123', '?key=456')
      .should.eql('http://www.google.com/foo/bar?test=123&key=456');

    urljoin('http:', 'www.google.com///', 'foo/bar', '?test=123', '?boom=value', '&key=456')
      .should.eql('http://www.google.com/foo/bar?test=123&boom=value&key=456');
  });
});
