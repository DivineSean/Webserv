#include "server.hpp"
#include "client.hpp"
#include <csignal>

int track_path(std::string root, std::string path)
{
    int i = 0;
    while (path[i])
    {
        if (path[i] != root[i])
            return i;
        i++;
    }
    return -1;
}

void trim_slash(std::string &target)
{
    int i = target.size() -1;
    while (i >= 0)
    {
        if (target[i] == '/' && target[i-1] == '/')
            i--;
        else
            break;
    }
    target = target.substr(0, i+1);
    
    
}

char **FillCgiEnv(std::map<std::string, std::string> reqMap)
{
    size_t len = reqMap.size() + 1;
    size_t i = 0;
    size_t j = 0;
    if (len == 0)
        return (NULL);
    for (std::map<std::string, std::string>::iterator it = reqMap.begin(); it != reqMap.end(); it++)
    {
        if (it->first.size() > 0 && it->second.size() > 0)
            j++;
    }
    char **s = new char *[j + 1];
    j = 0;
    for (std::map<std::string, std::string>::iterator it = reqMap.begin(); it != reqMap.end(); it++)
    {
        if (it->first.size() > 0 && it->second.size() > 0)
        {
            std::string key_value = "HTTP_" + it->first + '=' + it->second;
            
            i = key_value.size() + 1;
            s[j] = new char[i];
            for (i = 0; i < key_value.size(); i++)
                s[j][i] = toupper(key_value[i]);
            s[j][i] = '\0';
            j++;
        }
    }
    s[j] = NULL;
    return (s);
}

void    freeDoubleChar(char **s)
{
    size_t i = 0;
    while (s[i])
        delete[] s[i++];
    delete[] s;
}

void TimeoutHandler(int signum)
{
    (void)signum;
    kill(0, SIGKILL);
}

std::string CheckPath(std::string path)
{
    std::string s = "/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/munki:/Library/Apple/usr/bin";
    std::istringstream P(s);
    std::string b;
    while(getline(P, b, ':'))
    {
        std::string temp = b + "/" + path;
        if (access(temp.c_str(), X_OK) == 0)
            return (temp);
    }
    return (path);
}

char    **Filling_Command(std::string pyCompiler, std::string File)
{
    char **command = new char*[3];
    std::string temp = CheckPath(pyCompiler);
    command[0] = new char[temp.size() + 1];
    command[1] = new char[File.size() + 1];
    command[2] = NULL;
    strcpy(command[0], temp.c_str());
    strcpy(command[1], File.c_str());
    return (command);
}

int  cgi_execute(std::string pyCompiler, std::string File, std::map<std::string, std::string> &reqMap)
{ 
    char **env = FillCgiEnv(reqMap);
    if (!env || pyCompiler.empty() || File.empty())
        return (1);
    char **command = Filling_Command(pyCompiler, File);
    pid_t pid;
    int   fd[2];
    int status;

    if (pipe(fd) == -1)
        return 1;

    pid = fork();
    if (pid < 0)
        return 1;
    else if (pid == 0)
    {
        signal(SIGALRM, TimeoutHandler);
        alarm(2);
        close(fd[0]);
        close(STDERR_FILENO);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        if (execve(command[0], command, env) == -1)
            exit(1);
    }
    else
    {
        waitpid(-1, &status, 0);
        close(fd[1]);
        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
        {
            close(fd[0]);
            freeDoubleChar(env);
            freeDoubleChar(command);
            kill((pid + 1), SIGKILL);
            return 2;
        }
        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
        {
            close(fd[0]);
            freeDoubleChar(env);
            freeDoubleChar(command);
            return 1;
        }
        std::string FileContent;
        char buffer;
        while (read(fd[0], &buffer, 1) > 0)
            FileContent += buffer;
        close(fd[0]);
        freeDoubleChar(env);
        freeDoubleChar(command);
        if (File == "cookie.py")
            reqMap["Cookie_CGI"] = FileContent;
        else
            reqMap["CGI_Response"] = FileContent;
    }
    return 0;
}



bool check_intruder(std::string target)
{
    size_t i = 0;
    while(i < target.size())
    {
        if (target[i] != static_cast<char>(13))
        {
            if (target[i] == 'G')
                return false;
            return true;
        }
        i++;
    }
    return false;
}

int server::check_file(std::string &path, std::string keyword)
{
    
    struct stat path_info;
    std::string temp = keyword;
    
    if (stat(path.c_str(), &path_info) != 0)
    {
      
        if (path[0] == '/')
        {  
            path = '.' + path; 
            
            if (stat(path.c_str(), &path_info) != 0) 
                return 0;
        }
        else
        {
            return 0;
        }
    }
    if (S_ISDIR(path_info.st_mode)) 
    {  
        
        if (path.size() > 1 && path[path.size() -1] != '/')
            path += '/';
        if (keyword[0] == '/')
            temp = keyword.erase(0,1);   
        path =  path + temp;
        // if (path[0] == '/')
        //     path = '.' + path;
       
    }
    return 1;
}

std::string trim_char(std::string &target)
{
    size_t i = 0;
    while (i < target.size())
    {
        if (target[i] != (char) 13)
            break;
        i++;
    }
    target = target.substr(i, target.size());
    return target;
}

int server:: check_cgi(std::string &current_dir, std::string Path,  std::vector <std::string> py,  std::vector <std::string> php)
{
  
    if (Path.size() > 2)
    {
        if (Path.substr(Path.length() - 3) == ".py" && !py.empty() )
        {
            current_dir = py[2];
            if (Path.find(current_dir) != std::string::npos)
            {
                current_dir = Path;
                Path = "";
            }
            if (!check_file(current_dir, Path))
                return 2;
            // struct stat info;
            if (access(current_dir.c_str(), R_OK) != 0)
                return 2;
            return 0;
        }
        else if ( Path.size() > 3  && Path.substr(Path.size() - 4) == ".php"  && !php.empty())
        {
            current_dir = php[2];
            if (Path.find(current_dir) != std::string::npos)
            {
                current_dir = Path;
                Path = "";
            }
        
            if (!check_file(current_dir, Path))
                return 2;
            
            
            // struct stat info;
            if (access(current_dir.c_str(), R_OK) != 0)
                return 2;
            return 1;
        }
    }  
    return 2;
}

std::string server:: return_extension(std::vector<std::string> mime_type, std::string path_extension)
{
    size_t  i = 0;
    while (i < mime_type.size())
    {
        std::istringstream f(mime_type[i]);
        std::string s;
        while (getline(f, s, ' '))
        {
            std::string prev = s;
            getline(f, s, ' ');
            if (prev == path_extension)
            {
                if (s.find(":") == std::string::npos)
                    return s;
                std::istringstream v(s);
                std::string l;
                getline(v, l, ':');
                return (l);
            }
        }
        i++;
    }
    std::string none = "";
    return none;
}

