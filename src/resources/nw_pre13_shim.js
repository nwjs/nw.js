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

  // Following items exist when running with `--mixed-context`.
  // Copy them from `nw` to browser context
  if (!self.global) self.global = self.nw;
  if (!self.process) self.process = self.nw.process;
  if (!self.Buffer) self.Buffer = self.nw.Buffer;
  if (!self.root) self.root = self.nw.root;

}());
