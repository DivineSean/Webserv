#pragma once
# include "server.hpp"
# include "client.hpp"
class server;
class location;
class client;
class webserv
{
    private:
        std::vector<server> servers;
        std::vector<client> clients;
        std::vector<struct pollfd> pollfds;
        std::vector<std::string> ConfigContent;
        size_t                   svSocketCount;
    public:
        webserv(const std::vector<std::string>& ConfigContent);
        server FillingServer( const  std::string ConfigContent );
        void    ClientsTimeout();
        std::vector<std::string> getContent() const;
        std::vector<server> getServers() const;
        void    addClient(int sock);
        void    readFromClient(int sock);
        server  *FindServer(int sock);
        client  &getClient(int sock);
        int     getClientIndex(int sock);
        void    writeToClient(int sock);
        int     getSocket(int sock);
        void    runServer();
        ~webserv();
};
std::string Finder(const std::string KeyWord, std::string ConfigContent);
std::vector<std::string> Finder_vector(const std::string KeyWord, std::string ConfigContent);
std::vector<location> FindLocation(const std::string KeyWord, std::string ConfigContent, const server &s);
bool FindAutoIndex(const std::string KeyWord, std::string ConfigContent);
std::string Default(const std::string KeyWord);
std::vector<std::string> DefaultVector(const std::string KeyWord);
void    TrimWhitespace(std::string& input);
std::string Code_PathReturn(std::string s);
bool fileExists(const char* filename);
long long atol_l(std::string s);
std::string RWord(std::string Buffer, std::string replace, size_t i, size_t len);
std::string FindMimeDefault(std::string KeyWord, std::string ConfigContent);
void    Mimetypedup(std::string s, int status, std::vector<std::string> &types);
size_t SpaceCalculation(std::string s);
bool repooExists(const char* filename);
char *StringCharToChar(std::string s1, char *s2, size_t lenOFs2);
std::vector<std::string> FillingDefaultMimeTypes();
std::vector<std::string> FillingMimeTypes(std::vector<std::string> s);