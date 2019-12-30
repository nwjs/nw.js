function assert() {
  var str = '';
  str += ('<span class="description">' + arguments[arguments.length - 1] + ': </span>');

  var result = true;
  for (var i = 0; i < arguments.length - 1; ++i) {
    if (!arguments[i]) {
      result = false;
      str += ('<span style="color:red">FAILED</span>');
      str += ('<span style="color:grey"> at condition ' + (i + 1) + ' </span>');
    }
  }

  if (result) {
    str = '<div class="success test">' + str + '<span style="color:green">PASSED</span>';
  } else {
    str = '<div class="fail test">' + str;
  }

  str += '</div>';

  $(document.body).append(str);
}