std::string server:: check_mime_type(std::vector<std::string> mime_type, std::string extension)
{
    size_t  i = 0;
    while (i < mime_type.size())
    {
        std::istringstream f(mime_type[i]);
        std::string s;
        while (getline(f, s, ' '))
        {
            std::string prev = s;
            getline(f, s, ' ');
            if (s == extension)
                return prev;
            std::istringstream v(s);
            std::string l;
            
            while(getline(v, l, ':'))
            {
                if (l == extension)
                    return prev;
            }
        }
        i++;
    }
    std::string none = "";
    return none;
}

std::string get_Extension(std::string s)
{
    size_t i = s.find_last_of('.');
    if (i != std::string::npos)
        return s.substr(i, s.size());
    return "";
}
void client:: openFileSuccess(std::string Path)
{  
     
    
    if (Path.empty())
    {
        map["Status_Code"] = server->code400;
        return ;
    }
    std::size_t f =  Path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._:~?/#[]@!$&'()*+,;=%");
    if (f != std::string::npos) 
    {
        map["Status_Code"] = server->code400;
        return ;
    }
    if (Path.size() > 2048)
    {
        map["Status_Code"] = server->code414;
        return ;
    }
 
    if (Path.size() >= 3 && (Path.find("/..") == Path.size() - 3 || Path.find("/../") != std::string::npos))
    {
        
        map["Status_Code"] = server->code403;
        return ;
    }
    trim_slash(Path);
    size_t i = 0;
    std::string current_dir;
    std::ifstream file_object;
    std::vector<std::string> homepage;
    std::string HOMEHOMEPAGE;
    std::string buffer;
    std::string extension;
    bool flag_auto_index = server->getAutoindex();
    std::vector <location> a = server->getLocations();
    std::string root ;
    size_t counter = 0;
    int flag = 0;
    size_t t = 0;
    std::string Path_extension;
    for (std::vector<location>::const_iterator it = a.begin(); it != a.end(); ++it)
    {
        
        if (Path == "get_Cookie.html")
        {   
            map["Cookie_Body"] = server->cookie_html + "\"" + "/" + "\"" + server->cookie_html2; 
            map["Mime_type"] = "text/html";
            map["check_Cookie"] = "none" ;
            map["Status_Code"] = server->code200;
            return ;  
        }
        if (Path == "/")
        {
            if (access(".", F_OK) != 0)
            {
                map["Status_Code"] = server->code403;
                return ;
            }
            if (access("cookie.py", X_OK | F_OK) != 0)
            {
            
                map["Status_Code"] = server->code403;
                return ;
            }
            if (access("/usr/bin/python3", X_OK ) != 0)
            {
                std::cout << "bin" << std::endl;
                map["Status_Code"] = server->code403;
                return ;
            }
            int cgi_status = cgi_execute("/usr/bin/python3", "cookie.py", map); 
            if (cgi_status == 1)
            {
                map["Status_Code"] = server->code500;
                return ;
            }
            if (cgi_status == 2)
            {
                map["Status_Code"] = server->code408;
                return ;
            } 
            map["check_Cookie"] = "in";
            std::string cookie  = "UID=" + map["Cookie_CGI"] + "&500" + "\n"; 
            server->login_base.push_back(cookie);
        }
        Path_extension = get_Extension(Path);
        
        t = Path.find(it->_getPathLocation());
        if (Path == it->_getPathLocation() || t == 0 || (!Path_extension.empty() && it->_getPathLocation() == "*" + Path_extension))
        {
            // if (t == 0)
            // {
            //     Path.erase(0, t);
            // }
            while (counter < it->_getMethodesAllowed().size())
            {
                if ("GET" == it->_getMethodesAllowed()[counter])
                {
                    flag = 1;
                    break;
                }
                counter++;
            }
            if (flag == 0)
            {
                map["Status_Code"] = server->code405;
                return ;
            } 
            if (!it->_getReturn().empty())
            {
                std::vector <std::string> v = it->_getReturn();
                
                i = 0;
                std::string status_code;
                i = v[0].find("=");
                if (i != std::string::npos)
                {
                    map["Status_Code"] = v[0].substr(0, i);               
                    map["location"] = v[0].substr(i+1, v[0].size());
                
                    // if (map["location"].find("https://", 0) != std::string::npos) //link case
                    if (!map["location"].empty())
                    {
                        std::string href = "<html> <body> <a href = "+ map["location"] + ">Click Here</a></body> </html>";
                        
                        map["location_href"] = href;
                        
                        return ;
                    }
                    map["Status_Code"] = server->code404;
                    return ;
                }
            }
            if (!it->_getcginfo().empty() || !it->_getcginfoPHP().empty())
            {

                int cgi_checker = server->check_cgi(current_dir, Path, it->_getcginfo(), it->_getcginfoPHP());
                int cgi_status = 0;
                if (cgi_checker != 2)
                {
                    if (cgi_checker == 0 && !it->_getcginfo().empty())
                    {
                        cgi_status = cgi_execute(it->_getcginfo()[1], current_dir, map);
                    }
                    else if (cgi_checker == 1 && !it->_getcginfoPHP().empty())
                    {
                        cgi_status = cgi_execute(it->_getcginfoPHP()[1], current_dir, map);
                    }
                
                    if (cgi_status == 1)
                    {
                        map["Status_Code"] = server->code500;
                        return ;
                    }
                    if (cgi_status == 2)
                    {
                        map["Status_Code"] = server->code408;
                        return ;
                    }
                    map["if_CGI"] = "in";
                    map["Status_Code"] = server->code200;
                    return ;
                    
                }
            }
        
            current_dir = it->_getRoot();
            root = current_dir;
            if (!server->check_file(current_dir, Path))
            {
                
                map["Status_Code"] = server->code404;
                return ;
            }
            
            i = 0;
            
            i = current_dir.find_last_of('.');
            if (i != std::string::npos) 
                extension = current_dir.substr(i,  current_dir.size());
            //check mime_type
            
            map["Mime_type"] = server->check_mime_type(it->_getmimeTypes(), extension);

            homepage = it->_getIndex();
            flag_auto_index = it->_getAutoindex();
            
            for (std::vector<std::string>::const_iterator ss = homepage.begin(); ss != homepage.end(); ++ss)
                HOMEHOMEPAGE += *ss + ',';
            
            break;
        }
        else if (it == a.end() - 1 && current_dir.empty())
        { 
            counter = 0;
            flag = 0; 
            while (counter < server->getMethodesAllowed().size())
            {
                if (map["HTTP_Request"] == server->getMethodesAllowed()[counter])
                {
                    flag = 1;
                    break;
                }
                counter++;
            }
            if (flag == 0)
            {
                map["Status_Code"] = server->code405;
                return ;
            } 
            if (!server->getcginfo().empty() || !server->getcginfoPHP().empty())
            {
                
                int cgi_checker = server->check_cgi(current_dir, Path, server->getcginfo(), server->getcginfoPHP());
            
                int cgi_status = 0;
                
                if (cgi_checker != 2)
                {
                    if (cgi_checker == 0 && !server->getcginfo().empty())
                    {
                        cgi_status = cgi_execute(server->getcginfo()[1], current_dir, map);
                    }
                    else if (cgi_checker == 1 && !server->getcginfoPHP().empty())
                    {
                        cgi_status = cgi_execute(server->getcginfoPHP()[1], current_dir, map);
                        
                    }
                    else
                    {
                        map["Status_Code"] = server->code500;
                        return ;
                    }
                    if (cgi_status == 1)
                    {
                        map["Status_Code"] = server->code500;
                        return ;
                    }
                    if (cgi_status == 2)
                    {
                        map["Status_Code"] = server->code408;
                        return ;
                    }
                    map["if_CGI"] = "in";
                    {
                        map["Status_Code"] = server->code200;
                        return ;
                    }
                }
                
            }
            
            current_dir = server->getRoot();
            root = current_dir;
            if (!server->check_file(current_dir, Path))
            {
                map["Status_Code"] = server->code404;
                
                return ;
            }
            i = 0;
            i = current_dir.find_last_of('.');
            
            if (i != std::string::npos)
            {
                extension = current_dir.substr(i,  current_dir.size());
                map["Mime_type"] = server->check_mime_type(server->getmimeTypes(), extension);   
            }
            i = 0;
            while (i < server->getIndex().size())
                HOMEHOMEPAGE += server->getIndex()[i++] + ',';
        }
    }

    
    i = 0;
    
    struct stat current_dir_info;
    
    if (stat(current_dir.c_str(), &current_dir_info) == 0)
    {
        
        if (S_ISDIR(current_dir_info.st_mode)) 
        {      
            
            
            if (current_dir[current_dir.size() -1] != '/')
                current_dir += '/';
            std::istringstream lol(HOMEHOMEPAGE);
            std::string sub_path;
            struct stat inner_path_info;
            

            while (getline(lol, buffer, ','))
            {
                
                sub_path = current_dir+buffer;
                
                if (map["Mime_type"].empty())
                {
                    i = 0;
                    i = sub_path.find_last_of('.');
                    if (i != std::string::npos)
                    {
                        extension = sub_path.substr(i,  sub_path.size());
                        map["Mime_type"] = server->check_mime_type(server->getmimeTypes(), extension);
                    }
                }
            
                if (stat(sub_path.c_str(), &inner_path_info) == 0)
                {
                    if (S_ISDIR(inner_path_info.st_mode))
                    {
                        map["Status_Code"] = server->code403;
                        return ;
                    }
                    
                    if (access(sub_path.c_str(), R_OK) == 0)
                    {
                        map["FilePath"] = sub_path;
                        
                        // std::cout << map["FilePath"] << std::endl;
                        map["Status_Code"] = server->code200;
                        return ;
                    }
              
                    std::cerr << "Dir: Permissions issue" << std::endl;
                    map["Status_Code"] = server->code403;
                    return ;
                }
                sub_path.clear();
            }
            if (flag_auto_index)
            {
                
                DIR *access_current_dir;
                struct dirent *entry;
                
                access_current_dir = opendir(current_dir.c_str());
                if (access_current_dir != NOT_DIR)
                {
                    std::string ls_dir;
                    while((entry = readdir(access_current_dir)) != NULL)
                    {
                        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != ".." )
                        {
                           
                            int tp = track_path(root, current_dir);
                            if (tp != -1)
                            {
                                std::string trim = current_dir.substr(tp);
                                
                                if (trim[0] != '/')
                                    trim = '/' + trim;
                                
                                std::string temp = current_dir + std::string(entry->d_name);
                                if (stat(temp.c_str(), &inner_path_info) == 0)
                                {
                                    
                                    if (S_ISDIR(inner_path_info.st_mode))
                                    {
                                        ls_dir += trim + std::string(entry->d_name) +  '/' + '\n';
                                    }
                                    else
                                        ls_dir += trim  + std::string(entry->d_name) +  '\n';
                                }                             
                            }
                            else
                            {
                            
                                if (stat(std::string(entry->d_name).c_str(), &inner_path_info) == 0)
                                {
                                    if (S_ISDIR(inner_path_info.st_mode))
                                    {
                                        ls_dir += '/' +  std::string(entry->d_name) + '/' + '\n';
                                    }
                                    else
                                        ls_dir += '/' +  std::string(entry->d_name) + '\n';
                                }
                                
                            }
                        }
                        
                    }
                    closedir(access_current_dir);
                    
                    map["if_dir"] = ls_dir;
                    map["Status_Code"] = server->code200;
                    return ;
                }
                else
                {
                    //closedir(access_current_dir);
                    //403
                    std::cerr << "Unable to open dir" << std::endl;
                    map["Status_Code"] = server->code403;
                    return ;
                }
            }
            else
            {
                //403
                map["Status_Code"] = server->code403;
                return ;
            }
        }
        else 
        {
            if (access(current_dir.c_str(), R_OK) != -1) 
            {
                map["FilePath"] = current_dir;
                map["Status_Code"] = server->code200;
                return ;
            }
            std::cerr << "File: Permissions issue" << std::endl;
            map["Status_Code"] = server->code403;
            return ;
        } 
    }
    else if (errno == EACCES)
    {
        map["Status_Code"] = server->code403;
        return ;
    }
 
    map["Status_Code"] = server->code404;
  
    

    return ;
}

