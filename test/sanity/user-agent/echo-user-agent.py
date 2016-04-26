#!/usr/bin/env python

from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler

class RequestHandler(BaseHTTPRequestHandler):
    
    def do_GET(self):
        
        request_path = self.path
        
        print(self.headers['User-Agent'])
        
        self.send_response(200)
        self.send_header("Set-Cookie", "foo=bar")

    def log_request(self, code):
        pass
        
def main():
    port = 3456
    print('Listening on localhost:%s' % port)
    server = HTTPServer(('', port), RequestHandler)
    server.serve_forever()

        
if __name__ == "__main__":
    main()
