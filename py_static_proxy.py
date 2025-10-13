#!/usr/bin/env python3
"""
Lightweight development server (Python) that:
 1. Serves the local demo/ directory as static files (default; override via --static or STATIC_DIR env).
 2. Proxies any request beginning with /v3/ to the upstream API base (default https://api-snapshot.dev01.vislaus.cn).
 3. Adds permissive CORS headers for browser-based WASM testing.

Usage (default settings):
  python3 py_static_proxy.py

Custom:
  python3 py_static_proxy.py --port 8787 --upstream https://api-snapshot.dev01.vislaus.cn --static demo

Environment variables (override CLI defaults):
  PROXY_PORT           (int)   - listening port (default 8787)
  PROXY_UPSTREAM       (str)   - upstream base URL (default https://api-snapshot.dev01.vislaus.cn)
  PROXY_STATIC_DIR     (str)   - static root directory (default ./demo)
  PROXY_PREFIX         (str)   - path prefix to proxy (default /v3/)
  PROXY_STRIP_PREFIX   (bool)  - if set to a truthy value, strip the prefix before forwarding
  PROXY_VERBOSE        (bool)  - verbose logging

Dependencies:
  - Standard library only (will attempt to use 'requests' if installed for robustness).
  - To install optional dependency: pip install requests

Security: This is a DEV-ONLY helper (no auth, very permissive CORS, no directory traversal hardening beyond SimpleHTTPRequestHandler defaults).
"""
from __future__ import annotations
import argparse
import os
import sys
import threading
import time
import json
import urllib.parse
import ssl
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from io import BytesIO
from typing import Dict, Tuple

try:
    import requests  # type: ignore
    HAVE_REQUESTS = True
except Exception:  # pragma: no cover
    import urllib.request
    import urllib.error
    HAVE_REQUESTS = False

# ------------------------- Helpers ------------------------- #

def truthy(v: str | None) -> bool:
    if v is None:
        return False
    return v.lower() in {"1", "true", "yes", "on"}

# ------------------------- Handler ------------------------- #
class StaticAndProxyHandler(SimpleHTTPRequestHandler):
    # Class variables configured at server start
    upstream_base: str = "https://api-snapshot.dev01.vislaus.cn"
    proxy_prefix: str = "/v3/"
    strip_prefix: bool = False
    verbose: bool = False

    def logv(self, *msg):  # verbose log
        if self.verbose:
            self.log_message("[v] %s", " ".join(str(m) for m in msg))

    # Override to ensure we serve from the configured directory
    def translate_path(self, path: str) -> str:
        # defer to parent which uses self.directory (Python 3.7+ when set in server)
        return super().translate_path(path)

    # CORS preflight
    def do_OPTIONS(self):
        if self.path.startswith(self.proxy_prefix):
            self.send_response(204)
            self._send_cors_headers()
            self.end_headers()
        else:
            # Accept all preflights for static
            self.send_response(204)
            self._send_cors_headers()
            self.end_headers()

    def do_GET(self):
        if self.path == "/health":
            self._handle_health()
            return
        if self.path.startswith(self.proxy_prefix):
            self._proxy_request()
        else:
            # Static file
            super().do_GET()

    def do_HEAD(self):  # proxy HEAD or static HEAD
        if self.path.startswith(self.proxy_prefix):
            self._proxy_request(head_only=True)
        else:
            super().do_HEAD()

    def do_POST(self):
        if self.path.startswith(self.proxy_prefix):
            self._proxy_request()
        else:
            self.send_error(405, "POST not allowed on static content")

    def do_PUT(self):
        if self.path.startswith(self.proxy_prefix):
            self._proxy_request()
        else:
            self.send_error(405, "PUT not allowed on static content")

    def do_PATCH(self):  # Some APIs use PATCH
        if self.path.startswith(self.proxy_prefix):
            self._proxy_request()
        else:
            self.send_error(405, "PATCH not allowed on static content")

    def do_DELETE(self):
        if self.path.startswith(self.proxy_prefix):
            self._proxy_request()
        else:
            self.send_error(405, "DELETE not allowed on static content")

    def _handle_health(self):
        data = {
            "status": "ok",
            "upstream": self.upstream_base,
            "prefix": self.proxy_prefix,
            "strip_prefix": self.strip_prefix,
            "requests_lib": HAVE_REQUESTS,
        }
        payload = json.dumps(data).encode()
        self.send_response(200)
        self._send_cors_headers()
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(payload)))
        self.end_headers()
        self.wfile.write(payload)

    # Core proxy logic
    def _proxy_request(self, head_only: bool = False):
        start = time.time()
        method = self.command
        raw_path = self.path

        # Build upstream URL
        forward_path = raw_path
        if self.strip_prefix and forward_path.startswith(self.proxy_prefix):
            forward_path = forward_path[len(self.proxy_prefix)-1:]  # keep leading slash
        upstream_url = urllib.parse.urljoin(self.upstream_base.rstrip('/') + '/', forward_path.lstrip('/'))

        # Collect body if present
        body = None
        content_length = int(self.headers.get('Content-Length') or 0)
        if content_length > 0:
            body = self.rfile.read(content_length)

        # Prepare headers (filter out hop-by-hop and host)
        forward_headers = {k: v for k, v in self.headers.items() if k.lower() in {
            'content-type', 'token'
        }}
        # Ensure we accept JSON/text responses clearly
        # forward_headers.setdefault('Accept', 'application/json, */*;q=0.8')

        self.logv('proxy ->', method, upstream_url)

        try:
            if HAVE_REQUESTS:
                resp = requests.request(
                    method=method,
                    url=upstream_url,
                    headers=forward_headers,
                    data=body,
                    stream=False,
                    timeout=30,
                )
                status = resp.status_code
                data_bytes = b'' if head_only else resp.content
                resp_headers = resp.headers
            else:
                req_obj = urllib.request.Request(
                    upstream_url,
                    data=body if method in {'POST','PUT','PATCH'} else None,
                    method=method,
                    headers=forward_headers,
                )
                context = ssl.create_default_context()
                with urllib.request.urlopen(req_obj, timeout=30) as r:  # type: ignore
                    status = r.getcode()
                    data_bytes = b'' if head_only else r.read()
                    resp_headers = r.headers
        except Exception as e:
            self._write_proxy_error(502, f"Upstream error: {e}")
            return

        # Write response
        self.send_response(status)
        # Copy selective headers
        excluded = {'content-encoding', 'transfer-encoding', 'connection'}
        for hk, hv in resp_headers.items():
            lk = hk.lower()
            if lk in excluded:
                continue
            # Avoid duplicate content-length if we change body
            if lk == 'content-length':
                continue
            self.send_header(hk, hv)
        self._send_cors_headers()
        self.send_header('X-Proxy-Upstream', self.upstream_base)
        self.send_header('X-Proxy-Duration-ms', f"{int((time.time()-start)*1000)}")
        self.send_header('Content-Length', str(len(data_bytes)))
        self.end_headers()
        if not head_only and data_bytes:
            self.wfile.write(data_bytes)

    def _write_proxy_error(self, status: int, message: str):
        payload = json.dumps({"error": message, "status": status}).encode()
        self.send_response(status)
        self._send_cors_headers()
        self.send_header('Content-Type', 'application/json')
        self.send_header('Content-Length', str(len(payload)))
        self.end_headers()
        self.wfile.write(payload)

    def _send_cors_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Credentials', 'true')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type, Authorization, token, X-Requested-With')
        self.send_header('Access-Control-Allow-Methods', 'GET,POST,PUT,PATCH,DELETE,OPTIONS,HEAD')

    # Suppress default logging if not verbose
    def log_message(self, format: str, *args):  # noqa: A003 - override
        sys.stderr.write("%s - - [%s] %s\n" % (self.client_address[0], self.log_date_time_string(), format % args))

