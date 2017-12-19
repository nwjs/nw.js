# Autoupdates
---

[TOC]

NW.js tend to support the update solution made by community, not a built-in one. Below is the list of existing soutions, which worth checking.
- [node-webkit-updater](https://github.com/edjafarov/node-webkit-updater) (by [@edjafarov](https://github.com/edjafarov))
- [nwjs-autoupdater](https://github.com/oaleynik/nwjs-autoupdater) (by [@oaleynik](https://github.com/oaleynik))
- [nw-autoupdater](https://github.com/dsheiko/nw-autoupdater) (by [@dsheiko](https://github.com/dsheiko))

## node-webkit-updater

NPM module which gives you the low-level API to:

- Check the manifest for version (from your running "old" app).
- If the version is different from the running one, download new package to a temp directory.
- Unpack the package in temp.
- Run new app from temp and kill the old one (i.e. still all from the running app).
- The new app (in temp) will copy itself to the original folder, overwriting the old app.
- The new app will run itself from original folder and exit the process.

You should build this logic by yourself though ([basic example](https://github.com/edjafarov/node-webkit-updater/blob/master/examples/basic.js)).

## nwjs-autoupdater

The tiny golang application (when built it is just ~2MB), which can be bundled with NW.js application and then used to unpack updates.
To update target application updater needs to know two things - where zip archive with the new version is located and where is the app's executable to restart application after update. These can be passed to updater via command line arguments `--bundle` and `--inst-dir`, where `--bundle` is the path to the zip archive with the new app version and `--inst-dir` is the path to app's executable.

There are multiple advantages over `node-webkit-updater`:

- It can update the updater itself.
- Doesn't require elevated privilegies (unless application itself has been installed in the folder which require elevated privilegies).
- Updater's executalbe has way smaller size as it doesn't require bundling the whole new NW.js application with the main application.

The logic to check for updates needs to be built on your own too. The [example](https://github.com/oaleynik/nwjs-autoupdater/blob/master/examples/index.js) shows the way how to use javascript module as an entry point for NW.js application and check for updates in background.

## nw-autoupdater

The NPM module provides API similar to one of `node-webkit-updater`, but extended, adapted for NW.js with Node 7.x and based on clean async/await syntax. Namely it allows to:
- Read the manifest from the remote release server
- Check if the version in the remote manifest greater than one of the local manifest
- Download the latest available version matching the host platform (according to the `packages` map of the remote manifest)
  - Subscribe for download progress events
- Unpack it in a temporary directory (zip or tar.gz)
  - Subscribe for install progress events
- Close the app and launch it (as a detached process) from the downloaded release (from temporary directory)
- Backup actual version and replace it with the new one
- Restart the updated app from its original location

The package includes examples of release server and client.
