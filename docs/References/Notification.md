
_Notification API requires node-webkit >= v0.10.1_

`Notification` specification is based from http://www.w3.org/TR/notifications/

Here is the API specification:
```javascript 
[Constructor(DOMString title, optional NotificationOptions options)]
interface Notification : EventTarget {

  attribute EventHandler onclick;
  attribute EventHandler onshow;
  attribute EventHandler onerror;
  attribute EventHandler onclose;

  readonly attribute DOMString body;
  readonly attribute DOMString icon;

  void close();
};

dictionary NotificationOptions {
  DOMString body;
  DOMString icon;
};
```

# Sample code
```javascript 

var options = {
  icon: "http://yourimage.jpg",
  body: "Here is the notification text"
 };
	
var notification = new Notification("Notification Title",options);
notification.onclick = function () {
  document.getElementById("output").innerHTML += "Notification clicked";
}
	
notification.onshow = function () {
  // play sound on show
  myAud=document.getElementById("audio1");
  myAud.play();

  // auto close after 1 second
  setTimeout(function() {notification.close();}, 1000);
}
```
# General Notes
you can use `http://` or `file://` for the `icon`. node-webkit supports jpg / png files
## Windows
### Balloon Notification
if no icon is specified, it will use the app's icon.

icon size must be >= 32x32

### Toast Notification (requires node-webkit >= v0.11)
* Only for Win8 (or newer)
* The icon must be using `file://`
* A shortcut to your app, with a System.AppUserModel.ID, must be installed to the Start screen. Note, however, that it does not need to be pinned to the Start screen. For more information, see How to enable desktop toast notifications through an AppUserModelID (http://msdn.microsoft.com/en-us/library/windows/desktop/hh802762(v=vs.85).aspx).

The shortcut, ideally should be created by the application installer, but we have the API to create this, recommended to run this once during the application startup (in case the user deleted the shortcut link)

```javascript 
var gui = require('nw.gui');
gui.App.createShortcut(process.env.APPDATA + "\\Microsoft\\Windows\\Start Menu\\Programs\\node-webkit.lnk");
```
the command will create / update the shortcut link with AppUserModelID

The AppUserModelID can be specified in package.json file
```javascript
{
"app-id": "com.node.webkit.notification.test"
}
```
if it is not, it will use the application name as the AppUserModelID

for complete sample code, go here https://github.com/rogerwang/node-webkit/tree/master/tests/manual_tests/notification


