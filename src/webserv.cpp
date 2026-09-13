#include "webserv.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

bool    CheckNames(std::vector <std::string> NamesS1, std::vector <std::string> NamesS2)
{
    size_t i = 0;
    size_t j;
    while(i < NamesS1.size())
    {
        j = 0;
        while(j < NamesS2.size())
        {
            if (NamesS1[i] == NamesS2[j])
                return (false);
            j++;
        }
        i++;
    }
    return (true);
}

long long atol_l(std::string s)
{
    long long num = 0;
    size_t i = 0;
    while(i < s.size())
    {
        if (!isdegit(s[i]))
            return (-1);
        num = (num * 10) + (s[i] - 48);
        i++;
    }
    return (num);
}

void    DuplicateGateCheker(std::vector<server> LeServers)
{
    size_t i = 0;
    size_t j;
    size_t len = LeServers.size();
    while(i < len)
    {
        j = i + 1;
        while(j < len)
        {
            if (LeServers[i].getHost() == LeServers[j].getHost() && LeServers[i].getPort() == LeServers[j].getPort())
            {
                if (!CheckNames(LeServers[i].getServers_names(), LeServers[j].getServers_names()))
                {
                    std::cerr << "Server [" << i << "] And [" << j << "] HAVE SAME HOST/LISTEN/SERVERNAME" << std::endl;
                    exit(1);
                }
                else
                {
                    std::cerr << "Server [" << i << "] And [" << j << "] HAVE SAME HOST/PORT" << std::endl;
                    exit(1);
                }
            }
            j++;
        }
        i++;
    }
}

webserv::webserv(const std::vector<std::string>& ConfigContent)
{
    if (ConfigContent.empty())
        servers.push_back(FillingServer(""));
    else
        for (std::vector<std::string>::const_iterator it = ConfigContent.begin(); it != ConfigContent.end(); ++it)
            {servers.push_back(FillingServer(*it));}//std::cout << *it << std::endl;}exit(1);
    DuplicateGateCheker(servers);
}

std::vector<std::string> DefaultVector(const std::string KeyWord)
{
    std::vector<std::string> v;
    if (KeyWord == "server_names ")
        v.push_back("webserv");
    else if (KeyWord == "index ")
    {
        v.push_back("index.html");
        v.push_back("index.htm");
    }
    else if (KeyWord == "error_pages ")
    {
        v.push_back("301=www/error_pages/301.html");
        v.push_back("400=www/error_pages/400.html");
        v.push_back("404=www/error_pages/404.html");
        v.push_back("405=www/error_pages/405.html");
        v.push_back("408=www/error_pages/408.html");
        v.push_back("411=www/error_pages/411.html");
        v.push_back("413=www/error_pages/413.html");
        v.push_back("414=www/error_pages/414.html");
        v.push_back("501=www/error_pages/501.html");
        v.push_back("502=www/error_pages/502.html");
        v.push_back("503=www/error_pages/503.html");
        v.push_back("505=www/error_pages/505.html");
    }
    else if (KeyWord == "listen ")
        v.push_back("8080");
    else if (KeyWord == "allow ")
        v.push_back("GET");
    return (v);
}

std::string Default(const std::string KeyWord)
{
    if (KeyWord == "host ")
        return ("127.0.0.1");
    else if (KeyWord == "max_body_size ")
        return ("1M");
    else if (KeyWord == "root ")
        return ("www/Indexes");
    else if (KeyWord == "path_upload ")
        return ("www/UploadingFiles");
    else if (KeyWord == "cgi_info ")
        return ("");
    return ("");
}

bool isSpace(char ch)
{
    if (std::isspace(static_cast<unsigned char>(ch)) != 0)
        return true;
    return false;
}

void My_Unique(std::string &input)
{
    size_t i = 0;
    while(i < input.size())
    {
        if (isSpace(input[i]))
        {
            i++;
            while (i < input.size() && isSpace(input[i]))
                input.erase(input.begin() + i);
        }
        i++;
    }
}

void    TrimWhitespace(std::string& input)
{
    My_Unique(input);
    if (isSpace(input[0]))
        input.erase(input.begin());
    if (input.size())
    {
        if (isSpace(input[input.size() - 1]))
            input.erase(input.size() - 1);
    }
}

std::string Finder(std::string KeyWord, std::string ConfigContent)
{
    std::istringstream Texte(ConfigContent);
    std::string line;

    while (getline(Texte, line))
    {
        size_t i = line.find(KeyWord);
        if (i == 0)
        {
            std::string a = line.substr(KeyWord.size());
            a.erase(a.size() - 1);
            TrimWhitespace(a);
            return (a);
        }
    }
    return (Default(KeyWord));   
}

