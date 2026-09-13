# Webserv

A lightweight HTTP/1.1 web server written from scratch in C++, inspired by
Nginx. It serves static content, executes CGI scripts, handles file uploads, and
manages many simultaneous connections with a single-threaded, non-blocking event
loop built on `poll()`.

## Table of contents

- [Features](#features)
- [Requirements and platforms](#requirements-and-platforms)
- [Build and run](#build-and-run)
- [Development with Docker](#development-with-docker)
- [Project structure](#project-structure)
- [Architecture](#architecture)
  - [Request lifecycle](#request-lifecycle)
  - [Object model](#object-model)
- [Configuration reference](#configuration-reference)
- [Usage examples](#usage-examples)
- [Resources](#resources)

## Features

- **HTTP/1.1** with `GET`, `POST`, and `DELETE`.
- **Static file serving** with configurable document roots.
- **Directory listing** (autoindex) when no index file is present.
- **File uploads** via `POST`, including `chunked` transfer encoding.
- **CGI execution** (Python, PHP, and so on) based on file extension.
- **Custom error pages** per status code.
- **Multiple virtual servers**, each with per-`location` configuration overrides.
- **Non-blocking I/O**: one thread, a single `poll()` loop, no request blocks another.
- **Request limits and timeouts**: body-size limits and idle-connection timeouts.

## Requirements and platforms

Webserv is a POSIX program. It is built on Unix system calls, `poll()`, BSD
sockets, `fcntl`, `fork`/`exec` for CGI, and headers such as `unistd.h`,
`arpa/inet.h`, and `dirent.h`. These exist on Linux and macOS but **not on native
Windows**, whose C++ toolchain (MSVC/MinGW) does not provide them. As a result:

| Platform      | Native build (`make`) | Recommended way to run                     |
|---------------|-----------------------|--------------------------------------------|
| Linux         | Yes                   | Native, or Docker for a clean environment  |
| macOS         | Yes (with clang)      | Native, or Docker                          |
| **Windows**   | **No**                | **Docker** (this repo ships it), or WSL    |

**Windows users:** you cannot compile or run this project directly on Windows,
because the code uses Unix-only system calls that Windows does not have. Use the
[Docker environment](#development-with-docker) below. It runs a real Linux inside
a container, so the server behaves exactly as it would on a Linux machine, and
you do not have to install any compiler or library on Windows yourself. Docker
Desktop is the only thing you need.

Native builds need a C++ compiler (**clang** is what the project is tested with)
and `make`.

## Build and run

The application lives in the [`webserv/`](webserv) folder and has its own
Makefile. On Linux or macOS you can build it directly:

```bash
cd webserv
make                        # build the ./webserv binary
./webserv config/default.cfg
```

Other targets: `make clean` (remove object files), `make fclean` (also remove the
binary), `make re` (rebuild from scratch). Object files are written to
`webserv/obj/` so the source folders stay clean.

Once running, open the configured host and port in a browser, for example
`http://localhost:9090`.

On Windows, skip this section and use Docker below.

## Development with Docker

The repository ships a reproducible Linux dev environment. The container carries
its own compiler, libraries, and tools, so the project builds and runs the same
way on any machine with only Docker installed. This is the simplest option on
Linux and macOS, and the required option on Windows, where the server cannot run
natively.

The container bind-mounts the `webserv/` folder at `/webserv`, so edits you make
on the host apply instantly inside it, with no rebuild of the image.

You control the container from the **workspace root** (this folder) with its
`Makefile`:

```bash
git clone https://github.com/DivineSean/Webserv.git
cd Webserv

make up      # start the container (builds the image the first time)
make bash    # enter the container (starts it if needed, no rebuild); you land in /webserv
```

Then, inside the container:

```bash
make re && ./webserv config/default.cfg
```

Open `http://localhost:9090` on your host to reach the server.

| Host command | Effect                                              |
|--------------|-----------------------------------------------------|
| `make up`    | Start the container (builds the image on first run) |
| `make bash`  | Open a shell inside the container (no rebuild)      |
| `make re`    | Rebuild the image and restart the container         |
| `make down`  | Stop and remove the container                       |
| `make clean` | Remove the container, network, and image            |
| `make logs`  | Follow the container logs                           |

`make re` here (workspace root) rebuilds the **container**. The `make re` you run
**inside** the container rebuilds the **server binary**. They are different
Makefiles for different jobs.

These targets wrap Docker Compose. If your host has no `make` (for example plain
Windows PowerShell, where `make` is not installed), run the underlying commands
directly instead: `docker compose up -d --build`, then
`docker compose exec webserv bash`, and `docker compose down` when finished. On
Windows, running `make` itself requires WSL; the raw `docker compose` commands
work in PowerShell.

> Note on the compiler standard: the app Makefile targets `-std=c++98`. macOS's
> clang accepts the few newer features this code uses (`std::to_string`,
> `nullptr`, `<filesystem>`), but Linux does not, so the container compiles with
> `-std=c++17`. See `container/Dockerfile`.

## Project structure

The repository has two levels. The **workspace root** manages the dev container,
and **`webserv/`** is the application itself.

```
Webserv/                        # workspace root
├── Makefile                    # dev container control (make up / bash / down / logs)
├── docker-compose.yml          # dev container definition (mounts webserv/ at /webserv)
├── container/
│   ├── Dockerfile              # Linux image (clang, valgrind, python3, php, curl, siege)
│   └── .dockerignore
├── README.md
└── webserv/                    # the application
    ├── Makefile                # builds the ./webserv binary
    ├── main.cpp                # entry point: parse config, start the event loop
    ├── include/                # headers
    │   ├── webserv.hpp         # top-level server manager and config helpers
    │   ├── server.hpp          # a virtual server (config plus HTTP helpers)
    │   ├── location.hpp        # per-location configuration
    │   ├── client.hpp          # a single connection: request parse and response build
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
    └── template/               # example site assets
```

## Architecture

The server is layered so the network loop, the configuration model, and the
per-request logic stay separate:

```
        +------------------------------------------------+
        |  Event loop (runServer)                        |  poll() over all sockets
        |  webserv: owns servers, clients, pollfds       |  accept / read / write
        +----------------------+-------------------------+
                               |
        +----------------------v---------+   +-----------------------------+
        |  client                        |   |  Configuration model         |
        |  parse request, build response |   |  server  ->  many locations   |
        +----------------------+---------+   +-----------------------------+
                               | dispatch by method
        +----------------------v-------------------------+
        |  AMethodHandler  (abstract)                    |
        |    GetHandler / PostHandler / DeleteHandler    |
        +------------------------------------------------+
```

### Request lifecycle

1. `poll()` reports which sockets are ready.
2. A ready listening socket triggers `accept()` for a new connection, and a
   `client` with its `pollfd` is registered.
3. A readable client socket is read into the request buffer, and headers and body
   are parsed incrementally.
4. When the request is complete it is routed: the matching `server` and
   `location` are resolved, then a handler is selected for the HTTP method.
5. The handler produces the response: a status code, headers, and a body (a
   static file, a directory listing, a CGI result, or an error page).
6. A writable client socket receives the response, written in chunks until it is
   finished, then the connection is closed or kept alive.
7. Idle connections are closed after a timeout.

### Object model

- **`webserv`** is the top-level manager. It owns the list of `server`s, the
  active `client`s, and the `pollfd` set, and it drives the event loop.
- **`server`** is one virtual server: its listening host and ports, document
  root, allowed methods, error pages, CGI settings, and its `location`s.
- **`location`** holds configuration for a path or extension and overrides the
  server defaults for matching requests.
- **`client`** holds everything about a single connection: the socket, the parsed
  `request`, the `response` being built, and the owning `server`.
- **`request`** and **`response`** hold the parsed request and the response state.
- **`AMethodHandler`** is an abstract base that declares `handle()`. `GetHandler`,
  `PostHandler`, and `DeleteHandler` each implement one method's behaviour, and
  `HandlerFactory` returns the right one for a request. Routing calls `handle()`
  through the base class, so adding a method means adding a class instead of
  editing the dispatch. The request path never branches on a method string.

Configuration fields are encapsulated: their state is private and reached through
accessors, so parsing and request handling depend on the interfaces of `server`
and `location`, not on their internals.

## Configuration reference

Webserv is driven by a configuration file passed as the only argument, for
example `./webserv config/default.cfg`. The syntax is inspired by Nginx.

### Syntax rules

- The file contains one or more `server { ... }` blocks. Each defines one virtual
  server.
- A `server` block may contain `location <path> { ... }` sub-blocks that override
  settings for requests matching that path (or an extension such as `*.py`).
- Every directive ends with a semicolon `;`.
- List values are comma-separated, for example `listen 9090,9091;`.
- Anything after `#` on a line is a comment.
- Each directive may appear at most once per block. A duplicate is a syntax error
  and the server refuses to start.
- Whitespace and blank lines are free-form.

### Server directives

| Directive       | Value                                    | Example                                     | Description |
|-----------------|------------------------------------------|---------------------------------------------|-------------|
| `host`          | one IPv4 address                         | `host 127.0.0.1;`                           | Interface to bind. Use `0.0.0.0` to accept connections on any interface. |
| `listen`        | one or more ports                        | `listen 9090,9091;`                        | Ports the server listens on. |
| `server_names`  | one or more names                        | `server_names example.com,www.example.com;` | Virtual-host names matched against the request `Host` header. |
| `root`          | directory path                           | `root /var/www/;`                          | Document root under which request paths are resolved. |
| `index`         | one or more filenames                    | `index index.html,index.htm;`              | Files served when a directory is requested. |
| `allow`         | one or more HTTP methods                 | `allow GET,POST,DELETE;`                   | Permitted methods. Any other method gets `501`. |
| `autoindex`     | `on` / `off`                             | `autoindex on;`                            | When `on`, a directory without an index file returns a generated listing. |
| `max_body_size` | number plus unit `B`/`K`/`M`/`G`         | `max_body_size 10M;`                       | Maximum request body size (the unit is required). Exceeding it returns `413`. |
| `error_pages`   | `code=path` (comma-separated)            | `error_pages 404=/var/www/404.html;`       | Custom error page for the given status code(s). |
| `upload`        | `on` / `off`                             | `upload on;`                               | Enables file uploads via `POST`. |
| `path_upload`   | directory path                           | `path_upload /var/www/uploads/;`           | Where uploaded files are stored. |
| `cgi_info`      | `<ext> <interpreter> <cgi_dir>` triplets | `cgi_info *.py /usr/bin/python3 /cgi-bin;`  | Maps a file extension to an interpreter and CGI directory (comma-separated for several). |

### Location directives

A `location` accepts the same directives as a `server` (`root`, `index`,
`allow`, `autoindex`, `max_body_size`, `error_pages`, `upload`, `path_upload`,
`cgi_info`) plus one more:

| Directive | Value       | Example                       | Description |
|-----------|-------------|-------------------------------|-------------|
| `return`  | `code=path` | `return 301=/new-location;`   | Redirects matching requests to the given path or URL. |

Values set inside a `location` override the server-level values for requests that
match that location.

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
> the server runs inside the container).

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

## Resources

### HTTP and web servers
- [RFC 2616, HTTP/1.1](https://datatracker.ietf.org/doc/html/rfc2616)
- [RFC 3875, CGI](https://datatracker.ietf.org/doc/html/rfc3875)
- [HTTP overview (MDN)](https://developer.mozilla.org/en-US/docs/Web/HTTP)
- [HTTP/2 explained](https://www.cloudflare.com/learning/performance/http2/)

### Networking and sockets
- [What is TCP?](https://www.cloudflare.com/learning/network-layer/what-is-tcp/)
- [Socket programming in C++](https://www.geeksforgeeks.org/socket-programming-in-cpp/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)

### Nginx and CGI
- [CGI overview](https://www.tutorialspoint.com/http/http_cgi.htm)
- [Nginx location match tester](https://nginx.viraptor.info/)
- [How sessions and cookies work](https://developer.mozilla.org/en-US/docs/Web/HTTP/Cookies)
