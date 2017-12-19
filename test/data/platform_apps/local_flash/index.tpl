<html>

<head>
    <title>PDFtest2</title>
    <link rel="stylesheet" href="app.css" />
<script>
function inject() {
chrome.test.sendMessage("Launched");
  var child = document.createElement('div');
  // Set data="foo" to make this content same-origin (so it's not throttled).
  child.innerHTML = '<embed id="plugin" src="<swf_path>" style="min-width: 490px; min-height: 400px;" type="application/x-shockwave-flash" />';
  document.getElementById('content').appendChild(child);
  // Plugins are loaded synchronously during layout, so the plugin has either
  // been loaded or blocked at this point.
  var plugin = document.getElementById('plugin');
  try {
    // All Pepper plugins support postMessage().
    // If postMessage is undefined, the plugin is not loaded.
    if (plugin.postMessage == undefined) {
      document.title = 'Not Loaded';
      return;
    }

    plugin.postMessage('hello');
    // If we do not get an exception, the Pepper plugin is loaded.
    document.title = 'Loaded';
  } catch (e) {
    var errorMessage = 'Unexpected Exception: ' + e.toString();
    document.title = errorMessage;
    console.log(errorMessage);
  }
}
</script>
</head>

<body onload='inject();'>
<div id='content'></div>
</body>

</html>
