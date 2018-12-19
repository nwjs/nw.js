global.contentDocument = document
require('./core.js')

if (nw.App.argv.indexOf('inspect') !== -1) {
  nw.Window.open('about:blank', {
    "show": false,
    "width": 799,
    "height": 799,
  }, (inspectWin) => {
    inspectWin.maximize()
    inspectWin.window.location = "chrome://inspect/#devices"
    inspectWin.show()
  })
}
