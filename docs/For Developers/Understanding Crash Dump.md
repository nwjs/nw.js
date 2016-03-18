# Understanding Crash Dump {: .doctitle}
---

[TOC]

Once NW.js crashed, a `minidump` file (`.dmp`) will be generated on disk. Users can include it in the bug report. You can decode the `minidump` file to get the stack trace when crashed. So it's helpful to find out what's wrong with NW.js in certain cases.

To extract the stack trace from the `minidump` file, you need three things: the minidump (`.dmp`) file generated from the crash, symbols file of NW.js binary and the `minidump_stackwalk` tool.

## Find Minidump File

The minidump file will be generated in the following default directories when NW.js crashed:

* Linux: `~/.config/<name-in-manifest>/Crash\ Reports/`
* Windows: `%LOCALAPPDATA%\CrashPad`
* Mac: `~/Library/Application\ Support/<name-in-manifest>/CrashPad/`

The `<name-in-manifest>` is the `name` field of [Manifest file](../References/Manifest Format.md#name).

!!! note "Strip Headers from Linux Minidump File"
    Minidump file generated on Linux has additional headers in text format. They have to be stripped off before decoding. The real content of minidump file starts with `MDMP` followed by unreadable binaries. So simply delete the text before `MDMP`.

## Organize Symbol Files

Packages of symbol files for released NW.js can be found within the same folder of NW.js download folder. The symbol files (.sym) can be extracted from the downloaded packages.

Then you have to organize the symbol files in a **correct path with correct file name** in order be used by `minidump_stackwalk` tool. `minidump_stackwalk` uses [simple symbol supplier](https://code.google.com/p/chromium/codesearch#chromium/src/breakpad/src/processor/simple_symbol_supplier.cc&l=142) to find symbol files. Following is the way of how it finds the symbol files.

The tool will try to search the `.sym` file as in following pattern:
`{SYMBOLS_ROOT}/{DEBUG_FILE_NAME}/{DEBUG_IDENTIFIER}/{DEBUG_FILE_NAME_WITHOUT_PDB}.sym`

* `{SYMBOLS_ROOT}` is the root folder of all symbol files. You can put all versions / platforms of NW `.sym` files in a same folder.
* `{DEBUG_FILE_NAME}`, `{DEBUG_IDENTIFIER}` and `{DEBUG_FILE_NAME_WITHOUT_PDB}` can be obtained from the first line of `.sym` file which typically looks like `MODULE Linux x86_64 265BDB6BE043D5C70D3A1E279A8F0B1A0 nw`.
    - `265BDB6BE043D5C70D3A1E279A8F0B1A0` is `{DEBUG_IDENTIFIER}`
    - `nw` is `{DEBUG_FILE_NAME}`.
    - `{DEBUG_FILE_NAME_WITHOUT_PDB}` can be converted from `{DEBUG_FILE_NAME}` by removing `.pdb` extension which is only necessary for Windows.

## Decode Minidump with `minidump_stackwalk`

`minidump_stackwalk` can be built with NW.js or from breakpad source code directly on Mac and Linux. Or you can install prebuilt from Cygwin on Windows.

To get the stack trace from `minidump` file, run following command:

```bash
minidump_stackwalk minidump_file.dmp /path/to/symbols_root 2>&1
```

If the symbol files were not organized correctly, you still can get call stack from this tool. But you will not see symbols and there will be a warning in last section of output - "Loaded modules" like this:
```none
0x00240000 - 0x02b29fff nw.exe ??? (main) (WARNING: No symbols, nw.exe.pdb, 669008F7B6EE44058CBD5F21BEB5B5CFe)
```

## Trigger Crash for Testing

To test the crash dump feature, you can trigger the crash on purpose with APIs provided by NW.js: `App.crashBrowser()` and `App.crashRenderer()`. They crash the browser process and the renderer process respectively.

## References

* http://www.chromium.org/developers/decoding-crash-dumps  
* http://code.google.com/p/google-breakpad/wiki/GettingStartedWithBreakpad