std::vector<std::string> Finder_vector(std::string KeyWord, std::string ConfigContent)
{
    std::istringstream Texte(ConfigContent);
    std::vector<std::string> v;
    std::string line;

    while (getline(Texte, line))
    {
        size_t i = line.find(KeyWord);
        if (i == 0)
        {
            line = line.substr(KeyWord.size());
            line.erase(line.size() - 1);
            TrimWhitespace(line);
            size_t comma = line.find(',');
            if (comma == std::string::npos)
            {
                v.push_back(line);
                return (v);
            }
            else
            {
                std::istringstream t(line);
                std::string buffer;
                while (getline(t, buffer, ','))
                {
                    TrimWhitespace(buffer);
                    v.push_back(buffer);
                }
                return (v);
            }
        }
    }
    return (DefaultVector(KeyWord));
}

std::vector<location> FindLocation(const std::string KeyWord, std::string ConfigContent, const server &s)
{
    std::istringstream Texte(ConfigContent);
    std::string line;
    location l;
    location tmp(s);
    std::vector<location> L;
    std::vector<std::string> n;
    std::string b;
    size_t comma;

    while (getline(Texte, line))
    {
        size_t i = line.find(KeyWord);
        if (i == 0)
        {
            l = tmp;
            line = line.substr(KeyWord.size());
            std::istringstream t(line);
            std::string buffer;
            getline(t, buffer, ' ');
            l._setPathLocation(buffer);
            while (getline(t, buffer, ' '))
            {
                n.clear();
                if (buffer == "root")
                {
                    getline(t, buffer, ';');
                    if (buffer.find(",") != std::string::npos)
                    {
                        std::cerr << "[root] Accept Only One Value" << std::endl;
                        exit(1);
                    }
                    TrimWhitespace(buffer);
                    l._setRoot(buffer);
                }
                else if (buffer == "cgi_info")
                {
                    getline(t, buffer, ';');
                    l._setclear();
                    TrimWhitespace(buffer);
                    l._setcginfoS(buffer);
                }
                else if(buffer == "path_upload")
                {
                    getline(t, buffer, ';');
                    if (buffer.find(",") != std::string::npos)
                    {
                        std::cerr << "[uploadpath] Accept Only One Value" << std::endl;
                        exit(1);
                    }
                    TrimWhitespace(buffer);
                    l._setUploadPath(buffer);
                }
                else if (buffer == "index")
                {
                    getline(t, buffer, ';');
                    comma = buffer.find(',');
                    if (comma == std::string::npos)
                        n.push_back(buffer);
                    else
                    {
                        std::istringstream f(buffer);
                        while (getline(f, b, ','))
                        {
                            comma = b.find(';');
                            if (comma != std::string::npos)
                                b.erase(b.size() - 1);
                            TrimWhitespace(b);
                            n.push_back(b);
                        }
                    }
                    l._setIndex(n);
                }
                else if (buffer == "allow")
                {
                    getline(t, buffer, ' ');
                    comma = buffer.find(',');
                    if (comma == std::string::npos)
                        n.push_back(buffer.erase(buffer.size() - 1));
                    else
                    {
                        std::istringstream f(buffer);
                        while (getline(f, b, ','))
                        {
                            comma = b.find(';');
                            if (comma != std::string::npos)
                                b.erase(b.size() - 1);
                            TrimWhitespace(b);
                            n.push_back(b);
                        }
                    }
                    l._setMethodesAllowed(n);
                }
                else if (buffer == "error_pages")
                {
                    getline(t, buffer, ' ');
                    comma = buffer.find(',');
                    if (comma == std::string::npos)
                        n.push_back(buffer.erase(buffer.size() - 1));
                    else
                    {
                        std::istringstream f(buffer);
                        while (getline(f, b, ','))
                        {
                            comma = b.find(';');
                            if (comma != std::string::npos)
                                b.erase(b.size() - 1);
                            TrimWhitespace(b);
                            n.push_back(b);
                        }
                    }
                    l._setError_pages(n);        
                }
                else if (buffer == "max_body_size")
                {
                    getline(t, buffer, ' ');
                    buffer.erase(buffer.size() - 1);
                    TrimWhitespace(buffer);
                    l._setMax_body_string(buffer);
                }
                else if (buffer == "autoindex")
                {
                    getline(t, buffer, ' ');
                    if (buffer.find(";") == std::string::npos)
                    {
                        std::cerr << "[AutoIndex] Accept Only OFF or ON" << std::endl;
                        exit(1);
                    }
                    buffer.erase(buffer.size() - 1);
                    TrimWhitespace(buffer);
                    if (buffer == "on")
                        l._setAutoindex(ON);
                    else if (buffer == "off")
                        l._setAutoindex(OFF);
                    else
                    {
                        std::cerr << "[AutoIndex] Location Invalid Option '" << buffer << "'. Accepte Only [on] or [off]" << std::endl;
                        exit(1); 
                    }
                }
                else if (buffer == "upload")
                {
                    getline(t, buffer, ' ');
                    if (buffer.find(";") == std::string::npos)
                    {
                        std::cerr << "[upload] Accept Only OFF or ON" << std::endl;
                        exit(1);
                    }
                    buffer.erase(buffer.size() - 1);
                    TrimWhitespace(buffer);
                    if (buffer == "on")
                        l._setupload(ON);
                    else if (buffer == "off")
                        l._setupload(OFF);
                    else
                    {
                        std::cerr << "[upload] Location Invalid Option '" << buffer << "'. Accepte Only [on] or [off]" << std::endl;
                        exit(1); 
                    }
                }
                else if (buffer == "return")
                {
                    getline(t, buffer, ';');
                    comma = buffer.find(',');
                    if (comma == std::string::npos)
                        n.push_back(buffer);
                    else
                    {
                        std::istringstream f(buffer);
                        while (getline(f, b, ','))
                        {
                            comma = b.find(';');
                            if (comma != std::string::npos)
                                b.erase(b.size() - 1);
                            TrimWhitespace(b);
                            n.push_back(b);
                        }
                    }
                    l._setReturn(n);
                }
                else if (buffer == "mime")
                {
                    getline(t, buffer, ' ');
                    getline(t, buffer, ' ');
                    std::string m;
                    while(getline(t, buffer, ' '))
                    {
                        comma = buffer.find('}');
                        if (comma != std::string::npos)
                            break;
                        TrimWhitespace(buffer);
                        if (buffer.find(';') != std::string::npos)
                            m+= buffer + '\n';
                        else
                            m += buffer + ' ';
                    }
                    m.erase(m.size() - 1);
                    l._setmimeDefault(Finder("default ", m));
                    l._setmimeTypesString(Finder("types ", m));
                }
            }
            L.push_back(l);
        }
    }
    if (L.empty())
        L.push_back(tmp);
    return (L);
}

