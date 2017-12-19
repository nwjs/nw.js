# Package and Distribute {: .doctitle}
---

[TOC]

This document guides you how to package and distribute NW.js based app.

## Quick Start

You can use following tools to automatically package your NW.js based app for distribution.

* [nwjs-builder-phoenix](https://github.com/evshiron/nwjs-builder-phoenix) (recommended)
* [nw-builder](https://github.com/nwjs-community/nw-builder)

Or your can build your app manually with the instructions below.

## Prepare Your App

Before packaging, you should prepare all necessary files on hands. Check out following checklist to make sure you didn't miss anything:

* [ ] Source code and resources
* [ ] Install NPM modules with `npm install`
* [ ] [Rebuild native Node modules](Advanced/Use Native Node Modules.md)
* [ ] [Build NaCl binaries](Advanced/Use NaCl in NW.js.md)
* [ ] [Compile source code](Advanced/Protect JavaScript Source Code.md) and remove the original files
* [ ] Icon used in [manifest file](../References/Manifest Format.md#icon)

!!! warning
    Do not assume your `node_modules` that target one platform work _as is_ in all platforms. For instance `node-email-templates` has specific Windows & Mac OS X`npm install` commands. Besides, it requires python to install properly, which is not installed by default on Windows.
    As a rule of thumb **`npm install` your `package.json` on each platform you target** to ensure everything works as expected.

!!! note "Filename and Path"
    On most Linux and some Mac OS X, the file system is **case sensitive**. That means `test.js` and `Test.js` are different files. Make sure the paths and filenames used in your app have the right case. Otherwise your app may look bad or crash on those file systems.

!!! note "Long Path on Windows"
    The length of path used in your app may exceed the maximum length (260 characters) on Windows. That will cause various build failures. This usually happens during installing dependencies with `npm install` using older version of NPM (<3.0). Please build your app in the root directory, like `C:\build\`, to avoid this issue as much as possible.

## Prepare NW.js

You have to redistribute NW.js with your app to get your app running. NW.js provided multiple [build flavors](Advanced/Build Flavors.md) for different requirements and package size. Choose the proper build flavor for your product or [build it from source code](../For Developers/Building NW.js.md).

All files in the downloaded package should be redistributed with your product, except tools in SDK flavor including `nwjc`, `payload` and `chromedriver`.

## Package Your App

There two options to pack your app: plain files or zip file.

### Package Option 1. Plain Files (Recommended)

On Windows and Linux, you can put the files of your app in the same folder of NW.js binaries and then ship them to your users. Make sure `nw` (or `nw.exe`) is in the same folder as `package.json`. Or you can put the files of your app in a folder named `package.nw` in the same folder as `nw` (or `nw.exe`).

On Mac, put the files of your app into a folder named `app.nw` in `nwjs.app/Contents/Resources/` and done.

It's the recommended way to pack your app.

### Package Option 2. Zip File

You can package all the files into a zip file and rename it as `package.nw`. And put it along with NW.js binaries for Windows and Linux. For Mac, name it `app.nw` and put it in `nwjs.app/Contents/Resources/`.

!!! warning "Start Slow with Big Package or Too Many Files"
    At starting time, NW.js will unzip the package into temp folder and load it from there. So it will start slower if your package is big or contains too many files.

On Windows and Linux, you can even hide the zip file by appending the zip file to the end of `nw` or `nw.exe`.
You can run following command on Windows to achieve this:
```batch
copy /b nw.exe+package.nw app.exe
```
or following command on Linux:
```bash
cat nw app.nw > app && chmod +x app 
```

## Platform Specific Steps

### Windows

Icon for `nw.exe` can be replaced with tools like [Resource Hacker](http://www.angusj.com/resourcehacker/), [nw-builder](https://github.com/mllrsohn/node-webkit-builder) and [node-winresourcer](https://github.com/felicienfrancois/node-winresourcer).

You can create a installer to deploy all necessary files onto end user's system. You can use [Windows Installer](https://msdn.microsoft.com/en-us/library/cc185688(VS.85).aspx), [NSIS](http://nsis.sourceforge.net/Main_Page) or [Inno Setup](http://www.jrsoftware.org/isinfo.php).

### Linux

On Linux, you need to create proper [`.desktop` file](https://wiki.archlinux.org/index.php/Desktop_Entries).

To create a self-extractable installer script, you can use scripts like [`shar`](https://en.wikipedia.org/wiki/Shar) or [`makeself`](http://stephanepeter.com/makeself/).

To distribute your app through the package management system, like `apt`, `yum`, `pacman` etc, please follow their official documents to create the packages.

### Mac OS X

On Mac OS X, you need to modify following files to have your own icon and bundle id:

* `Contents/Resources/nw.icns`: icon of your app. `nw.icns` is in [Apple Icon Image Format](https://en.wikipedia.org/wiki/Apple_Icon_Image_format). You can convert your icon in PNG/JPEG format into ICNS by using tools like [Image2Icon](http://www.img2icnsapp.com/).
* `Contents/Info.plist`: the apple package description file. You can view [Implementing Cocoa's Standard About Panel](http://cocoadevcentral.com/articles/000071.php) on how this file will influence your app and what fields you should modify.

To rename the application, the following files should be modified:
* `Contents/Info.plist` - CFBundleDisplayName
* `Contents/Resources/en.lproj/InfoPlist.strings` - CFBundleDisplayName
* `package.json` -- add a string field `product_string` (e.g. foobar); the helper application will be shown as 'foobar Helper'. (since v0.24.4)
* `Contents/Versions/n.n.n.n/nwjs Helper.app/Contents/MacOS/nwjs Helper` - rename the file to 'foobar Helper'
* `Contents/Versions/n.n.n.n/nwjs Helper.app/Contents/Info.plist` - change CFBundleDisplayName
* `Contents/Versions/n.n.n.n/nwjs Helper.app` - rename the directory to 'foobar Helper.app'

You should sign your Mac app, or the user won't launch the app if Gatekeeper is turned on. See [Support for Mac App Store](Advanced/Support for Mac App Store.md) for details.

## References

See [wiki of NW.js](https://github.com/nwjs/nw.js/wiki/How-to-package-and-distribute-your-apps) for more tools of packaging your app.
