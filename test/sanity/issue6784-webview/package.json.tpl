{
  "name": "wvtest",
  "version": "1.0.0",
  "license": "MIT",
  "description": "",
  "main": "http://127.0.0.1:{port}",
  "webview": {
    "partitions": [
      {
          "name": "trusted*",
          "accessible_resources": ["*"]
      }
    ]
  },
  "node-remote": "<all_urls>"
}
