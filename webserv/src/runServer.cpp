#include "client.hpp"
#include "webserv.hpp"
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sys/poll.h>
#include <sys/select.h>

struct pollfd                  client::getpFd() const
{
    return (pfd);
}

void                  client::setpFd(struct pollfd pfd)
{
    this->pfd = pfd;
}

int webserv::getSocket(int sock)
{
  for (int i = 0; (size_t)i < pollfds.size(); i++)
  {
    if (pollfds[i].fd == sock)
      return (i);
  }
  return -1;
}

void webserv::writeToClient(int sock)
{
  client &c = getClient(sock);
  if (c.NotFound == true)
    return;
  c.GenerateBody();
  int WriteCount;
  WriteCount = write(sock, c.Response.responseToClient.c_str(), c.Response.responseToClient.size());
  if (WriteCount <= 0)
  {
    int CIndex = getClientIndex(sock);
    int index = getSocket(sock);
    if (index == -1)
      return;
    if (CIndex != -1)
      clients.erase(clients.begin() + CIndex);
    pollfds.erase(pollfds.begin() + index);
    close(sock);
    return ;
  }
  gettimeofday(&c.timestamp, NULL);
  c.Response.count = 0;
  if (c.Response.SetupOffset == true)
    c.Response.readOffset += WriteCount;
  else
    c.Response.readOffset += WriteCount - c.Response.HeadSize;
  c.Response.responseToClient.clear();
  if (c.Response.isDone == true)
  {
    int CIndex = getClientIndex(sock);
    int index = getSocket(sock);
    if (index == -1)
      return;
    if (CIndex != -1)
      clients.erase(clients.begin() + CIndex);
    pollfds.erase(pollfds.begin() + index);
    close(sock);
  }
}

int webserv::getClientIndex(int sock)
{
  for (int i = 0; (size_t)i < clients.size(); i++)
  {
    if (clients[i].getpFd().fd == sock)
      return (i);
  }
  return -1;
}

client &webserv::getClient(int sock)
{
  int i ;
  for (i = 0; (size_t)i < clients.size(); i++)
  {
    if (clients[i].getpFd().fd == sock)
    {
      clients[i].NotFound = false;
      return (clients[i]);
    }
  }
  clients[i - 1].NotFound = true;
  return clients[i - 1];
}

void    client::setMap(std::map <std::string, std::string> leMap)
{
  this->map = leMap;
}

std::map <std::string, std::string> client::getMap() const
{
  return (map);
}

void ft_memset(void *s, int c, size_t n)
{
  size_t i = 0;
  unsigned char *str = (unsigned char *)s;
  while (i < n)
  {
    str[i] = c;
    i++;
  }
}

void webserv::readFromClient(int sock)
{
    long long tracker;
    client &c = getClient(sock);
    if (c.NotFound == true)
        return;
    char Buffer[1024];
    ft_memset(Buffer, '\0', 1024);
    c.Request.valRead = read(sock, Buffer, 1024);
    if (c.Request.valRead == -1)
    {
      int CIndex = getClientIndex(sock);
      int index = getSocket(sock);
      if (index == -1)
        return;
      if (CIndex != -1)
        clients.erase(clients.begin() + CIndex);
      pollfds.erase(pollfds.begin() + index);
      close(sock);
      return ;
    }
    if (c.Request.valRead == 0)
      ;
    gettimeofday(&c.timestamp, NULL);
    if (c.Request.isHead == true)
    {
      tracker = atol_l(c.getMap()["Content-Length"].c_str());
      if (tracker == -1)
      {
        int CIndex = getClientIndex(sock);
        int index = getSocket(sock);
        if (index == -1)
          return;
        if (CIndex != -1)
          clients.erase(clients.begin() + CIndex);
        pollfds.erase(pollfds.begin() + index);
        close(sock);
        return ;
      }
      if (tracker == (long long)(c.Request.ourTracker + c.Request.valRead) && c.getMap()["Transfer-Encoding"] != "chunked")
        c.Request.isBody = true;
    }
    if (!c.Request.isHead)
    {
        std::string s;
        s.append(Buffer, c.Request.valRead);
        size_t endOfHead = s.find("\r\n\r\n");
        if (endOfHead != std::string::npos)
        {
            c.Request.Head.append(s.substr(0, endOfHead + 4));
            c.Request.Body.append(s.substr(endOfHead + 4, s.size()));
            c.Request.ourTracker += c.Request.Body.size();
            c.Request.isHead = true;
        }
        else
            c.Request.Head.append(s);
    }
    else
    {
        c.Request.ourTracker += c.Request.valRead;
        c.Request.Body.clear();
        c.Request.Body.append(Buffer, c.Request.valRead);
    }
    c.requestParse(pollfds[getSocket(sock)]);
    c.setpFd(pollfds[getSocket(sock)]);
    gettimeofday(&c.timestamp, NULL);
}

