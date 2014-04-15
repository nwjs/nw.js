function mytest(a) {
  document.write(a + 42);
    
  var gui = nodeRequire('nw.gui');
  var result = { ok: true };
  
  var client = nodeRequire('../../nw_test_app').createClient({
    argv: gui.App.argv,
    data: result,
  });
    
    
}