bool server:: end_of_header(std::string& buffer)
{
    if (!buffer.empty())
    {
        if (buffer[0] == 13 && buffer.size() == 1)
            return true;
    }
    return false;
}

bool FillingServerd_of_header(std::string& buffer)
{
    if (!buffer.empty())
    {
        if (buffer[0] == 13 && buffer.size() == 1)
            return true;
    }
    return false;
}

std::string server:: ParseHostHeader(std::istringstream& line_per_line, std::string& buffer, std::map <std::string, std::string> &map)
{
    std::string Host_value;
    int i = 0;
    while (getline(line_per_line, buffer, '\n'))
    {
        if (!buffer.empty())
        {
            if (end_of_header(buffer))
                throw code400;
            i = buffer.find(':');
            if (buffer.substr(0, i) == "Host")
            {
                Host_value = buffer.substr(i+1, buffer.size());
                TrimWhitespace(Host_value);
                map["Host"] = Host_value;
                return code200;
            }   
        }
    }
    return code400;
}

void  client:: requestParse(struct pollfd &p)
{  
    
   
    if (!Request.isHead && !Request.isBody)
    {
        
        return ;
    }
    std::string request = Request.Head;
    // std::cout << request << std::endl;
    int i = 0;
    std::string buffer;
    std::string key;
    std::string value;
    std::istringstream iss;
    if (request[0] == '\r' && request[1] == '\n')
        request = request.substr(2, request.size() - 1 );
    std::istringstream line_per_line(request);   
    getline(line_per_line, buffer, '\n');
    if (server->end_of_header(buffer))
    {
        map["Status_Code"] = server->code400;
        Response.isDone = true;
        p.events = POLLOUT;
        return ;
    }
    if (!buffer.empty())
    {
        if (buffer[0] == 13)
        {
            if (!check_intruder(buffer))
            {
                buffer = trim_char(buffer);
                getline(line_per_line, buffer, '\n');
                if (server->end_of_header(buffer))
                {

                    map["Status_Code"] = server->code400;
                    Response.isDone = true;
                    p.events = POLLOUT;
                    return ;
                }
            }
            else
            {
                map["Status_Code"] = server->code400;
                Response.isDone = true;
                p.events = POLLOUT;
                return ;
                //throw server->code400;
            }
        }
    }
   
    std::istringstream iss2(buffer);
    std::string fragment;
    if (!buffer.empty())
    {
        if (server->end_of_header(buffer))
        {
            map["Status_Code"] = server->code400;
            Response.isDone = true;
            p.events = POLLOUT;
            return ;
        }
        
        while (getline(iss2, fragment, ' '))
        {
            TrimWhitespace(fragment);
            if (fragment[0] == 13 && fragment.size() == 1)
                break;
            if (!fragment.empty())
            {
                if (map["HTTP_Request"].empty())
                    map["HTTP_Request"] = trim_char(fragment);
                else if (map["Request_Path"].empty())   
                {
                 
                    map["Request_Path"] = fragment;
                }
                else if (map["HTTP_version"].empty())
                {
                
                    if ( fragment[fragment.size() -1] == 13)
                        fragment = fragment.substr(0, fragment.size() - 1);
                    map["HTTP_version"] = fragment;
                    if (map["HTTP_version"] != "HTTP/1.1") 
                    {
                        map["Status_Code"] = server->code400;
                        Response.isDone = true;
                        p.events = POLLOUT;
                        return ;
                    }
                }
            }    
        }
        while (getline(line_per_line, buffer,'\n'))
        {
            if (server->end_of_header(buffer))
                break;
            TrimWhitespace(buffer);
            i = buffer.find(':');
            if (i != -1)
            {
                key = buffer.substr(0, i);
                if (buffer[i+1])    
                    value  = buffer.substr(i + 1, buffer.size());
                TrimWhitespace(value);
                map[key] = value;
            }
        }
        if (map["Host"].empty())
        {
            map["Status_Code"] = server->code400;
            Response.isDone = true;
            p.events = POLLOUT;
            return ;
            //throw server->code400; 
        }

        // check on Cookies
        if (!map["Cookie"].empty())
        {
            size_t i = 0;
            size_t j;
            bool isCookie = false;
            while (i < server->login_base.size())
            {
                j = server->login_base[i].find_last_of("UID");
                if (j != std::string::npos)
                {
                   
                    if (map["Cookie"] == server->login_base[i].substr(j + 2, 32))
                    {
                        map["check_Cookie"] = "out";
                        size_t t = server->login_base[i].find_last_of('&');
                        if (atoi(server->login_base[i].substr(t+1).c_str()) == 1)
                            server->login_base.erase(server->login_base.begin() + i);
                        else
                            server->login_base[i] = server->login_base[i].substr(0, t+1) + std::to_string(atoi(server->login_base[i].substr(t+1).c_str()) - 1);
                        isCookie = true;
                        break;
                    }
                }
                i++;
            }
            if (isCookie == false)
            {
               
                if (map["Request_Path"] != "/")
                {
                    map["Cookie"].clear();
                    map["Request_Path"] = "get_Cookie.html";
                }
            }
        }
        else if (map["Cookie"].empty())
        {
            
            if (map["Request_Path"] != "/")
            {
                map["Request_Path"] = "get_Cookie.html";
            }
        }
        
        if (map["HTTP_Request"] != "GET" && map["HTTP_Request"] != "POST" && map["HTTP_Request"] != "DELETE")
        {
            map["Status_Code"] = server->code501;
            Response.isDone = true;
            p.events = POLLOUT;
            return ;
            // throw server->code501;
        }
        if (map["HTTP_version"] != "HTTP/1.1")
        {
            map["Status_Code"] = server->code400;
            Response.isDone = true;
            p.events = POLLOUT;
            return ;
            // throw server->code400;
        } 
         
              
        
         
        // throw server->code404; // TO REMOVE 
        // if (ParseHostHeader(line_per_line, buffer) == "400")
        //     throw server->code400;
        if (map["HTTP_Request"] == "GET")
        {
           
            openFileSuccess(map["Request_Path"]);
            p.events = POLLOUT;
            return ;
                
        }
        else if (map["HTTP_Request"] == "DELETE")
        {
            Parse_DELETE(map["Request_Path"]);
            Response.isDone =true;
            p.events = POLLOUT;
            return ;
        }
        else if (map["HTTP_Request"] == "POST")
        {
            if (map["Transfer-Encoding"] != "chunked" && !map["Transfer-Encoding"].empty())
            {
                if (map["Content-Length"].empty())
                {
                    map["Status_Code"] = server->code411;
                }
                else if ((atol_l(map["Content-Length"]) == -1))
                {
                    if (atol_l(map["Content-Length"]) == -1)
                        map["Status_Code"] = server->code400;
                    else
                        map["Status_Code"] = server->code413;
                }
                else
                    map["Status_Code"] = server->code501;
                Response.isDone = true;
                p.events = POLLOUT;
                return ;
            }
            else if (map["Transfer-Encoding"].empty() && map["Content-Length"].empty())
            {
                map["Status_Code"] = server->code411;
                Response.isDone = true;
                p.events = POLLOUT;
                return ;
            }
            bool is = false;
            if (Request.isBody == false)
            {
                
                Parse_POST(map["Request_Path"], Request.Body);
                
                if (map["Status_Code"] != server->code200 && map["Status_Code"] != server->code201)
                {
                    
                    if (filefd != 0)
                        close(filefd);
                    Response.isDone = true;
                    p.events = POLLOUT;
                }
                is = true;
               
            }
            if (Request.isBody == true)
            {
                if (is != true)
                {
                    int f = write(filefd, Request.Body.c_str(), Request.Body.size());
                    if (f == -1)
                    {
                        map["Status_Code"] = server->code500;
                        Response.isDone = true;
                        p.events = POLLOUT;
                    }
                    else if (f == 0)
                        ;
                }
                close(filefd);
                Response.isDone = true;
                p.events = POLLOUT;
            }
            // Call Parse_POST
            //COntent length    
        } 
    }
    // map["Status_Code"] = server->code200;
    // std::cout << "200 OK" << std::endl ;
    // }
    // catch(std::string status_code)
    // {
    //     map["Status_Code"] = status_code;
    //     // map.clear();
    //     return map;
    // }
    return ;
}