bool FindAutoIndex(const std::string KeyWord, std::string ConfigContent)
{
    std::istringstream Texte(ConfigContent);
    std::string line;

    while (getline(Texte, line))
    {
        size_t i = line.find(KeyWord);
        if (i == 0)
        {
            line = line.substr(KeyWord.size());
            line.erase(line.size() - 1);
            TrimWhitespace(line);
            if (line == "on")
                return (ON);
            else if (line == "off")
                return (OFF);
            std::cerr << "[" << KeyWord << "] Invalid Option '" << line << "'. Accepte Only [on] or [off]" << std::endl;
            exit(1);
        }
    }
    return (OFF);
}

std::string FindMimeDefault(std::string KeyWord, std::string ConfigContent)
{
    std::string s = Finder(KeyWord, ConfigContent);
    if (s.empty())
        return ("application/octet-stream");
    std::istringstream t(s);
    std::string v;
    while(getline(t, v, ';'))
    {
        if (v.find("default", 0) != std::string::npos)
        {
            v = v.substr(8, v.size());
            TrimWhitespace(v);
            if (v.find(',') != std::string::npos)
            {
                std::cerr << "default Accepte Only one PathValue no More!!" << std::endl;
                exit(1); 
            }
            return (v);
        }
    }
    return ("text/plain");
}

size_t SpaceCalculation(std::string s)
{
    size_t i = 0;
    size_t j = 0;
    size_t count = 0;

    if (s.empty())
        return (0);
    if (s[0] == ':' || s[s.size() - 1] == ':')
    {
        std::cerr << "[BAD USAGE OF SEPERATOR] MIME TYPE SHOULD NOT START OR END WITH ':' SEPERATOR" << std::endl;
        exit(1);
    }
    while(i < s.size())
    {
        if (s[i] == 32)
        {
            count++;
            j = i + 1;
        }
        i++;
    }
    if ((count == 1 && s[j] == ':'))
    {
        std::cerr << "[MIME TYPE] Syntax Error. {" << s << "} MIME TYPE SHOULD BE IN THIS FORMAT [TYPE EXT1:EXT2]" << std::endl;
        exit(1);
    }
    return (count);
}

