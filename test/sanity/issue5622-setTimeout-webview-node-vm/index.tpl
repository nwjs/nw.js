<webview id="sandbox" allownw src="about:blank"></webview>
<script>
  const sandbox = document.getElementById('sandbox');
  setTimeout(function() {
    sandbox.showDevTools(true);
    sandbox.setAttribute('src', 'http://localhost:{port}/sandbox.html');
  }, 100);
</script>