void client:: Parse_DELETE(std::string Path)
{
    if (Path.empty())
    {
        map["Status_Code"] = server->code400;
        return;
    }
        
    std::size_t f =  Path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._:~?/#[]@!$&'()*+,;=%");
    if (f != std::string::npos) 
    {
        map["Status_Code"] = server->code400;
        return;
    }
    if (Path.size() > 2048)
    {
        map["Status_Code"] = server->code414;
        return;
    }
    if (Path.size()  >= 3 &&(Path.find("/..") == Path.size() - 3 || Path.find("/../") != std::string::npos))
    {
        map["Status_Code"] = server->code403;
        return ;
    }
    trim_slash(Path);
    size_t i = 0;
    std::string current_dir;
    std::ifstream file_object;
    std::vector<std::string> homepage;
    std::string HOMEHOMEPAGE;
    std::string buffer;
    std::string extension;
    bool flag_auto_index = server->getAutoindex();
    std::vector <location> a = server->getLocations();
    int cgi_status;
    size_t counter = 0;
    int flag = 0;
    size_t t = 0;
    std::string Path_extension;
    for (std::vector<location>::const_iterator it = a.begin(); it != a.end(); ++it)
    {    
        Path_extension = get_Extension(Path);
        t = Path.find(it->_getPathLocation());
        if (Path == it->_getPathLocation() ||  t == 0 || (!Path_extension.empty() && it->_getPathLocation() == "*" + Path_extension))
        {
            // if (t != std::string::npos && t <= Path.size())
            // {
            //     Path.erase(0, t);
            // }
            while (counter < it->_getMethodesAllowed().size())
            {
                if (map["HTTP_Request"] == it->_getMethodesAllowed()[counter])
                {
                    flag = 1;
                    break;
                }
                counter++;
            }
            if (flag == 0)
            {
                map["Status_Code"] = server->code405;
                return ;
            } 
            cgi_status = server->check_cgi(current_dir, Path, it->_getcginfo(), it->_getcginfoPHP());
            if (cgi_status == 1 || cgi_status == 0)
            {
                map["Status_Code"] = server->code501;
                return;
            }
            current_dir = it->_getRoot();
            if (!server->check_file(current_dir, Path))
            {
                map["Status_Code"] = server->code404;
                return;
            }
            homepage = it->_getIndex();
            flag_auto_index = it->_getAutoindex();
            for (std::vector<std::string>::const_iterator ss = homepage.begin(); ss != homepage.end(); ++ss)
                HOMEHOMEPAGE += *ss + ',';
            break;
        }
        else if (it == a.end() - 1 && current_dir.empty())
        {
            counter = 0;
            flag = 0; 
            while (counter < server->getMethodesAllowed().size())
            {
                if (map["HTTP_Request"] == server->getMethodesAllowed()[counter])
                {
                    flag = 1;
                    break;
                }
                counter++;
            }
            if (flag == 0)
            {
                map["Status_Code"] = server->code405;
                return ;
            } 
            cgi_status = server->check_cgi(current_dir, Path, server->getcginfo(), server->getcginfoPHP());
            if (cgi_status == 1 || cgi_status == 0)
            {
                map["Status_Code"] = server->code501;
                return;
            }
            current_dir = server->getRoot();
            if (!server->check_file(current_dir, Path))
            {
                map["Status_Code"] = server->code404;
                return;
            }
            while (i < server->getIndex().size())
                HOMEHOMEPAGE += server->getIndex()[i++] + ',';
        }
    }
    i = 0;
    struct stat current_dir_info;
    if (stat(current_dir.c_str(), &current_dir_info) == 0)
    {
        if (S_ISDIR(current_dir_info.st_mode)) 
        {       
            if (current_dir[current_dir.size() -1] != '/')
                current_dir += '/';
            std::istringstream lol(HOMEHOMEPAGE);
            std::string sub_path;
            struct stat inner_path_info;
            while (getline(lol, buffer, ','))
            {
                sub_path = current_dir+buffer ;
                if (stat(sub_path.c_str(), &inner_path_info) == 0)
                {
                    
                    if (access(sub_path.c_str(), W_OK) != 0)
                    {
                        if (remove(sub_path.c_str()) != 0)
                        {
                            std::cerr << "Deleting Permissions denied" << std::endl;
                            map["Status_Code"] = server->code403;
                            return;
                        }
                        map["DELETE_path"] = sub_path;
                        map["Status_Code"] = server->code200;
                        return;
                    }
                    std::cerr << "Dir: Permissions issue" << std::endl;
                    map["Status_Code"] = server->code403;
                    return;
                }
                sub_path.clear();
            }
            map["Status_Code"] = server->code403;
            return;
        }
        else
        {
            if (access(current_dir.c_str(), R_OK) != 0)
            {
                map["DELETE_path"] = current_dir;
                if (remove(current_dir.c_str()) != 0)
                {
                    std::cerr << "Deleting Permissions denied" << std::endl;
                    map["Status_Code"] = server->code403;
                    return;
                }
                map["DELETE_path"] = current_dir;
                
               
                map["Status_Code"] = server->code200;
                return;
            }
            std::cerr << "File: Permissions issue" << std::endl;
            map["Status_Code"] = server->code403;
            return;
        }
    }
    else if (errno == EACCES)
    {
        map["Status_Code"] = server->code403;
        return;
    }
    map["Status_Code"] = server->code404;
    return;
}   

