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
