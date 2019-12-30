function mytest(a) {
  document.write(a + 42);
    
  var gui = require('nw.gui');
  var result = { ok: true };
  
  var client = require('../../nw_test_app').createClient({
    argv: gui.App.argv,
    data: result,
  });
    
    
}