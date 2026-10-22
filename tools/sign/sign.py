import os
import os.path as osp
import hashlib
import base64
import json
import sys

# --- helpers ---

def write(text):
    """ helper for writing output, as a single point for replacement """
    print(text)

def filehash(filepath):
    blocksize = 4096
    sha = hashlib.sha256()
    with open(filepath, 'rb') as fp:
        while True:
            data = fp.read(blocksize)
            if not data:
                break
            sha.update(data)
    return base64.b64encode(sha.digest()) 

def fixbase64(str):
    return str.replace('/', '_').replace('+', '-').replace('=', '')

def sign_data(private_key_loc, data):
    from Crypto.PublicKey import RSA
    from Crypto.Signature import PKCS1_v1_5
    from Crypto.Hash import SHA256
    from base64 import b64encode, b64decode
    key = open(private_key_loc, "r").read()
    rsakey = RSA.importKey(key)
    signer = PKCS1_v1_5.new(rsakey)
    digest = SHA256.new()
    digest.update(data)
    sign = signer.sign(digest)
    return b64encode(sign)

# --- /helpers ---

hash = {
      "block_size": 4096,
      "hash_block_size": 4096,
      "format": "treehash",
      "files": [] }

#ROOT = '.'
#for root, dirs, files in os.walk(ROOT):
#    for fpath in [osp.join(root, f) for f in files]:
#        size = osp.getsize(fpath)
#        sha = fixbase64(filehash(fpath))
#        name = osp.relpath(fpath, ROOT)
#        hash['files'].append({"path": name, "root_hash": sha})

content_hashes=[hash]
    app_version="1.2.3"
    if osp.exists('package.json'):
        with open('package.json', 'r') as f:
            try:
                app_version=json.load(f).get('version','1.2.3')
            except:
pass
    payload={"content_hashes":content,"item_id":"abcdefghijklmnopqrs...","item_version":app_version}

#sys.stderr.write(json.dumps(hash))

payload_json = json.dumps(payload)
payload_encoded = fixbase64(base64.b64encode(payload_json))
protected = fixbase64(base64.b64encode('{"alg":"RS256"}'))
signature_input = (protected + '.' + payload_encoded).replace("\n", "")
signature = fixbase64(sign_data(osp.join(osp.dirname(__file__), 'private_key.pem'), signature_input))

manifest = open('package.json', "r").read()
manifest_encoded = fixbase64(base64.b64encode(manifest))
manifest_sig_input = (protected + '.' + manifest_encoded).replace("\n", "")
manifest_sig = fixbase64(sign_data(osp.join(osp.dirname(__file__), 'private_key.pem'), manifest_sig_input))

verfied_content = [ {
    "description": "treehash per file",
    "signed_content": {
        "payload": payload_encoded,
        "manifest": manifest_encoded,
        "signatures": [
            {
                "header": {"kid": "publisher"},
                "protected": protected,
                "signature": "whatever"
            },
            {
                "header": {"kid": "manifest"},
                "protected": protected,
                "signature": manifest_sig
            },
            {
                "header": {"kid": "nwjs"},
                "protected": protected,
                "signature": signature
            }
        ]
    }
}]


print json.dumps(verfied_content, indent=4)
