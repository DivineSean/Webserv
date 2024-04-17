#pragma once

# include "location.hpp"
# include <fcntl.h>
# include <sys/time.h>
# define ON 1
# define OFF 0
# define NOT_DIR 0x0
class location;
class server
{
    protected:
        std::vector<std::string>            servers_names;
        std::vector<std::string>            MethodesAllowed;
        std::vector<std::string>            error_pages;
        std::vector<std::string>            index;
        std::vector<std::string>            port;
        std::vector<std::string>            cgi_info;
        std::vector<std::string>            cgi_info_PHP;
        std::string                         mime_default;
        std::vector<std::string>            mime_types;
        std::string                         uploadPath;
        std::vector<location>               locations;
        long long                           max_body_size;
        std::string                         host;
        std::string                         root;
        bool                                autoindex;
        bool                                upload;
        std::vector<int>                    fd;
    public:
        std::string code404;
        std::string code400;
        std::string code501;
        std::string code405;
        std::string code200;
        std::string code413;
        std::string code411;
        std::string code403;
        std::string code414;
        std::string code408;
        std::string code500;
        std::string code201;
        std::string request_;
        std::string cookie_html;
        std::string cookie_html2;
        std::vector<std::string> login_base;
        server();
        std::vector<int>                    getFd() const;
        void                                setFd(std::vector<int> fd);
        std::string                         getmimeDefaul() const;
        void                                setmimeDefault(const std::string a);
        std::vector<std::string>            getmimeTypes() const;
        void                                setmimeTypes(const std::vector<std::string> b);
        std::vector<std::string>            getServers_names() const;
        std::vector<std::string>            getMethodesAllowed() const;
        std::vector<std::string>            getError_pages() const;
        std::vector<std::string>            getIndex() const;
        std::vector<std::string>            getPort() const;
        std::vector<std::string>            getcginfo() const;
        std::vector<std::string>            getcginfoPHP() const;
        std::vector<location>               getLocations() const;
        long long                           getMax_body_size() const;
        std::string                         getHost() const;
        std::string                         getRoot() const;
        std::string                         getUploadPath() const;
        bool                                getAutoindex() const;
        bool                                getupload() const;
        void                                setServers_names(const std::vector<std::string> servers_names);
        void                                setMethodesAllowed(const std::vector<std::string> MethodesAllowed);
        void                                setError_pages(const std::vector<std::string> error_pages);
        void                                setLocations(const std::vector<location> locations);
        void                                setMax_body_size(const std::string max_body_size);
        void                                setAutoindex(const bool autoindex);
        void                                setupload(const bool upload);
        void                                setIndex(const std::vector<std::string> index);
        void                                setHost(const std::string host);
        void                                setPort(const std::vector<std::string> port);
        void                                setcginfo(const std::string cgi_info);
        void                                setRoot(const std::string root);
        void                                setUploadPath(const std::string uploadPath);
        ~server();
        //saad partie
        std::string                         ParseHostHeader(std::istringstream& line_per_line, std::string& buffer, std::map <std::string, std::string> &map);
        
        bool                                end_of_header(std::string& buffer);
        int                                 check_file(std::string &path, std::string keyword);
        int                                 check_cgi(std::string &current_dir, std::string Path, std::vector <std::string> py, std::vector <std::string> PHP);
        
        std::string                         check_mime_type(std::vector<std::string> mime_type, std::string extension);
        std::string                         return_extension(std::vector<std::string> mime_type, std::string path_extension);
};
bool    isdegit(char c);