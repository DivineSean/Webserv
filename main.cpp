#include "webserv.hpp"

std::string RWord(std::string Buffer, std::string replace, size_t i, size_t len)
{
    std::string new_line;
    size_t index = 0;
    size_t index_replace = 0;
    while(index < i)
        new_line += Buffer[index++];
    index += len;
    while(replace[index_replace])
        new_line += replace[index_replace++];
    if (index < Buffer.size())
    {
        while(Buffer[index])
            new_line += Buffer[index++];
    }
    else
    {
        new_line += "\0";
    }
    return (new_line);
}

std::string Replace_Word(std::string Buffer, size_t i)
{
    std::string new_line;
    size_t index = 0;

    while(index < i)
        new_line += Buffer[index++];
    new_line += "\0";
    return (new_line);
}

std::string letsToken(int ac, char **av)
{
    std::fstream FileName;
    std::string Buffer;
    std::string Data;
    std::string replace;
    std::string word;
    size_t  i;
    size_t index;
    size_t index_;
    size_t index_s;

    if (ac == 1)
        return (Data);
    FileName.open(av[1], std::ios::in);
    if (!FileName)
    {
        std::cout << "Error while reading from the file" << std::endl;
        exit(1);
    }
    while(getline(FileName, Buffer))
    {
        TrimWhitespace(Buffer);
        index_ = 0;
        while ((i = Buffer.find("#", index_)) < Buffer.size())
        {
            index_ = i;
            Buffer = Replace_Word(Buffer, i);
        }
        word = ";";
        index_s = 0;
        while ((i = Buffer.find(word, index_s)) < Buffer.size())
        {
            replace = "; ";
            index_s = i + replace.size();
            Buffer = RWord(Buffer, replace, i, word.size());
        }
        word = "{";
        index = 0;
        while ((i = Buffer.find(word, index)) < Buffer.size())
        {
            replace = " { ";
            index = i + replace.size();
            Buffer = RWord(Buffer, replace, i, word.size());
        }
        word = ", ";
        size_t ind = 0;
        while ((i = Buffer.find(word, ind)) < Buffer.size())
        {
            replace = ",";
            ind = i + replace.size();
            Buffer = RWord(Buffer, replace, i, word.size());
        }
        word = " ,";
        size_t in = 0;
        while ((i = Buffer.find(word, in)) < Buffer.size())
        {
            replace = ",";
            in = i + replace.size();
            Buffer = RWord(Buffer, replace, i, word.size());
        }
        word = " , ";
        size_t ins = 0;
        while ((i = Buffer.find(word, ins)) < Buffer.size())
        {
            replace = ",";
            ins = i + replace.size();
            Buffer = RWord(Buffer, replace, i, word.size());
        }
        word = " ;";
        size_t i_s = 0;
        while ((i = Buffer.find(word, i_s)) < Buffer.size())
        {
            replace = ";";
            i_s = i + replace.size();
            Buffer = RWord(Buffer, replace, i, word.size());
        }
        word = ";;";
        size_t i_k = 0;
        while ((i = Buffer.find(word, i_k)) < Buffer.size())
        {
            replace = ";";
            i_s = i + replace.size();
            Buffer = RWord(Buffer, replace, i, word.size());
        }
        word = ",,";
        while ((i = Buffer.find(word)) != std::string::npos)
        {
            replace = ",";
            Buffer = RWord(Buffer, replace, i, word.size());
        }
        Data += Buffer + " ";
    }
    TrimWhitespace(Data);
    FileName.close();
    return (Data);
}

