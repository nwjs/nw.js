#!/usr/bin/env python3

from http.server import HTTPServer, BaseHTTPRequestHandler
import sys

class RequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        request_path = self.path
        print(self.headers['User-Agent'])
        self.send_response(200)
        self.send_header("Set-Cookie", "foo=bar")
        self.end_headers()

    def log_request(self, code='-', size='-'):
        pass

def main():
    port = int(sys.argv[1])
    print(f'Listening on localhost:{port}')
    server = HTTPServer(('', port), RequestHandler)
    server.serve_forever()

if __name__ == "__main__":
    main()