void client:: GenerateBody()
{
    // Generate Response Headers:
    struct timeval currentTime;
    gettimeofday(&currentTime, nullptr);
    time_t rawtime = currentTime.tv_sec;
    struct tm* gmTime = gmtime(&rawtime);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %T GMT", gmTime);
    std::string str(buffer);
    std::string response_headers;
    std::string Response_Body;
    map["code400"] = "Bad Request";
    map["code200"] = "OK";
    map["code301"] = "Moved Permanently";
    map["code401"] = "Unauthorized";
    map["code403"] = "Forbidden";
    map["code404"] = "Not Found";
    map["code500"] = "Internal Server Error";
    map["code501"] = "Not Implemented";
    map["code411"] = "Length Required";
    map["code414"] = "Length Too long";
    map["code408"] = "Request Timeout";
    map["code405"] = "Method Not Allowed";
    if (!map["HTTP_version"].empty() && !map["Status_Code"].empty())
        response_headers += map["HTTP_version"] + " " + map["Status_Code"] + " " + map["code"+map["Status_Code"]] + "\r\n"; 
    if (!str.empty())
        response_headers += "Date: " + str + "\r\n";
    //if (!map["ServerName"].empty())  servername = webserv
        response_headers += "Server: webserv\r\n";
    // if (map["Host"][map["Host"].size() -1] == '/' && map["Request_Path"][0] == '/')
    //     map["Host"] = map["Host"].erase(map["Host"].size() -1);
    if (!map["Connection"].empty())
        response_headers += "Connection: " + map["Connection"]  + "\r\n";
    
    if (!map["check_Cookie"].empty())
    {
       
        if (map["check_Cookie"] == "in")
        {
            response_headers += "Set-Cookie: "  + map["Cookie_CGI"] + "\r\n";
            
        }
        else if (map["check_Cookie"] == "out")
        {
            
           response_headers += "Cookie: " +  map["Cookie"]  + "\r\n";
        }
        else if (!map["Cookie_Body"].empty())
        {
            
            Response_Body = map["Cookie_Body"]; 
            
            response_headers += "Content-Length: " + std::to_string(Response_Body.size()) + "\r\n\r\n";
            Response.responseToClient = response_headers + Response_Body;
            Response.count = Response.responseToClient.size();
            Response.isDone = true;
            return ; 
            
        }
    }
    // Generate Response body: 
    //if Success:
    if (!map["location_href"].empty())
    {
        Response_Body = map["location_href"];
    }
    
    else if (map["if_CGI"] == "in")
    {
        Response_Body = map["CGI_Response"];
    }
   
    else if (!map["FilePath"].empty())
    {
        std::ifstream f(map["FilePath"].c_str(), std::ios::binary);
        struct stat fstat;
        stat(map["FilePath"].c_str(), &fstat);
        char  body[1024];
        memset(body, 0, sizeof(body));
        f.seekg(static_cast<std::streamoff>(Response.readOffset), std::ios::beg);
        f.read(body, sizeof(body));
        Response.count = f.gcount();
        if (Response.count <= 0)
        {
            if (Response.count == 0)
            {
                Response.responseToClient = response_headers;
                Response.count = response_headers.size();
                Response.isDone = true;
                f.close();
                return;
            }
            else
            {
                Response.isDone = true;
                f.close();
                map["Status_Code"] = server->code500;
                throw 0;
            }
        }
        if (Response.count + Response.readOffset >= static_cast<size_t>(fstat.st_size))
            Response.isDone = true;
        f.close();
        Response.responseToClient.append(body, Response.count);
        if (Response.WriteBody == true)
        {
            Response.SetupOffset = true;
            return;
        }
        
        map["Content_Length"] = "Content-Length: " + std::to_string(fstat.st_size) + "\r\n";
        Response.WriteHead = true;
        if (!map["Mime_type"].empty())
            response_headers += "Content-Type: " + map["Mime_type"] + "\r\n";
        //check if mimetype empty;
        else
        {
            map["Mime_type"] = server->getmimeDefaul();
            response_headers += "Content-Type: " + map["Mime_type"] + "\r\n";
        }
    }
    
    else if (!map["if_dir"].empty())
    {
        std::istringstream lol(map["if_dir"]);
        std::string buffer;
        std::string outputFile = "<html> <body>";
        while (getline(lol, buffer, '\n'))
            outputFile += "<h1><a href=\"" +  buffer + "\">" + buffer +"</a></h1>" ;
        outputFile += "</body></html>";
        Response_Body = outputFile;
    }
    else if (!map["DELETE_path"].empty())
    {
        Response_Body += "<html><h1>File: " + map["DELETE_path"] + " Deleted Successfuly </h1></html>";
    }
    else
    {
        try
        {
            std::string errorpage_path;
            std::vector <location> a = server->getLocations();
            size_t i = 0;
            size_t t = 0;
            // checking if errorpage directive is in location: 
            if (!map["Request_Path"].empty()) 
            {
                for (std::vector<location>::const_iterator it = a.begin(); it != a.end(); ++it)
                {
                    t = map["Request_Path"].find(it->_getPathLocation());
                    if (map["Request_Path"] == it->_getPathLocation() || t == 0 || it->_getPathLocation() == "*" + get_Extension(map["Request_Path"]))
                    {
                        if (!it->_getError_pages().empty())
                        {
                            while (i < it->_getError_pages().size())
                            {
                                std::size_t j = it->_getError_pages()[i].find("=");
                               
                                if (it->_getError_pages()[i].substr(0, j) == map["Status_Code"])
                                {
                                    errorpage_path = it->_getError_pages()[i].substr(j + 1, it->_getError_pages()[i].size());
                                    break;
                                }
                                i++;
                            }
                        }
                    }  
                }
            }
            i = 0;
            // errorpage directive not in location : checking if errorpage directive is in server 
            if (errorpage_path.empty())
            {
                while (i < server->getError_pages().size())
                {
                    std::size_t j = server->getError_pages()[i].find("=");
                    if (server->getError_pages()[i].substr(0, j) == map["Status_Code"])
                    {
                        errorpage_path = server->getError_pages()[i].substr(j + 1, server->getError_pages()[i].size());
                        break;
                    }
                    i++;
                }
            }
            int fd;
            //errorpage directive is neither in location nor in server, checking if page is in default directory 
            if (errorpage_path.empty())
            {
                errorpage_path = "./www/error_pages/" + map["Status_Code"] + ".html";
               
                if (access(errorpage_path.c_str(), R_OK) != 0)
                {
                    
                    throw 0;
                }
                      
            }
            fd = open(errorpage_path.c_str(), O_RDONLY);
            char  body;
            while (read(fd, &body, 1) > 0)
            {
                Response_Body = Response_Body + body;
                if (Response_Body.size()  >= 2000)
                {
                    map["Status_Code"] = server->code413;
                    throw 0;
                }
            }
            close(fd);       
        }
        catch(int error)
        {
            if (error == 0)
            {
                Response_Body = "<html><h1> Error " + map["Status_Code"] + "</h1> <html> ";
            }
        }
    }
    if (map["Content_Length"].empty())
        map["Content_Length"] = "Content-Length: " + std::to_string(Response_Body.size()) + "\r\n";
    response_headers += map["Content_Length"] + "\r\n";
    
    if (Response.WriteHead == true && Response.WriteBody == false)
    {
        Response.responseToClient = response_headers + Response.responseToClient;
        Response.WriteBody = true;
        Response.HeadSize = response_headers.size();
        Response.count += response_headers.size();
    }
    else
    {
       
        Response.responseToClient = response_headers + Response_Body;
        Response.count = Response.responseToClient.size();
        Response.isDone = true;
    }
}

