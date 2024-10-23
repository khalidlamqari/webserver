#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{

    int n_fd ;
    struct  sockaddr_in address ;
    memset(&address, 0, sizeof(address)) ;
    address.sin_family = AF_INET ;
    address.sin_addr.s_addr = INADDR_ANY ;
    address.sin_port = htons(5000) ;
   
    int s = socket(AF_INET, SOCK_STREAM, 0) ;



    bind( s, (struct sockaddr*)(&address),  sizeof(address)) ;

    listen( s, 5 );
    while (true)
    {

        n_fd = accept( s, (struct sockaddr * )&address, (socklen_t *)&( address.sin_len ) ) ;
        char message[1024] ;
        read( n_fd, message, 1024 ) ;

        std::cout << "message : " << message << std::endl;
        close(n_fd) ;
    }
    close(s);
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