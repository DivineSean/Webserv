#include "server.hpp"

server::server()
{
    code404 =  "404";
    code400 =  "400";
    code501 =  "501";
    code405 =  "405";
    code200 =  "200";
    code413 =  "413";
    code411 =  "411";
    code403 =  "403";
    code414 =  "414";
    code408 =  "408";
    code500 =  "500";
    code201 =  "201";
    cookie_html = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>Login Page</title>\n<style>\nbody {\nfont-family: Arial, sans-serif;\nbackground-color: #f4f4f4;\nmargin: 0;\npadding: 0;\ndisplay: flex;\njustify-content: center;\nalign-items: center;\nheight: 100vh;\n}\n.container {\nbackground-color: #fff;\npadding: 20px;\nborder-radius: 8px;\nbox-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\nwidth: 300px;\n}\nh2 {\ntext-align: center;\nmargin-bottom: 20px;\n}\ninput[type=\"text\"],\ninput[type=\"password\"] {\nwidth: 100%;\npadding: 10px;\nmargin-bottom: 10px;\nborder: 1px solid #ccc;\nborder-radius: 4px;\nbox-sizing: border-box;\n}\na[id=\"hihi\"] {\nwidth: 100%;\nfont-size: 15px;\nbackground-color: #007bff;\ncolor: #fff;\npadding: 4px;\nborder: none;\nborder-radius: 4px;\ncursor: pointer;\n}\ninput[type=\"submit\"]:hover {\nbackground-color: #0056b3;\n}\n</style>\n</head>\n<body>\n<div class=\"container\">\n<h2>Login</h2>\n<label for=\"username\">Username:</label>\n<input type=\"text\" id=\"username\" name=\"username\" placeholder=\"Enter your username\" required>\n<label for=\"password\">Password:</label>\n<input type=\"password\" id=\"password\" name=\"password\" placeholder=\"Enter your password\" required>\n<a id = \"hihi\" href=";
    cookie_html2= ">Sumbit</a>\n</div>\n</body>\n<script>\ndocument.getElementById(\"hihi\").addEventListener(\"click\", function(event) {\nvar username = document.getElementById(\"username\").value;\nvar password = document.getElementById(\"password\").value;\nif (!username || !password) {\nevent.preventDefault(); // Prevent form submission\nalert(\"Please fill in all fields.\");\n}\n});\n</script>\n</html>";

}

std::string                 server::getmimeDefaul() const
{
    return (this->mime_default);
}

void                        server::setmimeDefault(const std::string a)
{
    this->mime_default = a;
}

std::vector<int>            server::getFd() const
{
    return (fd);
}

void                        server::setFd(std::vector<int> fd)
{
    this->fd = fd;
}

std::vector<std::string>    server::getmimeTypes() const
{
    return(this->mime_types);
}

void                        server::setmimeTypes(const std::vector<std::string> b)
{
    this->mime_types = b;
}

void  server::setUploadPath(const std::string UploadPath)
{
    if (UploadPath.find(",") != std::string::npos)
    {
        std::cerr << "[UploadPath] Accepte Only One Value" << std::endl;
        exit(1);
    }
    this->uploadPath = UploadPath;
}

std::string  server::getUploadPath() const
{
    return (this->uploadPath);
}

std::vector<std::string>    server::getServers_names() const
{
    return (servers_names);
}

std::vector<std::string>    server::getMethodesAllowed() const
{
    return (MethodesAllowed);
}

std::vector<std::string>    server::getError_pages() const
{
    return (error_pages);
}

std::vector<location>       server::getLocations() const
{
    return (locations);
}

long long                   server::getMax_body_size() const
{
    return (max_body_size);
}

std::vector<std::string>    server::getIndex() const
{
    return (index);
}

std::string                 server::getHost() const
{
    return (host);
}

std::vector<std::string>    server::getPort() const
{
    return (port);
}

std::string                 server::getRoot() const
{
    return (root);
}

bool                        server::getAutoindex() const
{
    return (autoindex);
}

bool                        server::getupload() const
{
    return (upload);
}

void                        server::setServers_names(const std::vector<std::string> servers_names)
{
    this->servers_names = servers_names;
}

void                        server::setMethodesAllowed(const std::vector<std::string> MethodesAllowed)
{
    for (std::vector<std::string>::const_iterator it = MethodesAllowed.begin(); it != MethodesAllowed.end(); ++it)
    {
        if (*it != "GET" && *it != "POST" && *it != "DELETE")
        {
            std::cerr << "[ALLOW METHODS] '" << *it << "' is Invalid Method !!" << std::endl;
            exit(1);
        }
    }
    this->MethodesAllowed = MethodesAllowed;
}

std::string Code_PathReturn(std::string s)
{
    const char *str = s.c_str();
    int i = 0;
    size_t num = 0;
    if (!std::isdigit(str[0]))
    {
        std::cerr << "Error Page StatsCode UNVAILABLE" << std::endl;
        exit(1);
    }
    while(str[i])
    {
        if (!std::isdigit(str[i]))
        {
            if (num != 301 && num != 400 && num != 404 && num != 405 && num != 408 && num != 411 && num != 413 && num != 414 && num != 501 && num != 502 && num != 503 && num != 505)
            {
                std::cerr << "Error Page StatsCode " << num << " is not in Range" << std::endl;
                exit(1);
            }
            return (s.substr(i + 1));
        }
        num = (num * 10) + (str[i] - 48);
        i++;
    }
    return (s);
}

bool fileExists(const char* filename)
{
    std::ifstream file(filename);
    bool isOpen = file.is_open();
    file.close();
    return isOpen;
}

bool repooExists(const char* filename)
{
    DIR* isOpen = opendir(filename);
    if(isOpen != NULL)
    {
        closedir(isOpen);
        return true;
    }
    return false;
}

void                        server::setError_pages(const std::vector<std::string> error_pages)
{
    std::string data;
    std::string s;
    for (std::vector<std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
    {
        s = Code_PathReturn(*it);
        if (s.empty())
        {
            std::cerr << "Error Page in " << *it << " [PATH] not Defined" << std::endl;
            exit(1);    
        }
        if (!fileExists(s.c_str())) 
        {
            std::cerr << "Error Page in " << s << " [File] Not Exist" << std::endl;
            exit(1);  
        }
        if (repooExists(s.c_str()))
        {
            std::cerr << "Error Page in " << s << " [Directory] Not Allowed" << std::endl;
            exit(1);  
        }
    }
    for (std::vector<std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
    {
        std::istringstream txt(*it);
        std::string s2;
        getline(txt, s2, '=');
        if (data.find(s2) != std::string::npos)
        {
            std::cerr << "Same Error code in Error Page [" << s2 << "] Defined" << std::endl;
            exit(1);    
        }
        data += s2 + " ";
    }
    data.clear();
    this->error_pages = error_pages;
}

void                        server::setLocations(const std::vector<location> locations)
{
    size_t i = 0;
    size_t j;
    while(i < locations.size())
    {
        j = i + 1;
        while(j < locations.size())
        {
            if (locations[i]._getPathLocation() == locations[j]._getPathLocation())
            {
                std::cerr << "[WARNING] LOCATION PATH '" << locations[i]._getPathLocation() << "' DUPLICATED" << std::endl;
                exit(1);
            }
            j++;
        }
        i++;
    }
    this->locations = locations;
}

bool    isdegit(char c)
{
    return (c >= '0' && c <= '9');
}

void                        server::setMax_body_size(const std::string m)
{
    size_t i  = 0;
    size_t converter = 1;
    long long num = 0;
    const char *s = m.c_str();
    if (m[m.size() - 1] != 'M' && m[m.size() - 1] != 'G' && m[m.size() - 1] != 'K' && m[m.size() - 1] != 'B')
    {
        std::cerr << "[Server '" << this->servers_names[0] << "' Max Body Size] Should be Degit With [G='Go'] [M ='Mo'] [K='Ko'] [B='Byte']" << std::endl;
        exit(1);
    }
    while(i < m.size() - 1)
    {
        if (!isdegit(s[i]))
        {
            std::cerr << "[Server '" << this->servers_names[0] << "' Max Body Size] Should be Degit With One char Of This [G='Go'] [M ='Mo'] [K='Ko'] [B='Byte']" << std::endl;
            exit(1);
        }
        num = (num * 10) + (s[i] - 48);
        i++;
    }
    if (s[m.size() - 1] == 'M')
        converter = 1048576;
    else if (s[m.size() - 1] == 'G')
        converter = 1073741824;
    else if (s[m.size() - 1] == 'K')
        converter = 1024;
    else if (s[m.size() - 1] == 'B')
        converter = 1;
    this->max_body_size = num * converter;
}

void                        server::setAutoindex(const bool autoindex)
{
    this->autoindex = autoindex;
}

void                        server::setupload(const bool upload)
{
    this->upload = upload;
}

void                        server::setIndex(const std::vector<std::string> index)
{
    this->index = index;
}

bool IPCorrect(std::string s)
{
    const char *c = s.c_str();
    size_t i = 0;
    size_t num = 0;
    while(i < s.size())
    {
        num = (num * 10) + (c[i] - 48);
        i++; 
    }
    return (num <= 255);
}

void                        server::setHost(const std::string host)
{
    std::istringstream txt(host);
    std::string s;
    size_t tracker = 0;
    getline(txt, s, ' ');
    if (getline(txt, s, ' ') || s.find("..") != std::string::npos)
    {
        std::cerr << "[WARNING] '" << this->servers_names[0] << "' HOST NOT VALID" <<std::endl;
        exit(1);
    }
    if (s == "\"localhost\"")
    {
        this->host = "127.0.0.1";
        return ;
    }
    const char *v = s.c_str();
    size_t i = 0;
    if (s.size())
    {
        if (v[0] == '.' || v[s.size() - 1] == '.')
        {
            std::cerr << "[WARNING] '" << this->servers_names[0] << "' HOST NOT VALID" <<std::endl;
            exit(1);
        }
        if (v[0] == '"' && v[s.size() - 1] == '"')
        {
            this->host = s;
            return ;
        }
    }
    while(i < s.size())
    {
        if ((!isdegit(v[i]) && v[i] != '.') || tracker > 3)
        {
            std::cerr << "[WARNING] '" << this->servers_names[0] << "' HOST NOT VALID" <<std::endl;
            exit(1);
        }
        if (v[i] == '.')
            tracker++;
        i++;
    }
    std::istringstream Ch(s);
    std::string b;
    while(getline(Ch, b, '.'))
    {
        if (!IPCorrect(b))
        {
            std::cerr << "[WARNING] '" << this->servers_names[0] << "' HOST NOT VALID" <<std::endl;
            exit(1);
        }
    }
    this->host = s;
}

bool PortProtection(std::string string)
{
    const char *s = string.c_str();
    size_t i = 0;
    long long num = 0;

    while(s[i])
    {
        if (!isdegit(s[i]))
        {
            std::cerr << "[Listen] Invalid Port Input [" << string << " ] Should Not Have '" << s[i] << "'" << std::endl;
            exit(1);
        }
        num = (num * 10) + (s[i] - 48);
        i++;
    }
    if (num < 1024 || num > 65535)
    {
        std::cerr << "[Listen] Invalid Port Number [" << num << "]. Port number must be between 1024 and 65535." << std::endl;
        exit(1);
    }
    return (true);
}

void                        server::setPort(const std::vector<std::string> port)
{
    size_t i = 0;
    size_t j;
    while(i < port.size() && PortProtection(port[i]))
    {
        j = i + 1;
        while(j < port.size())
        {
            if (port[i] == port[j])
            {
                std::cerr << "[WARNING] Server Listen Port '" << port[i] << "' DUPLICATED" << std::endl;
                exit(1);
            }
            j++;
        }
        i++;
    }
    this->port = port;
}

void                        server::setRoot(const std::string root)
{
    if (root.find(",") != std::string::npos)
    {
        std::cerr << "[root] Accepte Only One Value" << std::endl;
        exit(1);
    }
    this->root = root;
}

std::vector<std::string>    server::getcginfo() const
{
    return (this->cgi_info);
}

std::vector<std::string>    server::getcginfoPHP() const
{
    return (this->cgi_info_PHP);
}

void                        server::setcginfo(const std::string s)
{
    std::vector<std::string> v;
    if (s.empty())
        return ;
    std::string cgi_info = s;
    size_t index = cgi_info.find(",");
    if (cgi_info.find(32) == std::string::npos || index != std::string::npos)
    {
        if (index != std::string::npos)
        {
            std::string b = cgi_info;
            cgi_info.erase(index);
            TrimWhitespace(cgi_info);
            setcginfo(cgi_info);
            b = b.substr(index + 1, b.size());
            TrimWhitespace(b);
            setcginfo(b);
            return ;
        }
        else
        {
            std::cerr << "Unknown [" << cgi_info << "]. cgi_info input should behave like this : '[type(space)compiler(space)cgi_path]'" << std::endl;
            exit(1);
        }
    }
    std::istringstream text(cgi_info);
    std::string Buffer;
    getline(text, Buffer, ' ');
    if (Buffer.find("*.py") == std::string::npos || Buffer.size() != 4)
    {
        if (Buffer.find("*.php") == std::string::npos || Buffer.size() != 5)
        {
            std::cerr << "Unsuported Types '" << Buffer << "' We support Only *.py and *.php Types" << std::endl;
            exit(1); 
        }
    }
    if ((Buffer[Buffer.size() - 1] == 'y' && Buffer[Buffer.size() - 2] == 'p' && Buffer[Buffer.size() - 3] == '.') || (Buffer[Buffer.size() - 1] == 'p' && Buffer[Buffer.size() - 2] == 'h' && Buffer[Buffer.size() - 3] == 'p' && Buffer[Buffer.size() - 4] == '.'))
    {
        v.push_back(Buffer);
        if (!getline(text, Buffer, ' '))
        {
            std::cerr << "Unknown [" << cgi_info << "]. cgi_info input should behave like this : '[type(space)compiler(space)cgi_path]'" << std::endl;
            exit(1);
        }
        v.push_back(Buffer);
        if (!getline(text, Buffer, ' '))
        {
            std::cerr << "Unknown [" << cgi_info << "]. cgi_info input should behave like this : '[type(space)compiler(space)cgi_path]'" << std::endl;
            exit(1);
        }
        v.push_back(Buffer);
        if (getline(text, Buffer, ' '))
        {
            std::cerr << "Unknown [" << cgi_info << "]. cgi_info input should Accepte 3 Option like this : '[type(space)compiler(space)cgi_path]'" << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cout << Buffer << std::endl;
        std::cerr << "Unsuported Types '" << Buffer << "' We support Only *.py and *.php Types" << std::endl;
        exit(1); 
    }
    if (v[0] == "*.py")
        this->cgi_info= v;
    else if (v[0] == "*.php")
        this->cgi_info_PHP = v;
}

server::~server(){}