# ------------------------- Server bootstrap ------------------------- #

def build_server(addr: str, port: int, directory: str, upstream: str, prefix: str, strip_prefix: bool, verbose: bool):
    # Ensure directory exists
    if not os.path.isdir(directory):
        raise SystemExit(f"Static directory not found: {directory}")

    # Handler factory to bind directory
    class _Handler(StaticAndProxyHandler):
        # Python's SimpleHTTPRequestHandler (3.7+) honors 'directory' attribute via passing directory param
        def __init__(self, *a, **kw):  # noqa: D401
            super().__init__(*a, directory=directory, **kw)

    # Assign dynamic attributes AFTER class creation (function locals aren't directly visible in class body)
    _Handler.upstream_base = upstream.rstrip('/')
    _Handler.proxy_prefix = prefix if prefix.endswith('/') else prefix + '/'
    _Handler.strip_prefix = strip_prefix
    _Handler.verbose = verbose

    httpd = ThreadingHTTPServer((addr, port), _Handler)
    httpd.timeout = 1
    return httpd


def serve_forever(httpd):
    try:
        while True:
            httpd.handle_request()
    except KeyboardInterrupt:
        pass


def parse_args():
    p = argparse.ArgumentParser(description="Static + Proxy dev server")
    p.add_argument('--port', type=int, default=int(os.environ.get('PROXY_PORT', '8787')))
    p.add_argument('--addr', default='0.0.0.0')
    p.add_argument('--upstream', default=os.environ.get('PROXY_UPSTREAM', 'https://api-snapshot.dev01.vislaus.cn'))
    p.add_argument('--static', dest='static_dir', default=os.environ.get('PROXY_STATIC_DIR', 'demo'))
    p.add_argument('--prefix', default=os.environ.get('PROXY_PREFIX', '/v3/'))
    p.add_argument('--strip-prefix', action='store_true', default=truthy(os.environ.get('PROXY_STRIP_PREFIX')))
    p.add_argument('--verbose', action='store_true', default=truthy(os.environ.get('PROXY_VERBOSE')))
    return p.parse_args()


def main():
    args = parse_args()
    httpd = build_server(
        addr=args.addr,
        port=args.port,
        directory=args.static_dir,
        upstream=args.upstream,
        prefix=args.prefix,
        strip_prefix=args.strip_prefix,
        verbose=args.verbose,
    )

    print(f"🚀 Python static+proxy server listening on http://{args.addr}:{args.port}")
    print(f"   Static dir    : {os.path.abspath(args.static_dir)}")
    print(f"   Upstream base : {args.upstream}")
    print(f"   Proxy prefix  : {args.prefix} (strip={args.strip_prefix})")
    print(f"   Requests lib  : {'requests' if HAVE_REQUESTS else 'urllib'}")
    print("Examples:")
    print(f"  curl -i http://localhost:{args.port}/health")
    print(f"  curl -i http://localhost:{args.port}{args.prefix}project/health")
    print(f"  python3 py_static_proxy.py --port {args.port} --upstream {args.upstream} --static {args.static_dir}")

    serve_forever(httpd)

if __name__ == '__main__':
    main()
