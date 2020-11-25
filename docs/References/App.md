# App {: .doctitle}

---

[TOC]

## App.argv

Get the filtered command line arguments when starting the app. In NW.js, some command line arguments are used by NW.js, which should not be interested of your app. `App.argv` will filter out those arguments and return the ones left. You can get filtered patterns from `App.filteredArgv` and the full arguments from `App.fullArgv`.

## App.fullArgv

Get all the command line arguments when starting the app. The return values contains the arguments used by NW.js, such as `--nwapp`, `--remote-debugging-port` etc.

## App.filteredArgv

Get a list of patterns of filtered command line arguments used by `App.argv`. By default, following patterns are used to filter the arguments:

```javascript
[
  /^--url=/,
  /^--remote-debugging-port=/,
  /^--renderer-cmd-prefix=/,
  /^--nwapp=/
]
```

## App.startPath

Get the directory where the application starts. The application will change the current directory to where the package files reside after start.

## App.dataPath

Get the application's data path in user's directory.

* Windows: `%LOCALAPPDATA%/<name>`
* Linux: `~/.config/<name>`
* OS X: `~/Library/Application Support/<name>/Default` (was `~/Library/Application Support/<name>` in v0.12.3 and below)

`<name>` is the **name** field in the `package.json` manifest.

## App.manifest

Get the JSON object of the manifest file.

## App.clearCache()

Clear the HTTP cache in memory and the one on disk. This method call is synchronized.

## App.clearAppCache(manifest_url)

Mark the Application cache group specified by manifest_url obsolete. This method call is synchronized.

## App.closeAllWindows()

Send the `close` event to all windows of current app, if no window is blocking the `close` event, then the app will quit after all windows have done shutdown. Use this method to quit an app will give windows a chance to save data.

## App.crashBrowser()
## App.crashRenderer()

These 2 functions crashes the browser process and the renderer process respectively, to test the [Crash dump](../For Developers/Understanding Crash Dump.md) feature.

## App.enableComponent(component, callback)

!!! warning "Experimental"
    This API is experimental and subject to change.

* `component` `{String}` ID of component; currently only `WIDEVINE` is supported.
* `callback` `function(version)` callback after the component is enabled; `version` string parameter is the version of the enabled component. '0.0.0.0' means it's not installed. Use `App.updateComponent()` to install it.

## App.getProxyForURL(url)

* `url` `{String}` the URL to query for proxy

