function output(id, msg) {
  var h1 = document.createElement('h1');
  h1.innerHTML = msg;
  h1.setAttribute('id', id);
  document.body.appendChild(h1);
}

function success(id, msg) {
  output(id, 'success' + (msg ? ': ' + msg: ''));
}

function fail(id, msg) {
  output(id, 'fail' + (msg ? ': ' + msg: ''));
}