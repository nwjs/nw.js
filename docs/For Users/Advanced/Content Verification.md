# Content Verification
---

[TOC]

The "content verification" feature, or "app signing", prevents loading unsigned files with your official binary. Given a keypair, 'verified_contents.json' is the signature for the application files. It's created with the tool 'sign.py' and the private key (private_key.pem file). The public key is built into NW.js binary. To run the signed application, use `nw --verify-content=enforce_strict .` in the application directory. It will show the simple page. In the next, try to modify index.html a little bit and run it again. NW will report the file is corrupted and quit immediately.

!!! note
    This feature doesn't prevent someone from hacking your app and loading it with other NW binaries. Consider writing some of your features with C++ and load it with Node.js module, NaCl or [compile your JS to binary with nwjc](Protect JavaScript Source Code.md)

## Sign an application

To sign an application with the demo keypair, do the following:

1. change to application directory
2. make sure `verified_contents.json` or `computed_hashes.json` is not there (you can just remove them)
3. run `payload.exe`; it will generate `payload.json`, which serves as input for `sign.py`
4. run `python sign.py > /tmp/verified_contents.json` (it's important that the tmp destination directory is not application directory)
5. move the generated `verified_contents.json` file to application dir and it's done.

## Rebuild to use your own keypair

To use your own keypair you'll need to rebuild the NW binary. And the argument for `--verify-content=` in the command line need to be set to `enforce_strict` by default in your build.

1. generate your keypair: `openssl genrsa -out private_key.pem 2048`
(the output file contains both the private and public key)
2. run python convertkey.py; it will convert the public key to C source code.
3. put the generated source code in
`content/nw/src/nw_content_verifier_delegate.cc`; replace the default key.
4. change the default value for the command line argument: change line no.73 in that file to:
   `Mode experiment_value =  ContentVerifierDelegate::ENFORCE_STRICT;`
5. build the NW binary.

Tools, sample application and the demo private key is under `tools/sign` directory. The demo private key is paired with the public key built into the official NW binaries.