size_t power(size_t value,size_t param)
{
    size_t t = 1;
    size_t i = 1;
    while (i <= param)
    {
        t *= value;
        i++;
    }
    return t;
}

size_t hex_to_dec(std::string hex)
{
    size_t i = 0;
    size_t len = hex.size() ;
    size_t p = len - 1 ;
    size_t num = 0;
    while(i < len)
    {
        hex[i] = std::tolower(hex[i]);
        if (hex[i] >= '0' && hex[i] <= '9')
        {
            // std::cout << hex[i] - 48 << std::endl;
            num = num + (hex[i] - 48) * power(16, p);
            p--;
        }
        else if (hex[i] >= 'a' && hex[i] <= 'f')
        {
            
            num = num + (hex[i] - 87) * power(16, p);
            p--;
        }
        i++;
    }
    return num;
}
void client::  process_chunked_CGI(std::string &request_body)
{
    if (Response.insideS == true)
    {
        if (!Response.stock.empty())
        {
            request_body = Response.stock + request_body;
            request_body.erase(0, 2);
            Response.stock.clear();
        }
        else
            request_body.erase(0, 2);
        Response.insideS = false;
    }
    if (Response.sizeIsdone == true)
    {
        size_t find = request_body.find_first_of("\r\n", 0);
        Response.sizeOfchunk = hex_to_dec(request_body.substr(0, find));
        Response.size_length += Response.sizeOfchunk;
        if (Response.sizeOfchunk == 0  || Response.size_length > Response.mbc)
        {
            if (Response.size_length > Response.mbc)
                map["Status_Code"] = server->code413;
            else
            {
                
                Request.isBody = true;
                CGI_POST_normalcase(request_body);
            }
            return;
        }
        request_body.erase(0, find + 2);
        Response.sizeIsdone = false;
    }
    if (Response.sizeOfchunk <= request_body.size())
    {
        map["CgiPost_Body"].append(request_body.c_str(), Response.sizeOfchunk);
        Response.stock = request_body.erase(0, Response.sizeOfchunk);
        Response.sizeOfchunk = 0;
        Response.sizeIsdone = true;
        Response.insideS = true;
    }
    else
    {
        map["CgiPost_Body"].append(request_body);
        Response.sizeOfchunk -= request_body.size();
    }
    if (Response.stock.find("0\r\n\r\n") != std::string::npos)
    {
        
        Request.isBody = true;
        CGI_POST_normalcase(request_body);
        return ;        
    }
}
void client::  process_chunked(std::string &request_body)
{
    int count;
    if (Response.insideS == true)
    {
        if (!Response.stock.empty())
        {
            request_body = Response.stock + request_body;
            request_body.erase(0, 2);
            Response.stock.clear();
        }
        else
            request_body.erase(0, 2);
        Response.insideS = false;
    }
    if (Response.sizeIsdone == true)
    {
        size_t find = request_body.find_first_of("\r\n", 0);
        
        Response.sizeOfchunk = hex_to_dec(request_body.substr(0, find));
        Response.size_length += Response.sizeOfchunk;
        if (Response.sizeOfchunk == 0  || Response.size_length > Response.mbc)
        {
            if (Response.size_length > Response.mbc)
            {
                
                remove(map["POST_file"].c_str());
                map["Status_Code"] = server->code413;
            }
            else
            {
                map["Status_Code"] = server->code201;
                Request.isBody = true;
            }
            return;
        }
        request_body.erase(0, find + 2);
        Response.sizeIsdone = false;
    }
    if (Response.sizeOfchunk <= request_body.size())
    {
       
        count = write(filefd, request_body.c_str(), Response.sizeOfchunk);
        if (count == -1)
        {
            map["Status_Code"] = server->code500;
            return;
        }
        if (count == 0)
            ;
        
        Response.stock = request_body.erase(0, Response.sizeOfchunk);
        Response.sizeOfchunk -= count;
        Response.sizeIsdone = true;
        Response.insideS = true;
    }
    else
    {
        count = write(filefd, request_body.c_str(), request_body.size());
        if (count == -1)
        {
            map["Status_Code"] = server->code500;
            return;
        }
        if (count == 0)
            ;
        Response.sizeOfchunk -= count;
    }
    if (Response.stock.find("0\r\n\r\n") != std::string::npos)
    {
        map["Status_Code"] = server->code201;
        Request.isBody = true;
        return ;        
    }
}


