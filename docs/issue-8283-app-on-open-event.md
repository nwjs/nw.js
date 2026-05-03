# Issue #8283: `nw.App.on('open')` only fires once

## Symptom

`nw.App.on('open', callback)` fires at most once when re-launching the same NW.js app. The first external launch (second process instance) triggers the callback, but all subsequent launches do not. `nw.App.onOpen.addListener(callback)` (direct) works correctly every time.

## Root cause

The `on` method in `api_nw_app.js` was registered via `apiFunctions.setHandleRequest('on', …)`. This routes the call through the extension API bridge's `APIBindingHooks::RunHooks`, which calls `APISignature::ParseArgumentsToV8` on the arguments before the custom handler runs. For function-typed arguments, this parsing wraps the original JavaScript callback in a **native V8 binding function** (`function () { [native code] }`).

That native wrapper forwards to the original callback on its first invocation, then becomes inert — exhibiting `once`-like semantics. Since the event emitter stores the wrapper (not the original function), subsequent event dispatches invoke the now-dead native binding, and the user's callback is never reached.

In contrast, `nw.App.onOpen.addListener(callback)` calls the event emitter's `addListener` directly as a native C++ binding, bypassing the API bridge's argument parsing entirely. The original JS function is stored in the listener list and works every time.

The same bug affects `removeListener` — the wrapped callback identity never matches any stored listener, so removal silently fails.

### Call chain (broken path)

```
nw.App.on('open', cb)
 → compiled API interceptor
 → APIBindingHooks::RunHooks
 → APISignature::ParseArgumentsToV8  ← wraps cb into native binding
 → custom hook handler (api_nw_app.js)
 → nw.App.onOpen.addListener(wrapped_cb)  ← stores the wrapper
 → EventEmitter stores native binding
 → on dispatch: native binding fires once, then dead
```

### Call chain (working path — direct addListener)

```
nw.App.onOpen.addListener(cb)
 → EventEmitter::AddListener (native C++ binding)
 → stores original cb directly
 → on dispatch: cb fires every time
```

## Fix

Define `on`, `once`, `removeListener`, and `removeAllListeners` as **direct JavaScript properties** on `bindingsAPI.compiledApi` instead of using `apiFunctions.setHandleRequest`. Direct properties bypass the API bridge's argument parsing entirely, so callbacks preserve their original JS function identity. This matches how `nw.Window.on` is implemented in `api_nw_newwin.js` (prototype method at [line 149](src/resources/api_nw_newwin.js#L149), never goes through the API bridge).

### Key change in `api_nw_app.js`

```js
// Before (broken):
apiFunctions.setHandleRequest('on', function(event, callback) {
    if (eventsMap.hasOwnProperty(event)) {
      nw.App[eventsMap[event]].addListener(callback);
    }
});

// After (fixed):
compiledApi.on = function(event, callback) {
    if (eventsMap.hasOwnProperty(event)) {
      compiledApi[eventsMap[event]].addListener(callback);
    }
};
```

## Verification

Test app registers both `nw.App.onOpen.addListener(…​)` and `nw.App.on('open', …​)`, each incrementing a counter. Launch the app, hide window, re-launch 4 more times:

| Launch | Before fix: `on()` counter | After fix: `on()` counter |
|--------|:---:|:---:|
| 2 | 1 | 1 |
| 3 | 1 (stuck) | **2** |
| 4 | 1 (stuck) | **3** |
| 5 | 1 (stuck) | **4** |

## Related

- #7860 — same symptom, fixed in 2022 by a chromium.src patch, later regressed
- The `once` method was also affected since it calls `this.on(event, g)` internally
