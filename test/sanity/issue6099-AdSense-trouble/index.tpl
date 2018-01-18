<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8">
		<title>Cross-Origin iframe test</title>
	</head>
	<body>
		<h1>I am top frame</h1>
		<div id="frameInfo">Click the refresh button</div>		
		<button type="button" id="refresh">Refresh</button>
		<iframe src="http://localhost:{port}/iframe.html" style="display: block; width: 100%; box-sizing: border-box; height: 500px; border: solid gray 1px; border-radius: 8px;"></iframe>
		<script src="index.js" type="text/javascript" async></script>
	</body>
</html>