void client:: CGI_POST_normalcase(std::string &body)
{
    int cgi_status;
    if (Request.isBody == true)
    {
        cgi_status = cgi_execute(map["Compiler"], map["cgi_file"], map);
        
        if (cgi_status == 1)
        {
            map["Status_Code"] = server->code500;
        }
        else if (cgi_status == 2)
        { 
            map["Status_Code"] = server->code408;
        }
        else
        {
            map["if_CGI"] = "in";
            map["Status_Code"] = server->code200;
        }
        return ;
    }
    if (!map["Content-Length"].empty() && body.size() == (size_t)atol_l(map["Content-Length"].c_str()))
    {
        Request.isBody = true;
        map["CgiPost_Body"].append(body);
        CGI_POST_normalcase(body);
        return ;
    }
    map["CgiPost_Body"].append(body);
}
void client:: Parse_POST(std::string Path, std::string request_body)
{

    if (Request.is_already_checked)
    {   
        if (map["Transfer-Encoding"] == "chunked")
        {
            process_chunked(Request.Body);
        }
        else
        {
            int f = write(filefd, request_body.c_str(), request_body.size());
            if (f == -1)
            {
                map["Status_Code"] = server->code500;
                Response.isDone = true;
            }
            else if (f == 0)
                ;
        }
        if (map["Status_Code"].empty())
        {   
            map["Status_Code"] = server->code200;
        }
        return ;
    } 
    int size = 0;
    if (Path.empty())
    {
        map["Status_Code"] = server->code400;
        return ;
    }
    std::size_t f =  Path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._:~?/#[]@!$&'()*+,;=%");
    if (f != std::string::npos)
    {
        map["Status_Code"] = server->code400;
        return ;
    } 
        
    if (Path.size() > 2048)
    {
        map["Status_Code"] = server->code414;
        return ;
    }
    if (Path.size()  >= 3 && (Path.find("/..") == Path.size() - 3 || Path.find("/../") != std::string::npos))
    {
        map["Status_Code"] = server->code403;
        return ;
    }
    trim_slash(Path);
    std::string upload;
    std::string uploadPath;
    std::string current_dir;
    size_t i = 0;
    std::string extension;
    std::vector <location> a = server->getLocations();
    std::vector<std::string> homepage;
    std::string HOMEHOMEPAGE;
    struct timeval currentTime;
    gettimeofday(&currentTime, nullptr);
    time_t rawtime = currentTime.tv_sec;
    struct tm* gmTime = gmtime(&rawtime);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%H%M%S", gmTime);
    std::string str(buffer);
    size_t counter = 0;
    int flag = 0;
    size_t t = 0;
    std::string Path_extension;
    for (std::vector<location>::const_iterator it = a.begin(); it != a.end(); ++it)
    {
        Path_extension = get_Extension(Path);
        t = Path.find(it->_getPathLocation());
        if (Path == it->_getPathLocation() || t == 0 || (!Path_extension.empty() && it->_getPathLocation() == "*" + Path_extension))
        {
            // if (t == 0)
            // {
            //     Path.erase(0, t);
            // }
            while (counter < it->_getMethodesAllowed().size())
            {
                if (map["HTTP_Request"] == it->_getMethodesAllowed()[counter])
                {
                    flag = 1;
                    break;
                }
                counter++;
            }
            if (flag == 0)
            {
                map["Status_Code"] = server->code405;
                return ;
            } 
            Response.mbc = it->_getMax_body_size();
            
            if (!it->_getcginfo().empty() || !it->_getcginfoPHP().empty())
            {
                int cgi_checker = server->check_cgi(current_dir, Path, it->_getcginfo(), it->_getcginfoPHP());
                
                if (cgi_checker != 2)
                {
                    if (cgi_checker == 0 && !it->_getcginfo().empty())
                    {                  
                        map["Compiler"] = it->_getcginfo()[1];
                        map["cgi_file"] = current_dir;
                        
                        if (map["Transfer-Encoding"] == "chunked")
                            process_chunked_CGI(Request.Body);
                        else
                        {
                            CGI_POST_normalcase(Request.Body);
                        }
                       
                    }
                    else if (cgi_checker == 1 && !it->_getcginfoPHP().empty())
                    {
                        map["Compiler"] = it->_getcginfoPHP()[1];
                        map["cgi_file"] = current_dir;
                        if (map["Transfer-Encoding"] == "chunked")
                            process_chunked_CGI(Request.Body);
                        else
                        {
                            CGI_POST_normalcase(Request.Body);
                        }
                    }
                    return;   
                }
            }
            //
            current_dir = it->_getRoot();
            if (!server->check_file(current_dir, Path))
            {
                if (it->_getupload())
                {
                    upload = "On";
                    current_dir = it->_getUploadPath();
                    if (!server->check_file(current_dir, Path))
                    {
                        map["Status_Code"] = server->code404;
                        return ;
                    }
                }
                else
                {
                    upload = "Off";
                    map["Status_Code"] = server->code404;
                    return ;
                }
            }
            
            
            if (!map["Content-Length"].empty() &&  atol_l(map["Content-Length"]) > Response.mbc)
            {
                
                map["Status_Code"] = server->code413;
                return;
            } 
            extension = server->return_extension(it->_getmimeTypes(), map["Content-Type"]);
        }
        else if (it == a.end() - 1 && upload.empty())
        {
            counter = 0;
            flag = 0; 
            while (counter < server->getMethodesAllowed().size())
            {
                if (map["HTTP_Request"] == server->getMethodesAllowed()[counter])
                {
                    flag = 1;
                    break;
                }
                counter++;
            }
            if (flag == 0)
            {
                map["Status_Code"] = server->code405;
                return ;
            } 
            Response.mbc = server->getMax_body_size();
            
            if (!server->getcginfo().empty() || !server->getcginfoPHP().empty())
            {
                int cgi_checker = server->check_cgi(current_dir, Path, server->getcginfo(), server->getcginfoPHP());
            
                if (cgi_checker != 2)
                {
                    if (cgi_checker == 0 && !server->getcginfo().empty())
                    {
                        map["Compiler"] = server->getcginfo()[1];
                        map["cgi_file"] = current_dir;
                        if (map["Transfer-Encoding"] == "chunked")
                            process_chunked_CGI(Request.Body);
                        else
                        {
                            CGI_POST_normalcase(Request.Body);
                        }
                        return;   
                        
                    }
                    else if (cgi_checker == 1 && !server->getcginfoPHP().empty())
                    {
                        map["Compiler"] = server->getcginfo()[1];
                        map["cgi_file"] = current_dir;
                        if (map["Transfer-Encoding"] == "chunked")
                            process_chunked_CGI(Request.Body);
                        else
                        {
                            CGI_POST_normalcase(Request.Body);
                        }
                        return;   
                       
                    }
                    
                }
            }
            //
            extension = server->return_extension(server->getmimeTypes(), map["Content-Type"]);
            current_dir = server->getRoot();
            if (!server->check_file(current_dir, Path))
            {  
                if (server->getupload())
                {
                    upload = "On";
                    current_dir = server->getUploadPath();
                    if (!server->check_file(current_dir, Path))
                    {
                        map["Status_Code"] = server->code404;
                        return ;
                    }
                }
                else
                {
                    upload = "Off";
                    map["Status_Code"] = server->code404;
                    return ;
                }
            }
            
            if (!map["Content-Length"].empty() &&  atol_l(map["Content-Length"]) > Response.mbc)
            {
                
                map["Status_Code"] = server->code413;
                return;
            } 
        }
    }
    // int t = request_.find("\r\n\r\n", 0);
    // std::string request_body = request_.substr(t+4);
    
    struct stat Path_info;
    
    Path = current_dir;
    if (stat(Path.c_str(), &Path_info) == 0)
    {
        std::string tempfile;
        if (S_ISDIR(Path_info.st_mode))
        {   
            if (current_dir[current_dir.size() - 1] != '/')
                tempfile = current_dir + "/" + str + extension;
            else
                tempfile = current_dir + str + extension;
            filefd = open(tempfile.c_str(), O_CREAT | O_RDWR, 0777);
            if (filefd == -1 || !filefd) 
            {    
                std::cerr << "Creating file permission denied" << std::endl;
                //POLLOUT
                map["Status_Code"] = server->code403;
                return ; // Forbidden
            }
            // handle_content_type(&request_body);
            Request.is_already_checked = true;
            map["POST_file"] = tempfile;
            if (map["Transfer-Encoding"] == "chunked")
            {
                process_chunked(Request.Body);
                map["Status_Code"] = server->code201;
            }
            else
            {
                size = write(filefd, request_body.c_str(), request_body.size());
                if (size == -1)
                {
                    map["Status_Code"] = server->code500;
                    Response.isDone = true;
                }
                else if (size == 0)
                    ;
                if (size > Response.mbc)
                {
                    remove(map["POST_file"].c_str());
                    map["Status_Code"] = server->code413;
                    return;

                }
                    
                map["Status_Code"] = server->code201; 
             
            }
            if (!map["Content-Length"].empty() && size == atol_l(map["Content-Length"].c_str()))
            {   
                close (filefd);
                Request.isBody = true;
                map["Status_Code"] = server->code201;
            }
            return ;
        }
    }
   
    i = Path.find_last_of('/');
    
    if (i != std::string::npos)
    {
        // std::string newfile = Path.substr(i + 1, Path.size());
       
        // if (Path.back() != '/')
        //     newfile = Path + "/" + newfile;
        // else
        //     newfile = Path + newfile;
        // std::cout << newfile << std::endl;
        if (stat(Path.c_str(), &Path_info) != 0)
        {
            filefd = open(Path.c_str(), O_CREAT | O_RDWR, 0777);
            if (filefd == -1 || !filefd) 
            {
                map["Status_Code"] = server->code403;
                return ; // Forbidden
            }
            // handle_content_type(&request_body); 
            Request.is_already_checked = true;
            map["POST_file"] = Path;
            if (map["Transfer-Encoding"] == "chunked")
            {
                process_chunked(Request.Body);
                map["Status_Code"] = server->code200;
            }
            else 
            {
                size = write(filefd, request_body.c_str(), request_body.size());
                if (size == -1)
                {
                    map["Status_Code"] = server->code500;
                    Response.isDone = true;
                }
                else if (size == 0)
                    ;
                map["Status_Code"] = server->code200;
            }
            
            if (!map["Content-Length"].empty() && size == atol_l(map["Content-Length"].c_str()))
            {

                Request.isBody = true;
                close (filefd);
                map["Status_Code"] = server->code201;
            }
            
            return ;
        }
        std::cerr << "File already exists" << std::endl;
        map["Status_Code"] = server->code400;
        return ;
    }
    return ;
}
//GET
// determine type of file with mime type from config file to put in in content type header
// determine lenght of file to put in content length attribute
//POST // 0