# Webserv - A Lightweight HTTP Server

## Overview
Webserv is a custom lightweight HTTP server built from scratch, inspired by Nginx and Apache. It follows the HTTP/1.1 standard, supports CGI, and handles concurrent connections efficiently. The project is designed to deepen the understanding of networking, socket programming, and web server internals.

## Features
- **HTTP/1.1 Support**: Implements core HTTP methods such as GET, POST, DELETE.
- **Static & Dynamic Content**: Serves HTML files and supports CGI execution.
- **Multi-client Handling**: Uses non-blocking I/O with select/poll/epoll.
- **Error Handling**: Custom error pages for better user experience.
- **Configurable Server**: Reads and processes custom configuration files.
- **Logging**: Basic logging for debugging and monitoring.
- **Security Enhancements**: Implements request size limitations and header validations.

## Installation & Usage
```sh
# Clone the repository
git clone https://github.com/DivineSean/webserv.git
cd webserv

# Compile the server
make

# Run the server
./webserv config.conf
```

## Configuration
Webserv uses a configuration file similar to Nginx:
```nginx
server {
    listen 8080;
    server_name localhost;
    root /var/www/html;
    index index.html;
    error_page 404 /404.html;
}
```

## Resources
### Web Server & HTTP Basics
- [Web Server (Notion)](https://www.notion.so)
- [RFC 2616 - HTTP/1.1](https://datatracker.ietf.org/doc/html/rfc2616)
- [RFC 3875 - CGI](https://datatracker.ietf.org/doc/html/rfc3875)
- [HTTP Protocol Overview](https://developer.mozilla.org/en-US/docs/Web/HTTP)
- [Understanding HTTP using Browsers](https://www.tutorialspoint.com/http/index.htm)
- [HTTP Full Form and Explanation](https://www.geeksforgeeks.org/full-form-of-http/)
- [HTTP 2.0 - A Second Look](https://www.cloudflare.com/learning/performance/http2/)

### Networking & Socket Programming
- [What is Transmission Control Protocol (TCP)?](https://www.cloudflare.com/learning/network-layer/what-is-tcp/)
- [Types of Sockets](https://www.geeksforgeeks.org/socket-types/)
- [Socket Programming in C](https://www.geeksforgeeks.org/socket-programming-cc/)
- [Socket Programming in C++](https://www.geeksforgeeks.org/socket-programming-in-cpp/)
- [Example: Network Socket Programming](https://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html)
- [TCP Server-Client Implementation in C](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/)

### Building a Web Server
- [A Simple HTTP Server from Scratch](https://ruslanspivak.com/lsbaws/)
- [HTTP Server Explained](https://dev.to/pankajtanwarbanna/http-server-explained-2kg9)
- [Making a Simple HTTP Webserver in C](https://dzone.com/articles/building-a-simple-http-server-in-c)
- [Building a Simple Server with C++](https://www.youtube.com/watch?v=esXw4bdaZkc)
- [Building a Web Server in C++ (Video Series)](https://www.youtube.com/playlist?list=PLRwVmtr-pp05BvPTWGlR2i3VRm32qjlD0)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)

### Webserv & CGI
- [CGI Overview](https://www.tutorialspoint.com/http/http_cgi.htm)
- [The NGINX Crash Course (Video)](https://www.youtube.com/watch?v=3d0pDfrjmqM)
- [NGINX Internal Architecture - Workers (Video)](https://www.youtube.com/watch?v=LMl3pXIKRlA)
- [Mastering NGINX (Book)](https://www.amazon.com/Mastering-Nginx-Dimitri-Aivaliotis/dp/178216232X)
- [Nginx Cheat Sheet](https://www.datadoghq.com/blog/nginx-cheat-sheet/)
- [Nginx Location Match Tester](https://nginx.viraptor.info/)
- [How Sessions and Cookies Work](https://developer.mozilla.org/en-US/docs/Web/HTTP/Cookies)
