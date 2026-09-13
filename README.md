# Webserv

A lightweight HTTP/1.1 web server written from scratch in C++, inspired by
Nginx. It serves static content, executes CGI scripts, handles file uploads, and
manages many simultaneous connections with a single-threaded, non-blocking event
loop built on `poll()`.

---

## Table of contents

- [Features](#features)
- [Requirements](#requirements)
- [Build & run](#build--run)
- [Development with Docker](#development-with-docker)
- [Project structure](#project-structure)
- [Architecture](#architecture)
  - [Request lifecycle](#request-lifecycle)
  - [Object model](#object-model)
- [Configuration reference](#configuration-reference)
- [Usage examples](#usage-examples)
- [Resources](#resources)

---

## Features

- **HTTP/1.1** with `GET`, `POST`, and `DELETE`.
- **Static file serving** with configurable document roots.
- **Directory listing** (autoindex) when no index file is present.
- **File uploads** via `POST`, including `chunked` transfer encoding.
- **CGI execution** (e.g. Python, PHP) based on file extension.
- **Custom error pages** per status code.
- **Multiple virtual servers** and per-`location` configuration overrides.
- **Non-blocking I/O**: one thread, a single `poll()` loop, no request blocks another.
- **Request limits & timeouts**: body-size limits and idle-connection timeouts.

---

## Requirements

- A C++ compiler (**clang** recommended — this is the toolchain the project is
  built and tested with) and `make`.
- A POSIX system (Linux or macOS). On Windows, use WSL or the provided
  [Docker environment](#development-with-docker).

---

## Build & run

```bash
make                        # build the ./webserv binary
./webserv config/default.cfg
```

Other Makefile targets: `make clean` (remove objects), `make fclean` (remove
objects and binary), `make re` (rebuild from scratch).

Once running, open the configured host/port in a browser, e.g.
`http://localhost:9090`.

---

## Development with Docker

The repository ships a reproducible Linux dev environment (Docker), so the
project builds and runs the same way on any machine — Linux, macOS, or Windows
(WSL) — with only Docker installed. The container bind-mounts the repository, so
your edits on the host apply instantly inside it.

```bash
git clone https://github.com/DivineSean/Webserv.git
cd Webserv

make up      # build the image and start the container (repo synced to /webserv)
make bash    # open a shell inside the container
```

Then, inside the container:

```bash
make re && ./webserv config/default.cfg
```

Open `http://localhost:9090` on your host to reach the server.

| Host command | Effect                                        |
|--------------|-----------------------------------------------|
| `make up`    | Build the image and start the synced container |
| `make bash`  | Shell into the container                       |
| `make down`  | Stop and remove the container                  |
| `make logs`  | Follow container logs                          |

These wrap Docker Compose (`docker-compose.yml` + `docker/Dockerfile`); the raw
equivalents are `docker compose up -d --build`, `docker compose exec webserv
bash`, and `docker compose down`.

> The container compiles with clang and `-std=c++17`. macOS's clang tolerates
> the `-std=c++98` in the project Makefile, but Linux does not (the code uses
> `std::to_string`, `nullptr`, and `<filesystem>`), so the container adjusts the
> standard transparently. See `docker/Dockerfile` for details.

---

## Project structure

```
Webserv/
├── Makefile                # builds the server
├── main.cpp                # entry point: parse config, start the event loop
├── include/                # headers
│   ├── webserv.hpp         # top-level server manager + config helpers
│   ├── server.hpp          # a virtual server (config + HTTP helpers)
│   ├── location.hpp        # per-location configuration
│   ├── client.hpp          # a single connection: request parse + response build
│   ├── request.hpp         # parsed request data
│   ├── response.hpp        # response state
│   └── MethodHandler.hpp   # HTTP method handler hierarchy (GET/POST/DELETE)
├── src/                    # implementations
│   ├── webserv.cpp
│   ├── server.cpp
│   ├── location.cpp
│   ├── runServer.cpp       # the poll() event loop
│   ├── HTTP_Requests.cpp   # request parsing, routing, response generation
│   └── MethodHandler.cpp   # concrete method handlers
├── config/
│   └── default.cfg         # example configuration
├── www/                    # web root: static pages, error pages, uploads
├── cgi-bin/                # CGI scripts
├── template/               # example site assets
├── docker/
│   └── Dockerfile          # Linux dev image (clang, valgrind, python3, php, curl, siege)
└── docker-compose.yml      # dev container: builds the image, syncs the repo
```

---

## Architecture

The server is layered so that the network loop, the configuration model, and the
per-request logic are separated:

```
        ┌───────────────────────────────────────────────┐
        │  Event loop (runServer)                        │  poll() over all sockets
        │  webserv: owns servers, clients, pollfds       │  accept / read / write
        └───────────────┬───────────────────────────────┘
                        │
        ┌───────────────▼───────────────┐   ┌──────────────────────────────┐
        │  client                        │   │  Configuration model          │
        │  parse request, build response │   │  server  ── has many ──> location │
        └───────────────┬───────────────┘   └──────────────────────────────┘
                        │ dispatch by method
        ┌───────────────▼───────────────────────────────┐
        │  AMethodHandler  (abstract)                     │
        │    ├── GetHandler                               │
        │    ├── PostHandler                              │
        │    └── DeleteHandler                            │
        └────────────────────────────────────────────────┘
```

### Request lifecycle

1. `poll()` reports which sockets are ready.
2. A ready **listening** socket → `accept()` a new connection; a `client` and its
   `pollfd` are registered.
3. A readable **client** socket → data is read and appended to the request
   buffer; headers and body are parsed incrementally.
4. When the request is complete, it is **routed**: the matching `server` and
   `location` are resolved, then a handler is selected for the HTTP method.
5. The handler produces the response (status code, headers, and body — a static
   file, a directory listing, a CGI result, or an error page).
6. A writable client socket → the response is written in chunks until finished;
   the connection is then closed or kept alive.
7. Idle connections are closed after a timeout.

### Object model

- **`webserv`** — the top-level manager. Owns the list of `server`s, the active
  `client`s, and the `pollfd` set, and drives the event loop.
- **`server`** — one virtual server: its listening host/ports, document root,
  allowed methods, error pages, CGI settings, and its `location`s.
- **`location`** — configuration for a path or extension that overrides the
  server defaults for matching requests.
- **`client`** — everything about a single connection: the socket, the parsed
  `request`, the `response` being built, and the owning `server`.
- **`request` / `response`** — the parsed request and the response state.
- **`AMethodHandler`** — an abstract base declaring `handle()`. `GetHandler`,
  `PostHandler`, and `DeleteHandler` each implement one method's behaviour, and
  `HandlerFactory` returns the right one for a request. Routing calls `handle()`
  through the base class, so adding a method means adding a class rather than
  editing the dispatch — the request path never branches on a method string.

Configuration fields are encapsulated (private state exposed through accessors),
so parsing and request handling depend on the interfaces of `server` and
`location`, not on their internals.

---

## Configuration reference

Webserv is driven by a configuration file passed as the only argument:
`./webserv config/default.cfg`. The syntax is inspired by Nginx.

### Syntax rules

- The file contains **one or more `server { ... }` blocks**. Each defines one
  virtual server.
- A `server` block may contain **`location <path> { ... }`** sub-blocks that
  override settings for requests matching that path (or an extension like `*.py`).
- Every directive ends with a semicolon **`;`**.
- List values are **comma-separated** (e.g. `listen 9090,9091;`).
- Anything after **`#`** on a line is a comment.
- Each directive may appear **at most once per block**; a duplicate is a syntax
  error and the server refuses to start.
- Whitespace and blank lines are free-form.

### Server directives

| Directive       | Value                                    | Example                                     | Description |
|-----------------|------------------------------------------|---------------------------------------------|-------------|
| `host`          | one IPv4 address                         | `host 127.0.0.1;`                           | Interface to bind. Use `0.0.0.0` to accept connections from any interface. |
| `listen`        | one or more ports                        | `listen 9090,9091;`                        | Ports the server listens on. |
| `server_names`  | one or more names                        | `server_names example.com,www.example.com;` | Virtual-host names matched against the request `Host` header. |
| `root`          | directory path                           | `root /var/www/;`                          | Document root under which request paths are resolved. |
| `index`         | one or more filenames                    | `index index.html,index.htm;`              | Files served when a directory is requested. |
| `allow`         | one or more HTTP methods                 | `allow GET,POST,DELETE;`                   | Permitted methods; any other method gets `501`. |
| `autoindex`     | `on` / `off`                             | `autoindex on;`                            | When `on`, a directory without an index file returns a generated listing. |
| `max_body_size` | number + unit `B`/`K`/`M`/`G`            | `max_body_size 10M;`                       | Maximum request body size (unit required). Exceeding it returns `413`. |
| `error_pages`   | `code=path` (comma-separated)            | `error_pages 404=/var/www/404.html;`       | Custom error page(s) for the given status code(s). |
| `upload`        | `on` / `off`                             | `upload on;`                               | Enables file uploads via `POST`. |
| `path_upload`   | directory path                           | `path_upload /var/www/uploads/;`           | Where uploaded files are stored. |
| `cgi_info`      | `<ext> <interpreter> <cgi_dir>` triplets | `cgi_info *.py /usr/bin/python3 /cgi-bin;`  | Maps a file extension to an interpreter and CGI directory (comma-separated for several). |

### Location directives

A `location` accepts the same directives as a `server` (`root`, `index`,
`allow`, `autoindex`, `max_body_size`, `error_pages`, `upload`, `path_upload`,
`cgi_info`) and, additionally:

| Directive | Value       | Example                       | Description |
|-----------|-------------|-------------------------------|-------------|
| `return`  | `code=path` | `return 301=/new-location;`   | Redirects matching requests to the given path or URL. |

Values set inside a `location` override the server-level values for requests
that match that location.

### Example

```nginx
server
{
    host 0.0.0.0;
    listen 9090,9091;
    server_names localhost;
    root /webserv/www/;
    index index.html,index.htm;
    allow GET,POST,DELETE;
    autoindex on;
    max_body_size 10M;
    upload on;
    path_upload /webserv/www/UploadingFiles/;
    error_pages 404=/webserv/www/error_pages/404.html;
    cgi_info *.php php /cgi-bin, *.py /usr/bin/python3 /cgi-bin;

    location /UploadingFiles
    {
        root /webserv/www;
        allow GET,DELETE;
    }

    location *.py
    {
        cgi_info *.py /usr/bin/python3 /cgi-bin/;
    }
}
```

> Use absolute paths for `root` and `path_upload`, and set `host 0.0.0.0;` when
> the server must be reachable from another machine (for example your host when
> running inside a container).

---

## Usage examples

With the server running on port `9090`:

```bash
# Fetch a page
curl http://localhost:9090/

# Upload a file
curl -X POST --data-binary @photo.jpg http://localhost:9090/photo.jpg

# Delete an uploaded file
curl -X DELETE http://localhost:9090/UploadingFiles/photo.jpg
```

---

## Resources

### HTTP & web servers
- [RFC 2616 — HTTP/1.1](https://datatracker.ietf.org/doc/html/rfc2616)
- [RFC 3875 — CGI](https://datatracker.ietf.org/doc/html/rfc3875)
- [HTTP overview (MDN)](https://developer.mozilla.org/en-US/docs/Web/HTTP)
- [HTTP/2 — a second look](https://www.cloudflare.com/learning/performance/http2/)

### Networking & sockets
- [What is TCP?](https://www.cloudflare.com/learning/network-layer/what-is-tcp/)
- [Socket programming in C++](https://www.geeksforgeeks.org/socket-programming-in-cpp/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)

### Nginx & CGI
- [CGI overview](https://www.tutorialspoint.com/http/http_cgi.htm)
- [Nginx location match tester](https://nginx.viraptor.info/)
- [How sessions and cookies work](https://developer.mozilla.org/en-US/docs/Web/HTTP/Cookies)