void    Mimetypedup(std::string s, int status, std::vector<std::string> &types)
{
    static std::vector<std::string> b;
    if (status == OFF)
    {
        size_t i = 0;
        size_t j;
        while(i < b.size())
        {
            j = i + 1;
            while(j < b.size())
            {
                if (b[i] == b[j])
                {
                    std::cerr << "[MIME TYPE] Error When Using Extensions. One or Multiple Extensions Defined in Defferents Path or the same Path" << std::endl;
                    b.clear();
                    types.clear();
                    exit(1);
                }
                j++;
            }
            i++;
        }
        b.clear();
    }
    std::istringstream t(s);
    std::string ll;
    getline(t, ll, ' ');
    getline(t, ll, ' ');
    std::istringstream n(ll);
    while(getline(n, ll, ':'))
        b.push_back(ll);
}

std::vector<std::string> FillingDefaultMimeTypes()
{
    std::vector<std::string> v;
    v.push_back("audio/aac .aac");
    v.push_back("application/x-abiword .abw");
    v.push_back("image/apng .apng");
    v.push_back("application/x-freearc .arc");
    v.push_back("image/avif .avif");
    v.push_back("video/x-msvideo .avi");
    v.push_back("application/vnd.amazon.ebook .azw");
    v.push_back("application/octet-stream .bin");
    v.push_back("image/bmp .bmp");
    v.push_back("application/x-bzip .bz");
    v.push_back("application/x-bzip2 .bz2");
    v.push_back("application/x-cdf .cda");
    v.push_back("application/x-csh .csh");
    v.push_back("text/css .css");
    v.push_back("text/csv .csv");
    v.push_back("application/msword .doc");
    v.push_back("application/vnd.openxmlformats-officedocument.wordprocessingml.document .docx");
    v.push_back("application/vnd.ms-fontobject .eot");
    v.push_back("application/epub+zip .epub");
    v.push_back("application/gzip .gz");
    v.push_back("image/gif .gif");
    v.push_back("text/html .htm:.html");
    v.push_back("image/vnd.microsoft.icon .ico");
    v.push_back("text/calendar .ics");
    v.push_back("application/java-archive .jar");
    v.push_back("image/jpeg .jpeg:.jpg");
    v.push_back("text/javascript .js");
    v.push_back("application/json .json");
    v.push_back("application/ld+json .jsonld");
    v.push_back("audio/midi .mid:.midi");
    v.push_back("text/javascript .mjs");
    v.push_back("audio/mpeg .mp3");
    v.push_back("video/mpeg .mpeg");
    v.push_back("video/mp4 .mp4");
    v.push_back("application/vnd.apple.installer+xml .mpkg");
    v.push_back("application/vnd.oasis.opendocument.presentation .odp");
    v.push_back("application/vnd.oasis.opendocument.spreadsheet .ods");
    v.push_back("application/vnd.oasis.opendocument.text .odt");
    v.push_back("audio/ogg .oga:.ogv");
    v.push_back("application/ogg .ogx");
    v.push_back("audio/opus .opus");
    v.push_back("font/otf .otf");
    v.push_back("image/png .png");
    v.push_back("application/pdf .pdf");
    v.push_back("application/x-httpd-php .php");
    v.push_back("application/vnd.ms-powerpoint .ppt");
    v.push_back("application/vnd.openxmlformats-officedocument.presentationml.presentation .pptx");
    v.push_back("application/vnd.rar .rar");
    v.push_back("application/rtf .rtf");
    v.push_back("application/x-sh .sh");
    v.push_back("image/svg+xml .svg");
    v.push_back("application/x-tar .tar");
    v.push_back("image/tiff .tif:.tiff");
    v.push_back("video/mp2t .ts");
    v.push_back("font/ttf .ttf");
    v.push_back("text/plain .txt");
    v.push_back("application/vnd.visio .vsd");
    v.push_back("audio/wav .wav");
    v.push_back("audio/webm .weba:.webm");
    v.push_back("image/webp .webp");
    v.push_back("font/woff .woff");
    v.push_back("font/woff2 .woff2");
    v.push_back("application/xhtml+xml .xhtml");
    v.push_back("application/vnd.ms-excel .xls");
    v.push_back("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet .xlsx");
    v.push_back("application/xml .xml");
    v.push_back("application/vnd.mozilla.xul+xml .xul");
    v.push_back("application/zip .zip");
    v.push_back("video/3gpp .3gp");
    v.push_back("video/3gpp2 .3g2");
    v.push_back("application/x-7z-compressed .7z");
    return (v);
}

