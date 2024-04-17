#pragma once

# include <iostream>
# include <cstring>
# include <vector>
# include <fstream>
# include <sstream>
# include <string>
# include <map>
# include <algorithm>
# include <cctype>
# include <filesystem>
# include <stack>
# include "server.hpp"
# include "webserv.hpp"
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <cstddef>
# include <dirent.h>
# include <sys/wait.h>
# include <csignal>

class server;
class location
{
    private:
        std::string                 _PathLocation;
        std::vector<std::string>    _MethodesAllowed;
        std::vector<std::string>    _error_pages;
        std::vector<std::string>    _Return;
        std::string                 _mime_default;
        std::vector<std::string>    _mime_types;
        long long                   _max_body_size;
        std::vector<std::string>    _index;
        std::string                 _root;
        bool                        _autoindex;
        bool                        _upload;
        std::string                 _uploadPath;
        std::vector<std::string>    _cgi_info;
        std::vector<std::string>    _cgi_info_PHP;
    public:
        location();
        location( const server &s );
        location &operator=(const location &copy);
        std::string                 _getmimeDefaul() const;
        void                        _setmimeDefault(const std::string a);
        std::vector<std::string>    _getmimeTypes() const;
        void                        _setmimeTypes(const std::vector<std::string> b);
        void                        _setmimeTypesString(const std::string b);
        std::vector<std::string>    _getMethodesAllowed() const;
        std::vector<std::string>    _getError_pages() const;
        long long                   _getMax_body_size() const;
        std::vector<std::string>    _getIndex() const;
        std::string                 _getRoot() const;
        std::vector<std::string>    _getcginfo() const;
        std::vector<std::string>    _getcginfoPHP() const;
        std::string                 _getPathLocation() const;
        std::string                 _getUploadPath() const;
        std::vector<std::string>    _getReturn() const;
        bool                        _getAutoindex() const;
        bool                        _getupload() const;
        void                        _setMethodesAllowed(const std::vector<std::string> MethodesAllowed);
        void                        _setError_pages(const std::vector<std::string> error_pages);
        void                        _setMax_body_size(const long long max_body_size);
        void                        _setMax_body_string(const std::string m);
        void                        _setAutoindex(const bool autoindex);
        void                        _setupload(const bool upload);
        void                        _setIndex(const std::vector<std::string> index);
        void                        _setRoot(const std::string root);
        void                        _setPathLocation(const std::string PathLocation);
        void                        _setReturn(const std::vector<std::string> Return);
        void                        _setUploadPath(const std::string UploadPath);
        void                        _setcginfo(const std::vector<std::string> cgi_info);
        void                        _setcginfoPHP(const std::vector<std::string> cgi_info_PHP);
        void                        _setcginfoS(const std::string cgi_info);
        void                        _setclear();
        void                        _Mimetypedup(std::string s, int status, std::vector<std::string> &types);
        ~location();
};