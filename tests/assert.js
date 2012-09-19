function assert() {
  document.write('<div>' + arguments[arguments.length - 1] + ': ');

  var result = true;
  for (var i = 0; i < arguments.length - 1; ++i) {
    if (!arguments[i]) {
      result = false;
      document.write('<span style="color:red">FAILED</span>');
      document.write('<span style="color:grey"> at condition ' + (i + 1) + ' </span>');
    }
  }

  if (result) {
    document.write('<span style="color:green">PASSED</span>');
  }

  document.write('</div>');
}