std::vector<std::string> FillingMimeTypes(std::vector<std::string> s)
{
    std::vector<std::string> v;
    v = FillingDefaultMimeTypes();
    if (s.empty())
        return (v);
    for (std::vector<std::string>::iterator it = s.begin(); it != s.end(); ++it)
    {
        for(std::vector<std::string>::iterator it2 = v.begin(); it2 != v.end(); ++it2)
        {
            size_t lukas = (*it2).find(32);
            size_t saidos = (*it).find(32);
            if ((*it).substr(0, saidos) == (*it2).substr(0, lukas) || (*it).substr(saidos + 1) == (*it2).substr(lukas + 1) || *it == *it2)
            {
                v.erase(it2);
                it2 = v.begin();
            }
            if (lukas < (*it2).size())
            {
                size_t abdo = (*it2).substr(lukas + 1).find(":");
                if (abdo != std::string::npos)
                {
                    if ((*it2).substr(abdo + 1) == (*it).substr(saidos + 1) || (*it2).substr(lukas + 1, abdo - 1) == (*it2).substr(abdo + 1))
                    {
                        v.erase(it2);
                        it2 = v.begin();
                    }
                }
            }
        }
    }
    if (v.empty())
        return (s);
    for (std::vector<std::string>::iterator it = s.begin(); it != s.end(); ++it)
        v.push_back(*it);
    return (v);
}

std::vector<std::string> FindMimeTypes(std::string KeyWord, std::string ConfigContent)
{
    std::vector<std::string> types;
    std::string s = Finder(KeyWord, ConfigContent);
    if (s.empty())
        return (FillingDefaultMimeTypes());
    std::istringstream t(s);
    std::string v;
    size_t i = 0;
    std::string replace;
    size_t index_s = 0;
    while(getline(t, v, ';'))
    {
        if (v.find("types", 0) != std::string::npos)
            break;
    }
    if (v.find("types", 0) != std::string::npos)
    {
        v = v.substr(6, v.size());
        TrimWhitespace(v);
        i = 0;
        while ((i = v.find(',', index_s)) != std::string::npos)
        {
            replace = "\n";
            index_s = i + 1;
            v = RWord(v, replace, i, 1);
            TrimWhitespace(v);
        }
        std::istringstream l(v);
        while(getline(l, v))
        {
            if (SpaceCalculation(v) != 1)
            {
                std::cerr << "[SERVER MIME TYPE] Syntax Error. {" << v << "} MIME TYPE SHOULD BE IN THIS FORMAT [TYPE EXT1:EXT2]" << std::endl;
                exit(1);
            }
            Mimetypedup(v, ON, types);
            types.push_back(v);
        }
        Mimetypedup("", OFF, types);
    }
    return (FillingMimeTypes(types));
}

server webserv::FillingServer( const  std::string ConfigContent )
{
    server s;

    s.setServers_names(Finder_vector("server_names ", ConfigContent));
    s.setMethodesAllowed(Finder_vector("allow ", ConfigContent));
    s.setError_pages(Finder_vector("error_pages ", ConfigContent));
    s.setMax_body_size(Finder("max_body_size ", ConfigContent));
    s.setIndex(Finder_vector("index ", ConfigContent));
    s.setHost(Finder("host ", ConfigContent));
    s.setcginfo(Finder("cgi_info ", ConfigContent));
    s.setPort(Finder_vector("listen ", ConfigContent));
    s.setRoot(Finder("root ", ConfigContent));
    s.setUploadPath(Finder("path_upload ", ConfigContent));
    s.setAutoindex(FindAutoIndex("autoindex ", ConfigContent));
    s.setupload(FindAutoIndex("upload ", ConfigContent));
    s.setLocations(FindLocation("location ", ConfigContent, s));
    s.setmimeDefault(FindMimeDefault("mime type {", ConfigContent));
    s.setmimeTypes(FindMimeTypes("mime type {" , ConfigContent));
    return (s);
}

std::vector<std::string> webserv::getContent() const
{
    return (ConfigContent);
}

std::vector<server> webserv::getServers() const
{
    return (servers);
}

webserv::~webserv()
{
}