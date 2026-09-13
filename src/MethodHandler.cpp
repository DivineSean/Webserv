#include "MethodHandler.hpp"
#include "webserv.hpp"
#include <unistd.h>

AMethodHandler::~AMethodHandler() {}

void GetHandler::handle(client &c, struct pollfd &p)
{
    c.openFileSuccess(c.mapRef()["Request_Path"]);
    p.events = POLLOUT;
}

void DeleteHandler::handle(client &c, struct pollfd &p)
{
    c.Parse_DELETE(c.mapRef()["Request_Path"]);
    c.Response.isDone = true;
    p.events = POLLOUT;
}

void PostHandler::handle(client &c, struct pollfd &p)
{
    std::map<std::string, std::string> &map = c.mapRef();

    if (map["Transfer-Encoding"] != "chunked" && !map["Transfer-Encoding"].empty())
    {
        if (map["Content-Length"].empty())
        {
            map["Status_Code"] = c.server->code411;
        }
        else if ((atol_l(map["Content-Length"]) == -1))
        {
            if (atol_l(map["Content-Length"]) == -1)
                map["Status_Code"] = c.server->code400;
            else
                map["Status_Code"] = c.server->code413;
        }
        else
            map["Status_Code"] = c.server->code501;
        c.Response.isDone = true;
        p.events = POLLOUT;
        return ;
    }
    else if (map["Transfer-Encoding"].empty() && map["Content-Length"].empty())
    {
        map["Status_Code"] = c.server->code411;
        c.Response.isDone = true;
        p.events = POLLOUT;
        return ;
    }
    bool is = false;
    if (c.Request.isBody == false)
    {
        c.Parse_POST(map["Request_Path"], c.Request.Body);
        if (map["Status_Code"] != c.server->code200 && map["Status_Code"] != c.server->code201)
        {
            if (c.filefd != 0)
                close(c.filefd);
            c.Response.isDone = true;
            p.events = POLLOUT;
        }
        is = true;
    }
    if (c.Request.isBody == true)
    {
        if (is != true)
        {
            int f = write(c.filefd, c.Request.Body.c_str(), c.Request.Body.size());
            if (f == -1)
            {
                map["Status_Code"] = c.server->code500;
                c.Response.isDone = true;
                p.events = POLLOUT;
            }
            else if (f == 0)
                ;
        }
        close(c.filefd);
        c.Response.isDone = true;
        p.events = POLLOUT;
    }
}

AMethodHandler *HandlerFactory::create(const std::string &method)
{
    if (method == "GET")
        return new GetHandler();
    if (method == "POST")
        return new PostHandler();
    if (method == "DELETE")
        return new DeleteHandler();
    return NULL;
}

std::map<std::string, std::string> &client::mapRef()
{
    return map;
}
