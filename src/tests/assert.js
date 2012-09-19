function assert() {
  var result = true;
  for (var i = 0; i < arguments.length - 1; ++i) {
    result &= arguments[i];
  }

  document.write(arguments[arguments.length - 1] + ': ');
  if (result) {
    document.write('<span style="color:green">PASSED</span>');
  } else {
    document.write('<span style="color:red">FAILED</span>');
  }
  document.write('<br/>');
}