void    locationReapetCheker(std::string prev, std::string Buffer, int status)
{
    static int locationreturn = OFF;
    static int locationroot = OFF;
    static int locationindex = OFF;
    static int locationallow_methods = OFF;
    static int locationmaxsize = OFF;
    static int locationautoindex = OFF;
    static int locationCgiInfo = OFF;
    static int locationUpload = OFF;
    static int uploadpath = OFF;
    static int locationerror_pages = OFF;

    if (status == OFF)
    {
        locationreturn = OFF;
        locationroot = OFF;
        locationindex = OFF;
        locationallow_methods = OFF;
        locationmaxsize = OFF;
        locationautoindex = OFF;
        locationCgiInfo = OFF;
        locationUpload = OFF;
        uploadpath = OFF;
        locationerror_pages = OFF;
        return ;
    }
    if (prev == "return" && locationreturn == OFF)
        locationreturn = ON;
    else if (prev == "error_pages" && locationerror_pages == OFF)
        locationerror_pages = ON;
    else if (prev == "root" && locationroot == OFF)
        locationroot = ON;
    else if (prev == "path_upload" && uploadpath == OFF)
        uploadpath = ON;
    else if (prev == "index" && locationindex == OFF)
        locationindex = ON;
    else if (prev == "allow" && locationallow_methods == OFF)
        locationallow_methods = ON;
    else if (prev == "max_body_size" && locationmaxsize == OFF)
        locationmaxsize = ON;
    else if (prev == "autoindex" && locationautoindex == OFF)
        locationautoindex = ON;
    else if (prev == "cgi_info" && locationCgiInfo == OFF)
        locationCgiInfo = ON;
    else if (prev == "upload" && locationUpload == OFF)
        locationUpload = ON;
    else
    {
        if (prev == "index")
        {
            std::cerr << "Please Put Index [" << Buffer << "] in First Index Declared With ',' Between Them !!" << std::endl;
            exit(1);
        }
        else if (prev == "return")
        {
            std::cerr << "Please Put Return [" << Buffer << "] in First Return Declared With ',' Between Them !!" << std::endl;
            exit(1);  
        }
        else if (prev == "error_pages")
        {
            std::cerr << "Please Put Return [" << Buffer << "] in First Return Declared With ',' Between Them !!" << std::endl;
            exit(1);  
        }
        else if (prev == "path_upload")
        {
            std::cerr << "Please Put path_upload [" << Buffer << "] in First path_upload Declared With ',' Between Them !!" << std::endl;
            exit(1);  
        }
        std::cerr << "webserv: [DIRECTIVE " << prev << "] SHOULD NOT BE REPEATED" << std::endl;
        exit(1);
    }
}

void    serverReapetChecker(std::string Buffer, int status)
{
    static int listen = OFF;
    static int Host = OFF;
    static int ServerName = OFF;
    static int ErrorPage = OFF;
    static int root = OFF;
    static int index = OFF;
    static int allow_methods = OFF;
    static int maxsize = OFF;
    static int autoindex = OFF;
    static int CgiInfo = OFF;
    static int Upload = OFF;
    static int UploadPath = OFF;
    if (status == OFF)
    {
        listen = OFF;
        Host = OFF;
        ServerName = OFF;
        ErrorPage = OFF;
        root = OFF;
        index = OFF;
        allow_methods = OFF;
        maxsize = OFF;
        autoindex = OFF;
        CgiInfo = OFF;
        Upload = OFF;
        UploadPath = OFF;
        return ;
    }
    if (Buffer == "listen" && listen == OFF)
        listen = ON;
    else if (Buffer == "host" && Host == OFF)
        Host = ON;
    else if (Buffer == "path_upload" && UploadPath == OFF)
        UploadPath = ON;
    else if (Buffer == "server_names" && ServerName == OFF)
        ServerName = ON;
    else if (Buffer == "error_pages" && ErrorPage == OFF)
        ErrorPage = ON;
    else if (Buffer == "root" && root == OFF)
        root = ON;
    else if (Buffer == "index" && index == OFF)
        index = ON;
    else if (Buffer == "allow" && allow_methods == OFF)
        allow_methods = ON;
    else if (Buffer == "max_body_size" && maxsize == OFF)
        maxsize = ON;
    else if (Buffer == "autoindex" && autoindex == OFF)
        autoindex = ON;
    else if (Buffer == "cgi_info" && CgiInfo == OFF)
        CgiInfo = ON;
    else if (Buffer == "upload" && Upload == OFF)
        Upload = ON;
    else
    {
        if (Buffer == "index")
        {
            std::cerr << "Please Put [" << Buffer << "] in First Index Declared With ',' Between Them !!" << std::endl;
            exit(1);
        }
        else if (Buffer == "listen")
        {
            std::cerr << "Please Put [" << Buffer << "] in First listen Declared With ',' Between Them !!" << std::endl;
            exit(1);  
        }
        else if (Buffer == "server_names")
        {
            std::cerr << "Please Put [" << Buffer << "] in First server_names Declared With ',' Between Them !!" << std::endl;
            exit(1);  
        }
        else if (Buffer == "error_pages")
        {
            std::cerr << "Please Put [" << Buffer << "] in First error_pages Declared With ',' Between Them !!" << std::endl;
            exit(1);  
        }
        else if (Buffer == "cgi_info")
        {
            std::cerr << "Please Put [" << Buffer << "] in First cgi_info Declared With ',' Between Them !!" << std::endl;
            exit(1);
        }
        else if (Buffer == "path_upload")
        {
            std::cerr << "Please Put [" << Buffer << "] in First path_upload Declared With ',' Between Them !!" << std::endl;
            exit(1);
        }
        std::cerr << "webserv: [DIRECTIVE " << Buffer << "] SHOULD NOT BE REPEATED" << std::endl;
        exit(1);
    }
}

