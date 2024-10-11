#include <iostream>
#include <string>
#include <vector>
#include <map>



int main()
{


    std::map<std::string , std::string > m ;
    m["1"] = "111";
    m["1"] = "2222";

    std::cout << m["1"] << std::endl;
    m.key_comp();
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