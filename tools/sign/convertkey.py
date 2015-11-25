from Crypto.PublicKey import RSA
import sys

key = RSA.importKey(open("private_key.pem").read())
der = key.publickey().exportKey("DER")

i = 0
total = len(der)
sys.stdout.write('''
const uint8 kNWSignaturesPublicKey[] = {
   ''')
for c in der:
    sys.stdout.write("{0:#04x}".format(ord(c)))
    i += 1
    if i < total: sys.stdout.write(", ")
    if i % 10 == 0:
        sys.stdout.write("\n   ")
print '''
};
'''
