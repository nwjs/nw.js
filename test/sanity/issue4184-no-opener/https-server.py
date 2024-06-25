#!/usr/bin/env python

import http.server, http.server
import ssl
import sys

PORT = int(sys.argv[1])

httpd = http.server.HTTPServer(('localhost', PORT), http.server.SimpleHTTPRequestHandler)

context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
context.load_cert_chain(certfile='./cert.pem', keyfile='./key.pem')

# Wrap the socket using the context's wrap_socket method
httpd.socket = context.wrap_socket(httpd.socket, server_side=True)

print("serving at port", PORT)
httpd.serve_forever()
