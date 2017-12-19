(function() {

  // detect `nw` object of NW13
  if (!(self.nw && self.nw.require)) return;

  var realrequire = nw.require;
  self.require = function() {
    if (arguments[0] === 'nw.gui') {
      return nw;
    } else {
      return realrequire.apply(self, [].slice.call(arguments, 0));
    }
  };
  self.require.cache = realrequire.cache;
  self.require.extensions = realrequire.extensions;
  self.require.resolve = realrequire.resolve;

  // Following items exist when running with `--mixed-context`.
  // Copy them from `nw` to browser context
  if (!self.process) self.process = self.nw.process;
  if (!self.Buffer) self.Buffer = self.nw.Buffer;
  if (!self.global) self.global = self.nw.global;

}());
