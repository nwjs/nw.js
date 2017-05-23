# &lt;webview&gt; Tag
---

[TOC]

Use the `<webview>` tag to embed 'guest' content (such as web pages) in your app. Different from the `<iframe>`, the `<webview>` runs in a separate process than your app; it doesn't have the same permissions as your app and all interactions between your app and embedded content will be asynchronous. This keeps your app safe from the embedded content.

## Synopsis

To embed a web page in your app, add the webview tag to your app's embedder page (this is the app page that will display the guest content). In its simplest form, the webview tag includes the src of the web page and css styles that control the appearance of the webview container:

```html
<webview id="foo" src="http://www.google.com/" style="width:640px; height:480px"></webview>
```

## References

See [Chrome document of `<webview>` tag](https://developer.chrome.com/apps/tags/webview) for detailed API references.

NW.js added the following methods besides upstream APIs:

### webview.showDevTools(show, [container])

* `show` `{boolean}` flag: open or close the devtools window
* `container` `{webview Element}` _Optional_ the element of the `<webview>` to be used to display devtools. By default, the DevTools is shown with a new window.

### loading local files in webview

Add the following permission to the manifest:
```json
  "webview": {
     "partitions": [
        {
          "name": "trusted",
          "accessible_resources": [ "<all_urls>" ]
        }
     ]
  }
```

and add 'partition="trusted"' attribute to the webview tag.

### Node.js support in webview

To enable Node.js support in WebView, add the `allownw` attribute to the webview tag. Then Node.js will be turned on, no matter it loads local file or remote site. Use this feature with caution because webview is normally supposed to load untrusted contents.

### Cookies support in webview

Webview has 'getCookieStoreId()' function which returns storeId which can be used in [chrome.cookies](https://developer.chrome.com/extensions/cookies) API.

##### Example in console:
Lets assume you have simple NW.js app with webview.

Show all cookies for e.g. http://docs.nwjs.io in console (to have some cookies there you need to visit page first):
```html
chrome.cookies.getAll({url:"http://docs.nwjs.io", storeId:webview.getCookieStoreId()}, console.log.bind(console));
```

### Custom invalid certificate handling support in webview
By default NW.js Webview handle invalid certificates the same way as chromium - it shows chrome-like invalid certificate page to user. This is sufficient for most use cases. If you need to handle them in you code (or you just want to provide custom UI for user) you can create webview with `useautomaticcerthandling` attribute. Then you need to take care of `certificateerror` event on webview with information about certificate which you get when invalid certificate occur. To decide what to do with invalid certificate you need to call `allowCertificate(boolean allow)` function on webview.

- `useautomaticcerthandling` - attribute on webview. For custom handling of invalid certificates create webview with `useautomaticcerthandling` set to true
- `certificateerror` - event on webview with information about certificate
- `allowCertificate(boolean allow)` function on webview to decide whether accept or deny certificate
