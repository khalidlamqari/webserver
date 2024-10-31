#include <iostream>

void remove_from_last_slash( std::string & target )
{
    size_t end = target.find_last_of('/') ;
    if ( end != std::string::npos )
    {
        if ( target.back() == '/' )
            target.pop_back();
        else
            target = target.substr(0, end + 1) ;
    }
    else
        target = "" ;
}

int main()
{
    std::string new_taget = "/about/aaaaa";
    while ( true )
    {

    
        remove_from_last_slash( new_taget ) ;
        std::cout << " new_target  " << new_taget << std::endl;
        if ( new_taget == "" ) // if all paths removed / / /
            break ;
    }

}