# Linux development environment for the 42 Webserv project.
# The project source is NOT copied in — it is bind-mounted at runtime (see the
# wrapper Makefile), so any edit you make on the host appears instantly inside
# the container.
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Toolchain + the tools you actually use to test a web server.
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        clang \
        make \
        valgrind \
        gdb \
        python3 \
        php-cli \
        curl \
        siege \
        net-tools \
        procps \
        lsof \
        vim \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# The app Makefile invokes the compiler as `c++`. On Ubuntu `c++` is g++, which
# rejects this codebase (e.g. `server *server;`). The project is built with clang,
# so we make `c++` a thin clang++ wrapper. It also pins -std=c++17, because the
# code uses C++11/17 features (std::to_string, nullptr, <filesystem>) that the
# Makefile's -std=c++98 does not allow on Linux (Apple's clang tolerates them).
# To build exactly as the Makefile states instead: `rm /usr/local/bin/c++`.
RUN printf '%s\n' \
    '#!/bin/sh' \
    'exec clang++ "$@" -std=c++17' \
    > /usr/local/bin/c++ \
    && chmod +x /usr/local/bin/c++

WORKDIR /webserv

# Keep the container alive so you can `make bash` into it whenever you want.
CMD ["sleep", "infinity"]
