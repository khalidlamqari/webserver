#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
std::vector<std::string> _split_(const std::string & str, char c)
{
    std::vector<std::string> strings ;


    size_t i = 0 ;
    size_t start , end;
    
    while ( i < str.length() )
    {
        while ( i < str.length() && str[i] == c ) 
            i++ ;
        
        start = i ;
        
        while ( i < str.length() && str[i] != c ) 
            i++;

        end = i;

        if (start == end)
            break ;
        
        strings.push_back( str.substr( start, end - start ) );
    }
    return ( strings );
}

void normalize_target(std::string &target)
{
    if ( target.find("..") == std::string::npos )
        return ;
    std::vector<std::string> directories = _split_(target, '/');
    std::vector<std::string>::iterator it = directories.begin();
    while (it != directories.end())
    {
        if (it == directories.begin() && *it == "..")
            throw 403;
        if (*it != ".." && it + 1 != directories.end() && *(it + 1) == "..")
        {
            it = directories.erase(it, it + 2);
            if (it != directories.begin())
                --it;
        }
        else
            ++it;
    }
    target = "/";
    for (std::vector<std::string>::iterator it = directories.begin() ; it != directories.end() ; ++it) 
        target += "/" + *it;
}

std::string get_content_type( const std::string & file_name )
{
    std::string imgs[] = {".png",".avif", ".gif", ".webp", ".dmp", ".apng"} ;
    std::string jpeg[] = {".jpg", ".jpeg", ".jfif", ".pjpeg", ".pjp"} ;
    std::string ico[] = {".ico", ".cur"} ;
    std::string texts[] = {".txt", ".html", ".htm", ".css", ".js"} ;

    for (size_t i = 0; i < texts->length(); ++i)
    {
        if ( file_name.find(texts[i], file_name.length() - texts[i].length() ) != std::string::npos )
        {
            if ( texts[i] == ".js" || texts[i] == ".javascript")
                return ( "\r\nContent-Type: text/javascript; charset=utf-8") ;
            return ( "\r\nContent-Type: text/" +  texts[i].erase(0, 1) + "; charset=utf-8") ;
        }
    }
    for (size_t i = 0; i < imgs->length(); ++i)
    {
        if ( file_name.find(imgs[i], file_name.length() - imgs[i].length()) != std::string::npos )
            return ( "\r\nContent-Type: image/" +  imgs[i].erase(0, 1) ) ;
    }
    for ( size_t i = 0; i < jpeg->length() ; ++i)
    {
        if ( file_name.find(file_name.length() - jpeg[i].length()) != std::string::npos)
            return ( "\r\nContent-Type: image/jpeg" ) ;
    }
    for (size_t i = 0; i < ico->length(); ++i)
    {
        if ( file_name.find(file_name.length() - ico[i].length()) != std::string::npos )
            return ( "\r\nContent-Type: image/x-icon" ) ;
    }
    return ("\r\nContent-Type: text/plain") ;
}

int main()
{

    std::string file =  "index.html";

    std::cout << get_content_type(file) << std::endl;


    // std::string ss = "/path1/../.."  ;// /path1/path2/
    // normalize_target( ss ) ;

    // std::cout << ss << std::endl;

    // int n_fd ;
    // struct  sockaddr_in address ;
    // memset(&address, 0, sizeof(address)) ;
    // address.sin_family = AF_INET ;
    // address.sin_addr.s_addr = INADDR_ANY ;
    // address.sin_port = htons(5000) ;
   
    // int s = socket(AF_INET, SOCK_STREAM, 0) ;



    // bind( s, (struct sockaddr*)(&address),  sizeof(address)) ;

    // listen( s, 5 );
    // while (true)
    // {
    //     n_fd = accept( s, (struct sockaddr * )&address, (socklen_t *)&( address.sin_len ) ) ;
    //     char message[1024] ;
    //     read( n_fd, message, 1024 ) ;

    //     std::cout << "message : " << message << std::endl;
    //     close(n_fd) ;
    // }
    // close(s);
    // std::map<std::string , std::string > m ;
    // m["1"] = "111";
    // m["1"] = "2222";

    // std::cout << m["1"] << std::endl;
    // m.key_comp();
    // std::vector<std::string> ss = split("hello \r\r  world\r\r\n");

    // std::vector<std::string>::iterator it = ss.begin();
    // std::vector<std::string>::iterator end = ss.end();


    // while (it != end )
    // {
    //     std::cout << "|" <<  *it << "|" << std::endl;
    //     it++ ;
    // }

    // std::string s = "field-name: field-value " ;
    // size_t i =  s.find( ":" ); 
    // if (i == std::string::npos)
    // {
    //     std::cout << "not found!" << std::endl;
    //     return 1 ;
    // }
    // std::string str = s.substr( 0, i );

    // std::cout << str << std::endl ;


}