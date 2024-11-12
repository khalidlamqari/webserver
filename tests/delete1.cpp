#include <iostream>
#include <cstdio>


int main()
{
    int status;

    status = std::remove("ss");

    if ( status == -1 )
        std::cout << "failed to delete file " << std::endl ;
    else
        std::cout << "successful deleting" << std::endl ;
    
}

