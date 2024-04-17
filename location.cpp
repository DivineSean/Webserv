#include "location.hpp"

location::location() {}

location::location(const server &copy)
{
    std::vector <std::string> s;
    this->_setMethodesAllowed(copy.getMethodesAllowed());
    this->_setPathLocation("/");
    this->_setError_pages(copy.getError_pages());
    this->_setMax_body_size(copy.getMax_body_size());
    this->_setIndex(copy.getIndex());
    this->_setRoot(copy.getRoot());
    this->_setAutoindex(copy.getAutoindex());
    this->_setupload(copy.getupload());;
    this->_setUploadPath(copy.getUploadPath());
    this->_setcginfo(copy.getcginfo());
    this->_setcginfoPHP(copy.getcginfoPHP());
    this->_setmimeDefault(copy.getmimeDefaul());
    this->_setmimeTypes(copy.getmimeTypes());
    this->_Return.clear();
}

std::vector<std::string>    location::_getcginfoPHP() const
{
    return (this->_cgi_info_PHP);
}

void                        location::_setcginfoPHP(const std::vector<std::string> cgi_info_PHP)
{
    this->_cgi_info_PHP = cgi_info_PHP;
}

std::vector<std::string>    location::_getcginfo() const
{
    return (this->_cgi_info);
}

void                        location::_setcginfo(const std::vector<std::string> cgi_info)
{
    this->_cgi_info = cgi_info;
}

void                        location::_setclear()
{
    this->_cgi_info.clear();
    this->_cgi_info_PHP.clear();
}

void                        location::_setcginfoS(const std::string s)
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
            _setcginfoS(cgi_info);
            b = b.substr(index + 1, b.size());
            TrimWhitespace(b);
            _setcginfoS(b);
            // std::cout << b << std::endl;exit(1);
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
        this->_cgi_info= v;
    else if (v[0] == "*.php")
        this->_cgi_info_PHP = v;
}

void  location::_setUploadPath(const std::string UploadPath)
{
    if (UploadPath.find(",") != std::string::npos)
    {
        std::cerr << "[UploadPath] Accepte Only One Value" << std::endl;
        exit(1);
    }
    this->_uploadPath = UploadPath;
}

std::string  location::_getUploadPath() const
{
    return (this->_uploadPath);
}

location &location::operator=(location const &copy)
{
    this->_setMethodesAllowed(copy._getMethodesAllowed());
    this->_setPathLocation(copy._getPathLocation());
    this->_setError_pages(copy._getError_pages());
    this->_setMax_body_size(copy._getMax_body_size());
    this->_setIndex(copy._getIndex());
    this->_setRoot(copy._getRoot());
    this->_setAutoindex(copy._getAutoindex());
    this->_setupload(copy._getupload());;
    this->_setUploadPath(copy._getUploadPath());
    this->_Return = copy._getReturn();
    this->_setcginfo(copy._getcginfo());
    this->_setcginfoPHP(copy._getcginfoPHP());
    this->_setmimeDefault(copy._getmimeDefaul());
    this->_setmimeTypes(copy._getmimeTypes());
    this->_Return.clear();
	return (*this);
}

void                        location::_setupload(const bool upload)
{
    this->_upload = upload;
}

std::vector<std::string>    location::_getMethodesAllowed() const
{
    return (_MethodesAllowed);
}

std::vector<std::string>    location::_getError_pages() const
{
    return (_error_pages);
}

long long                   location::_getMax_body_size() const
{
    return (_max_body_size);
}

std::vector<std::string>    location::_getIndex() const
{
    return (_index);
}

std::string                 location::_getRoot() const
{
    return (_root);
}

std::vector<std::string>    location::_getReturn() const
{
    return (_Return);
}

std::string                 location::_getPathLocation() const
{
    return (_PathLocation);
}

bool                        location::_getAutoindex() const
{
    return (_autoindex);
}

bool                        location::_getupload() const
{
    return (_upload);
}

void                        location::_setMethodesAllowed(const std::vector<std::string> MethodesAllowed)
{
    for (std::vector<std::string>::const_iterator it = MethodesAllowed.begin(); it != MethodesAllowed.end(); ++it)
    {
        if (*it != "GET" && *it != "POST" && *it != "DELETE")
        {
            std::cerr << "[ALLOW METHODS] '" << *it << "' is Invalid Method !!" << std::endl;
            exit(1);
        }
    }
    this->_MethodesAllowed = MethodesAllowed;
}

