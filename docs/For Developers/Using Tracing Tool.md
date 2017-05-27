# Using Tracing Tool {: .doctitle}
---

The about:tracing tool is valuable to see what Chrome is doing When diagnosing performance problems.
See [Trace Event Profiling Tool (about:tracing)](https://www.chromium.org/developers/how-tos/trace-event-profiling-tool) for more information on what this tool does.

## Open Tracing Tool

There is no tab manager to open new tab for typing "about:tracing" linke Chrome Browser, so you need to open Tracing programmatically using NW.js API [win.open()](../References/Window.md) for a window.

```javascript
// Create a new window to open Tracing tool
nw.Window.open('chrome://tracing', {}, function(new_win) {
  // And something for new window
});
```