server *webserv::FindServer(int sock)
{
  size_t i = 0;
  size_t j;
  while(i < servers.size())
  {
    j = 0;
    while(j < servers[i].getFd().size())
    {
      if (servers[i].getFd()[j] == sock)
        return (&servers[i]);
      j++;
    }
    i++;
  }
  return (NULL);
}

void webserv::addClient(int sock)
{
  struct sockaddr_in clientAddr;
  struct pollfd pfd;
  client s;
  int len = sizeof(sockaddr_in);
  int newSocket = accept(sock, (struct sockaddr *)&clientAddr, (socklen_t *)&len);
  if (newSocket <= 0)
    return ;
  fcntl(newSocket, F_SETFL, O_NONBLOCK);
  pfd.fd = newSocket;
  pfd.events = POLLIN;
  pollfds.push_back(pfd);
  s.server = FindServer(sock);
  s.setpFd(pfd);
  s.Request.isBody = false;
  s.Request.isHead = false;
  s.Request.is_already_checked = false;
  s.Response.isDone = false;
  s.Response.WriteBody = false;
  s.Response.WriteHead = false;
  s.Response.SetupOffset = false;
  s.Response.sizeIsdone = true;
  s.Response.size_length = 0;
  s.Response.insideS = false;
  s.Request.ourTracker = 0;
  s.Response.readOffset = 0;
  s.Response.count = 0;
  s.Response.HeadSize = 0;
  gettimeofday(&s.timestamp, NULL);
  clients.push_back(s);
}

std::vector<struct pollfd> CreatServers(std::vector <server> &s, size_t &l)
{
  size_t len = s.size();
  size_t i = 0;
  size_t j;
  std::vector<struct pollfd> pollfds;
  std::vector<int> fd;
  struct pollfd pfd;
  
  while(i < len)
  {
    j = 0;
    while(j < s[i].getPort().size())
    {
      pfd.fd = socket(AF_INET, SOCK_STREAM, 0);
      if (pfd.fd == -1)
      {
        std::cerr << "Error: socket creation failed" << std::endl;
        exit (1);
      }
      pfd.events = POLLIN;
      fcntl(pfd.fd, F_SETFL, O_NONBLOCK);
      int len = 1;
      setsockopt(pfd.fd, SOL_SOCKET, SO_REUSEADDR, &len, sizeof(len));
      struct sockaddr_in server;
      server.sin_family = AF_INET;
      server.sin_port = htons(atoi(s[i].getPort()[j].c_str()));
      server.sin_addr.s_addr = inet_addr(s[i].getHost().c_str());
      if (bind(pfd.fd, (struct sockaddr *)&server, sizeof(server)) < 0)
      {
        std::cerr << "Error: bind failed" << std::endl;
        exit (1);
      }
      if (listen(pfd.fd, SOMAXCONN) < 0) {
        std::cerr << "Error: listen failed" << std::endl;
        exit (1);
      }
      pollfds.push_back(pfd);
      fd.push_back(pfd.fd);
      j++;
    }
    s[i].setFd(fd);
    fd.clear();
    i++;
  }
  l = pollfds.size();
  return (pollfds);
}

void  webserv::ClientsTimeout()
{
  struct timeval now;
  gettimeofday(&now, NULL);
  for (size_t i = 0; i < clients.size(); i++)
  {
    if ((size_t)(now.tv_sec - clients[i].timestamp.tv_sec) > 10)
    {
      clients[i].getMap().clear();
      std::map<std::string, std::string> leMap;
      leMap["Status_Code"] = "408";
      clients[i].setMap(leMap);
      struct pollfd pfd = clients[i].getpFd();
      pfd.events = POLLOUT;
      clients[i].setpFd(pfd);
      size_t index = getSocket(pfd.fd);
      pollfds[index] = pfd;
    }
  }
}

void webserv::runServer()
{
  this->pollfds = CreatServers(this->servers, this->svSocketCount);
  while (true)
  {
    if (poll(&pollfds[0], pollfds.size(), -1) > 0)
    {
      for (int i = 0; (size_t)i < pollfds.size(); i++)
      {
        if (pollfds[i].revents & POLLIN)
        {
          size_t fdPosition = pollfds[i].fd - 3;
          if (fdPosition < svSocketCount)
            addClient(pollfds[i].fd);
          else
            readFromClient(pollfds[i].fd);
        }
        else if (pollfds[i].revents & POLLOUT)
          writeToClient(pollfds[i].fd);
      }
    }
    else
    {
      std::cerr << "Error: poll failed" << std::endl;
      exit(1);
    }
    ClientsTimeout();
  }
  return ;
}
