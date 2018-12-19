var webview = document.createElement('webview')
var devtools = document.createElement('webview')

webview.setAttribute('style', 'height:300px;width:100%;position:absolute')
devtools.setAttribute('style', 'height:300px;width:100%;position:absolute;top:300px')
devtools.setAttribute('partition', 'trusted')

var container = global.contentDocument.getElementById('container')
container.appendChild(webview)
container.appendChild(devtools)

webview.src = 'https://mp.weixin.qq.com'

const devtoolsviewCommit = () => {
  devtools.removeEventListener('loadcommit', devtoolsviewCommit)
  webview.showDevTools(true, devtools)
}
devtools.addEventListener('loadcommit', devtoolsviewCommit)

devtools.src = 'about:blank'