Query the proxy to be used for loading `url` in DOM. The return value is in the same format used in [PAC](http://en.wikipedia.org/wiki/Proxy_auto-config) (e.g. "DIRECT", "PROXY localhost:8080").

## App.setProxyConfig(config, pac_url)

* `config` `{String}` Proxy rules
* `pac_url` `{String}` PAC url

Set the proxy config which the web engine will be used to request network resources or PAC url to detect proxy automatically.

Rule (copied from [`net/proxy/proxy_config.h`](https://github.com/nwjs/chromium.src/blob/nw13/net/proxy/proxy_config.h))

```
    // Parses the rules from a string, indicating which proxies to use.
    //
    //   proxy-uri = [<proxy-scheme>"://"]<proxy-host>[":"<proxy-port>]
    //
    //   proxy-uri-list = <proxy-uri>[","<proxy-uri-list>]
    //
    //   url-scheme = "http" | "https" | "ftp" | "socks"
    //
    //   scheme-proxies = [<url-scheme>"="]<proxy-uri-list>
    //
    //   proxy-rules = scheme-proxies[";"<scheme-proxies>]
    //
    // Thus, the proxy-rules string should be a semicolon-separated list of
    // ordered proxies that apply to a particular URL scheme. Unless specified,
    // the proxy scheme for proxy-uris is assumed to be http.
    //
    // Some special cases:
    //  * If the scheme is omitted from the first proxy list, that list applies
    //    to all URL schemes and subsequent lists are ignored.
    //  * If a scheme is omitted from any proxy list after a list where a scheme
    //    has been provided, the list without a scheme is ignored.
    //  * If the url-scheme is set to 'socks', that sets a fallback list that
    //    to all otherwise unspecified url-schemes, however the default proxy-
    //    scheme for proxy urls in the 'socks' list is understood to be
    //    socks4:// if unspecified.
    //
    // For example:
    //   "http=foopy:80;ftp=foopy2"  -- use HTTP proxy "foopy:80" for http://
    //                                  URLs, and HTTP proxy "foopy2:80" for
    //                                  ftp:// URLs.
    //   "foopy:80"                  -- use HTTP proxy "foopy:80" for all URLs.
    //   "foopy:80,bar,direct://"    -- use HTTP proxy "foopy:80" for all URLs,
    //                                  failing over to "bar" if "foopy:80" is
    //                                  unavailable, and after that using no
    //                                  proxy.
    //   "socks4://foopy"            -- use SOCKS v4 proxy "foopy:1080" for all
    //                                  URLs.
    //   "http=foop,socks5://bar.com -- use HTTP proxy "foopy" for http URLs,
    //                                  and fail over to the SOCKS5 proxy
    //                                  "bar.com" if "foop" is unavailable.
    //   "http=foopy,direct://       -- use HTTP proxy "foopy" for http URLs,
    //                                  and use no proxy if "foopy" is
    //                                  unavailable.
    //   "http=foopy;socks=foopy2   --  use HTTP proxy "foopy" for http URLs,
    //                                  and use socks4://foopy2 for all other
    //                                  URLs.
```

## App.quit()

Quit current app. This method will **not** send `close` event to windows and app will just quit quietly.

## App.setCrashDumpDir(dir)

!!! warning "Deprecated"
    This API was deprecated since 0.11.0.

* `dir` `{String}` path to generate the crash dump

Set the directory where the minidump file will be saved on crash. For more information, see [Crash dump](../For Developers/Understanding Crash Dump.md).

## App.addOriginAccessWhitelistEntry(sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains)

* `sourceOrigin` `{String}` The source origin. e.g. `http://github.com/`
* `destinationProtocol` `{String}` The destination protocol where the `sourceOrigin` can access to. e.g. `app`
* `destinationHost` `{String}` The destination host where the `sourceOrigin` can access to. e.g. `myapp`
* `allowDestinationSubdomains` `{Boolean}` If set to true, the `sourceOrigin` is allowed to access subdomains of destinations.

Add an entry to the whitelist used for controlling cross-origin access. Suppose you want to allow HTTP redirecting from `github.com` to the page of your app, use something like this:

```javascript
App.addOriginAccessWhitelistEntry('http://github.com/', 'chrome-extension', location.host, true);
```

Use `App.removeOriginAccessWhitelistEntry` with exactly the same arguments to do the contrary.

## App.removeOriginAccessWhitelistEntry(sourceOrigin, destinationProtocol, destinationHost, allowDestinationSubdomains)

* `sourceOrigin` `{String}` The source origin. e.g. `http://github.com/`
* `destinationProtocol` `{String}` The destination protocol where the `sourceOrigin` can access to. e.g. `app`
* `destinationHost` `{String}` The destination host where the `sourceOrigin` can access to. e.g. `myapp`
* `allowDestinationSubdomains` `{Boolean}` If set to true, the `sourceOrigin` is allowed to access subdomains of destinations.

Remove an entry from the whitelist used for controlling cross-origin access. See `addOriginAccessWhitelistEntry` above.

## App.registerGlobalHotKey(shortcut)

* `shortcut` `{Shortcut}` the `Shortcut` object to register.

Register a global keyboard shortcut (also known as system-wide hot key) to the system.

See [Shortcut](Shortcut.md) for more information.

## App.unregisterGlobalHotKey(shortcut)

* `shortcut` `{Shortcut}` the `Shortcut` object to unregister.

Unregisters a global keyboard shortcut.

See [Shortcut](Shortcut.md) for more information.

## App.updateComponent(component, callback)

!!! warning "Experimental"
    This API is experimental and subject to change.

* `component` `{String}` ID of component; currently only `WIDEVINE` is supported.
* `callback` `function(success)` callback after the component is updated; `success` is a boolean parameter for the update result.

## Event: open(args)

* `args` `{String}` the full command line of the program.

Emitted when users opened a file with your app.

## Event: reopen

This is a Mac specific feature. This event is sent when the user clicks the dock icon for an already running application.