void    mimetypeReapetCheker(std::string prev, std::string Buffer, int status)
{
    static int defaulttype = OFF;
    static int types = OFF;

    if (status == OFF)
    {
        defaulttype = OFF;
        types = OFF;
        return ;
    }
    if (prev == "default" && defaulttype == OFF)
        defaulttype = ON;
    else if (prev == "types" && types == OFF)
        types = ON;
    else
    {
        if (Buffer == "types")
        {
            std::cerr << "Please Put [" << Buffer << "] in First types directive Declared With ',' Between Them for multiple!!" << std::endl;
            exit(1);  
        }
        std::cerr << "webserv: [DIRECTIVE " << Buffer << "] SHOULD NOT BE REPEATED" << std::endl;
        exit(1);
    }
}

std::vector<std::string> File_Data(std::string Content)
{
    int serverLock = OFF;
    int locationLock = OFF;
    int mimetypeLock = OFF;
    int mimelocationLock;
    int mimeserverLock = OFF;
    std::string Buffer;
    std::string Data;
    std::vector<std::string> g;
    std::string prev;

    if (Content.empty())
        return (g);
    TrimWhitespace(Content);
    std::istringstream Texte(Content);
    while(getline(Texte, Buffer, ' '))
    {
        if (Buffer == "server")
        {
            if (!getline(Texte, Buffer, ' '))
            {
                std::cout << "webserv: server Syntax Error" << std::endl;
                exit(1);
            }
            if (Buffer != "{")
            {
                std::cout << "webserv: server Syntax Error" << std::endl;
                exit(1);
            }
            serverLock = ON;
            Data = "server\n{\n";
            mimeserverLock = OFF;
            while(serverLock == ON)
            {
                if (!getline(Texte, Buffer, ' '))
                {
                    std::cout << "webserv: server Syntax Error" << std::endl;
                    exit(1);
                }
                if (Buffer == "location")
                {
                    if (!getline(Texte, Buffer, ' '))
                    {
                        std::cout << "webserv: location Syntax Error" << std::endl;
                        exit(1);
                    }
                    if (Buffer == "{")
                    {
                        std::cout << "webserv: location Syntax Error" << std::endl;
                        exit(1);
                    }
                    Data += "location " + Buffer + ' ';
                    if (!getline(Texte, Buffer, ' '))
                    {
                        std::cout << "webserv: location Syntax Error" << std::endl;
                        exit(1);
                    }
                    if (Buffer != "{")
                    {
                        std::cout << Buffer << std::endl;
                        std::cout << "websserv: location Syntax Error" << std::endl;
                        exit(1);
                    }
                    else
                    {
                        Data += Buffer + ' ';
                        mimelocationLock = OFF;
                        locationLock = ON;
                    }
                    if (!getline(Texte, Buffer, ' '))
                    {
                        std::cout << "webserv: location Syntax Error" << std::endl;
                        exit(1);
                    }
                    prev.clear();
                    while(locationLock == ON)
                    {
                        prev = Buffer;
                        if (prev == "}")
                        {
                            Data += prev + '\n';
                            locationLock = OFF;
                            break;
                        }
                        if (!getline(Texte, Buffer, ' '))
                        {
                            std::cout << "webserv: location Syntax Error" << std::endl;
                            exit(1);
                        }
                        else if (prev == "error_pages" || prev == "path_upload"|| prev == "return" || prev == "root" || prev == "index" || prev == "allow" || prev == "max_body_size" || prev == "autoindex" || prev == "upload" || prev == "cgi_info")
                        {
                            locationReapetCheker(prev, Buffer, ON);
                            if (Buffer.find(";") == std::string::npos)
                            {
                                Data += prev + " " + Buffer + ' ';
                                while (getline(Texte, Buffer, ' '))
                                {
                                    Data += Buffer + ' ';
                                    if (Buffer.find(";") != std::string::npos)
                                    {
                                        Data.erase(Data.size() - 1);
                                        Data += ' ';
                                        break;
                                    }
                                }    
                            }
                            else
                                Data += prev + " " + Buffer + ' ';
                            if (!getline(Texte, Buffer, ' '))
                            {
                                std::cout << "webserv: location Syntax Error" << std::endl;
                                exit(1);
                            }
                        }
                        else if (prev == "mime" && Buffer == "type")
                        {
                            if (mimelocationLock == OFF)
                                mimelocationLock = ON;
                            else
                            {
                                std::cout << "webserv: location mime type Duplicated" << std::endl;
                                exit(1);
                            }
                            Data += "mime " + Buffer + ' ';
                            if (!getline(Texte, Buffer, ' '))
                            {
                                std::cout << "webserv: location mime type Syntax Error" << std::endl;
                                exit(1);
                            }
                            if (Buffer != "{")
                            {
                                std::cout << Buffer << std::endl;
                                std::cout << "websserv: location mime Syntax Error" << std::endl;
                                exit(1);
                            }
                            if (Buffer != "{")
                            {
                                std::cout << Buffer << std::endl;
                                std::cout << "websserv: location mime Syntax Error" << std::endl;
                                exit(1);
                            }
                            else
                            {
                                Data += Buffer + ' ';
                                mimetypeLock = ON;
                            }
                            if (!getline(Texte, Buffer, ' '))
                            {
                                std::cout << "webserv: location mime type Syntax Error" << std::endl;
                                exit(1);
                            }
                            prev.clear();
                            while(mimetypeLock == ON)
                            {
                                prev = Buffer;
                                if (prev == "}")
                                {
                                    Data += prev + ' ';
                                    mimetypeLock = OFF;
                                    if (!getline(Texte ,Buffer, ' '))
                                    {
                                        std::cout << "webserv: location mime type Syntax Error" << std::endl;
                                        exit(1);
                                    }
                                    break;
                                }
                                if (!getline(Texte, Buffer, ' '))
                                {
                                    std::cout << "webserv: location mime type Syntax Error" << std::endl;
                                    exit(1);
                                }
                                else if (prev == "default"|| prev == "types")
                                {
                                    mimetypeReapetCheker(prev, Buffer, ON);
                                    if (Buffer.find(";") == std::string::npos)
                                    {
                                        Data += prev + " " + Buffer + ' ';
                                        while (getline(Texte, Buffer, ' '))
                                        {
                                            Data += Buffer + ' ';
                                            if (Buffer.find(";") != std::string::npos)
                                            {
                                                Data.erase(Data.size() - 1);
                                                Data += ' ';
                                                break;
                                            }
                                        }    
                                    }
                                    else
                                        Data += prev + " " + Buffer + ' ';
                                    if (!getline(Texte, Buffer, ' '))
                                    {
                                        std::cout << "webserv: location mime type Syntax Error" << std::endl;
                                        exit(1);
                                    }
                                }
                                else
                                {
                                    if (prev != ";")
                                    {
                                        std::cout << "webserv: [MIME_TYPE_DIRECTIVE] " << prev << " NOT ALLOWED !!" << std::endl;
                                        exit(1);
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (prev != ";")
                            {
                                std::cout << "webserv: [LOCATION_DIRECTIVE] " << prev << " NOT ALLOWED !!" << std::endl;
                                exit(1);
                            }
                        }
                    }
                }
                else if (Buffer == "mime")
                {
                    if (mimeserverLock == OFF)
                        mimeserverLock = ON;
                    else
                    {
                        std::cout << "webserv: server mime type Duplicated" << std::endl;
                        exit(1);
                    }
                    if (!getline(Texte, Buffer, ' '))
                    {
                        std::cout << "webserv: mime type Syntax Error" << std::endl;
                        exit(1);
                    }
                    if (Buffer != "type")
                    {
                        std::cout << "webserv: mime type Syntax Error" << std::endl;
                        exit(1);
                    }
                    Data += "mime " + Buffer + ' ';
                    if (!getline(Texte, Buffer, ' '))
                    {
                        std::cout << "webserv: mime type Syntax Error" << std::endl;
                        exit(1);
                    }
                    if (Buffer != "{")
                    {
                        std::cout << Buffer << std::endl;
                        std::cout << "websserv: mime Syntax Error" << std::endl;
                        exit(1);
                    }
                    else
                    {
                        Data += Buffer + ' ';
                        mimetypeLock = ON;
                    }
                    if (!getline(Texte, Buffer, ' '))
                    {
                        std::cout << "webserv: mime type Syntax Error" << std::endl;
                        exit(1);
                    }
                    prev.clear();
                    while(mimetypeLock == ON)
                    {
                        prev = Buffer;
                        if (prev == "}")
                        {
                            Data += prev + '\n';
                            mimetypeLock = OFF;
                            break;
                        }
                        if (!getline(Texte, Buffer, ' '))
                        {
                            std::cout << "webserv: mime type Syntax Error" << std::endl;
                            exit(1);
                        }
                        else if (prev == "default"|| prev == "types")
                        {
                            mimetypeReapetCheker(prev, Buffer, ON);
                            if (Buffer.find(";") == std::string::npos)
                            {
                                Data += prev + " " + Buffer + ' ';
                                while (getline(Texte, Buffer, ' '))
                                {
                                    Data += Buffer + ' ';
                                    if (Buffer.find(";") != std::string::npos)
                                    {
                                        Data.erase(Data.size() - 1);
                                        Data += ' ';
                                        break;
                                    }
                                }    
                            }
                            else
                                Data += prev + " " + Buffer + ' ';
                            if (!getline(Texte, Buffer, ' '))
                            {
                                std::cout << "webserv: mime type Syntax Error" << std::endl;
                                exit(1);
                            }
                        }
                        else
                        {
                            if (prev != ";")
                            {
                                std::cout << "webserv: [MIME_TYPE_DIRECTIVE] " << prev << " NOT ALLOWED !!" << std::endl;
                                exit(1);
                            }
                        }
                    }
                }
                else if (Buffer == "}")
                {
                    Data += Buffer;
                    serverLock = OFF;
                    serverReapetChecker(Buffer, OFF);
                    break;
                }
                else if (Buffer == "path_upload"|| Buffer == "listen" || Buffer == "root" || Buffer == "index" || Buffer == "allow" || Buffer == "max_body_size" || Buffer == "autoindex" || Buffer == "upload" || Buffer == "cgi_info" || Buffer == "host" || Buffer == "server_names" || Buffer == "error_pages")
                {
                    serverReapetChecker(Buffer, ON);
                    std::string prev1 = Buffer;
                    if (!getline(Texte, Buffer, ' '))
                    {
                        std::cout << "webserv: server Syntax Error" << std::endl;
                        exit(1);
                    }
                    if (Buffer.find(";") == std::string::npos)
                    {
                        Data += prev1 + " " + Buffer + ' ';
                        while (getline(Texte, Buffer, ' '))
                        {
                            Data += Buffer + ' ';
                            if (Buffer.find(";") != std::string::npos)
                            {
                                Data.erase(Data.size() - 1);
                                Data += '\n';
                                break;
                            }
                        }    
                    }
                    else
                        Data += prev1 + " " + Buffer + '\n';
                    prev1.clear();
                }
                else
                {
                    std::cout << "webserv: [DIRECTIVE] " << Buffer << " NOT ALLOWED !!" << std::endl;
                    exit(1);
                }
                locationReapetCheker(prev, Buffer, OFF);
                mimetypeReapetCheker(prev, Buffer, OFF);
            }
            g.push_back(Data);
            Data.clear();
        }
        else
        {
            std::cout << "webserv: configFile should have server{} only !!" << std::endl;
            exit(1);
        }
    }
    return (g);
}

void    CheckFileExtension(int ac, char **av)
{
    if (ac == 1)
        return;
    if (ac > 2)
    {
        std::cerr << "webserv: Too many arguments" << std::endl;
        exit(1);
    }
    std::string file = av[1];
    size_t i = file.find_last_of(".");
    if (i == std::string::npos)
    {
        std::cerr << "webserv: configFile should have .cfg extension" << std::endl;
        exit(1);
    }
    if (file.substr(i) != ".cfg")
    {
        std::cerr << "webserv: configFile should have .cfg extension" << std::endl;
        exit(1);
    }
    return;
}

int main(int ac, char **av)
{
    CheckFileExtension(ac, av);
    std::string Content = letsToken(ac, av);
    std::vector <std::string> ArrayOfContent = File_Data(Content);
    webserv data(ArrayOfContent);
    signal(SIGPIPE, SIG_IGN);
    data.runServer();
    return (0);
}