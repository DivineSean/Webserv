#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include "webserv.hpp"
#include "response.hpp"
#include "request.hpp"

class server;
class client
{
    private:
        struct pollfd         pfd;
        std::map <std::string, std::string> map;
    public:
        struct timeval        timestamp;
        request               Request;
        response              Response;
        server                *server;
        bool                  NotFound;
        int                   filefd;
        void                                requestParse( struct pollfd &p);
        void                                GenerateBody();
        struct pollfd                       getpFd() const;
        void                                setpFd(struct pollfd fd);
        void                                setMap(std::map <std::string, std::string> leMap);
        std::map <std::string, std::string> getMap() const;
        void                                openFileSuccess(std::string Path);
        void                                Parse_POST(std::string Path, std::string request_body);
        void                                process_chunked(std::string &chunk);
        void                                Parse_DELETE(std::string Path); 
        void                                process_chunked_CGI(std::string &request_body);
        void                                CGI_POST_normalcase(std::string &body);
       
};      