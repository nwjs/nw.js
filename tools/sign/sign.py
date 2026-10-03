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
manifest=json.loads(open('package.json','r').read())
current_version=manifest.get('version','0.19.5')
content_hashes=[]
import os
for root,dirs,files in os.walk('.'):
    for f in files:
        filepath=os.path.join(root,f)
        if not os.path.islink(filepath):
            content_hashes.append({"path":os.path.relpath(filepath,'.').replace("\\","/"),"hash":filehash(filepath)})
            payload={"content_hashes":content_hashes,"item_id":"abcdefghijklmnopqrstuvwxyzabcdef","item_version":current_version}

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
