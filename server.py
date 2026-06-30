import http.server
import socketserver
import sys

PORT = 8000

class CustomHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        # Disable cache for local development
        self.send_header('Cache-Control', 'no-store, must-revalidate')
        super().end_headers()

# Add WASM mime type to ensure the browser compiles it correctly
CustomHTTPRequestHandler.extensions_map.update({
    '.wasm': 'application/wasm',
})

def start_server():
    with socketserver.TCPServer(("", PORT), CustomHTTPRequestHandler) as httpd:
        print(f"Serving at http://localhost:{PORT}")
        print("Open http://localhost:8000/lsm2_wasm_connected.html in your browser.")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down server...")
            sys.exit(0)

if __name__ == "__main__":
    start_server()
