# Chrome Extension API {: .doctitle}
---

We already support all chrome.* API in the ["Chrome App" platform](https://developer.chrome.com/apps/api_index)

In addition, we supported some chrome.* API in the [extension platform](https://developer.chrome.com/extensions/api_index). This document tracks the list of them:

* [chrome.contentSettings](https://developer.chrome.com/extensions/contentSettings) : used to control notfication settings.
* [chrome.tabs](https://developer.chrome.com/extensions/tabs) : used to support fully DevTools extensions
* [chrome.proxy](https://developer.chrome.com/extensions/proxy) : manage proxy settings
* [chrome.history](https://developer.chrome.com/extensions/history) : manage history see below

##### chrome.history
Unlike Chrome tab history is not added automatically. You can add them manually e.g. on webview loadstart or loadstop event. `chrome.history.addUrl` details object has optional title attribute (so you can have title in history). Main reason to use `chrome.history.addUrl` is that pages added there are considered visited. E.g. links in webview will change color (`visited` css style will be applied).