void                        location::_setError_pages(const std::vector<std::string> error_pages)
{
    std::string data;
    std::string s;
    for (std::vector<std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
    {
        s = Code_PathReturn(*it);
        if (s.empty())
        {
            std::cerr << "Location Error Page in " << *it << " [PATH] not Defined" << std::endl;
            exit(1);    
        }
        if (s[s.size() - 1] == ';')
            s.erase(s.size() - 1);
        if (!fileExists(s.c_str())) 
        {
            std::cerr << "Location Error Page in " << s << " [File] Not Exist" << std::endl;
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
            std::cerr << "Location Same Error code in Error Page [" << s2 << "] Defined" << std::endl;
            exit(1);    
        }
        data += s2 + " ";
    }
    data.clear();
    this->_error_pages = error_pages;
}

void                        location::_setMax_body_size(const long long max_body_size)
{
    this->_max_body_size = max_body_size;
}

void                        location::_setMax_body_string(const std::string m)
{
    size_t i  = 0;
    size_t converter = 1;
    long long num = 0;
    const char *s = m.c_str();
    if (m[m.size() - 1] != 'M' && m[m.size() - 1] != 'G' && m[m.size() - 1] != 'K' && m[m.size() - 1] != 'B')
    {
        std::cerr << "[Location " << this->_PathLocation << " Max Body Size] Should be Degit With [G='Go'] [M ='Mo'] [K='Ko'] [B ='Byte']" << std::endl;
        exit(1);
    }
    while(i < m.size() - 1)
    {
        if (!isdegit(s[i]))
        {
            std::cerr << "[Location " << this->_PathLocation << " Max Body Size] Should be Degit With One char Of This [G='Go'] [M ='Mo'] [K='Ko'] [B ='Byte']" << std::endl;
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
    this->_max_body_size = num * converter;
}

void                        location::_setAutoindex(const bool autoindex)
{
    this->_autoindex = autoindex;
}

void                        location::_setIndex(const std::vector<std::string> index)
{
    this->_index = index;
}

void                        location::_setRoot(const std::string root)
{
    if (root.find(",") != std::string::npos)
    {
        std::cerr << "[Location root] Accepte Only One Value" << std::endl;
        exit(1);
    }
    this->_root = root;
}

void                        location::_setPathLocation(const std::string PathLocation)
{
    this->_PathLocation = PathLocation;
}

std::string Code_PathReturn_(std::string s)
{
    int i = 0;
    int flag = 0;
    size_t num = 0;
    if (s != "" && !std::isdigit(s[0]))
    {
        s = "302=" + s;
        flag = 1;
    }
    const char *str = s.c_str();
    while(str[i])
    {
        if (!isdigit(str[i]) || (str[i] == 48 && s.size() == 1))
        {
            if (num < 1 || num > 999)
            {
                std::cerr << "[Location Return] StatsCode " << num << " is not in Range [1-999]" << std::endl;
                exit(1);
            }
            if (s[i] != '=')
            {
                std::cerr << "[Location Return] You Forgot '=' in >> " << s << std::endl;
                exit(1);
            }
            if (s.substr(i + 1).empty())
            {
                std::cerr << "Return in " << s << " [PATH] not Defined" << std::endl;
                exit(1);    
            }
            return (s);
        }
        num = (num * 10) + (str[i] - 48);
        i++;
    }
    return (s);
}

void                        location::_setReturn(const std::vector<std::string> Return)
{   
    size_t i = 0;
    std::string data;
    std::string s;
    while(i < Return.size())
    {
        s = Code_PathReturn_(Return[i]);
        if (s.find("=") == std::string::npos && (s.size() < 1 || s.size() > 3))
        {
                std::cerr << "[Location Return] StatsCode " << s << " is not in Range [1-999]" << std::endl;
                exit(1);
        }
        this->_Return.push_back(s);
        i++;
    }
}

std::string                 location::_getmimeDefaul() const
{
    return (this->_mime_default);
}

std::vector<std::string>    location::_getmimeTypes() const
{
    return (this->_mime_types);
}

void                        location::_setmimeDefault(const std::string a)
{
    if (a.empty() && _mime_default.empty())
    {
        this->_mime_default = "texte/plain";
        return ;
    }
    if (a.find(',') != std::string::npos)
    {
        std::cerr << "location mime type default Accepte Only one PathValue no More!!" << std::endl;
        exit(1); 
    }
    this->_mime_default = a;
}

void    location::_Mimetypedup(std::string s, int status, std::vector<std::string> &types)
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

void                        location::_setmimeTypesString(const std::string a)
{
    std::string v = a;
    std::vector<std::string> types;
    size_t i;
    size_t index_s = 0;
    if (a.empty() && _mime_types.empty())
    {
        this->_mime_types = FillingDefaultMimeTypes();
        return;
    }
    while ((i = v.find(',', index_s)) != std::string::npos)
    {
        index_s = i + 1;
        v = RWord(v, "\n", i, 1);
        TrimWhitespace(v);
    }
    std::istringstream l(v);
    while(getline(l, v))
    {
        if (SpaceCalculation(v) != 1)
        {
            std::cerr << "[Location MIME TYPE] Syntax Error. {" << v << "} MIME TYPE SHOULD BE IN THIS FORMAT [TYPE EXT1:EXT2]" << std::endl;
            exit(1);
        }
        _Mimetypedup(v, ON, types);
        types.push_back(v);
    }
    _Mimetypedup("", OFF, types);
    this->_mime_types = FillingMimeTypes(types);
}

void                        location::_setmimeTypes(const std::vector<std::string> b)
{
    if (b.empty() && _mime_types.empty())
    {
        this->_mime_types = FillingDefaultMimeTypes();
        return;
    }
    this->_mime_types = b;
}

location::~location(){}