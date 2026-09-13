#pragma once

# include <string>
# include <map>
# include <poll.h>
# include "client.hpp"

class client;

class AMethodHandler
{
    public:
        virtual ~AMethodHandler();
        virtual void handle(client &c, struct pollfd &p) = 0;
};

class GetHandler : public AMethodHandler
{
    public:
        virtual void handle(client &c, struct pollfd &p);
};

class PostHandler : public AMethodHandler
{
    public:
        virtual void handle(client &c, struct pollfd &p);
};

class DeleteHandler : public AMethodHandler
{
    public:
        virtual void handle(client &c, struct pollfd &p);
};

class HandlerFactory
{
    public:
        static AMethodHandler *create(const std::string &method);
};
