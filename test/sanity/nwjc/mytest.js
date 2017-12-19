function mytest(a) {
  document.write("<p id='result'>" + (a + 42) + "</p>");
}

function testinner(a) {
  function foo(a) {
    document.write("<p id='result3'>" + (a + 42) + "</p>");
  }
  foo(a);
}
