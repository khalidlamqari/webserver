#include <iostream>
#include <sys/stat.h>
#include <dirent.h>

void f()
{
    system("leaks ss") ;
}

int main()
{
    // atexit(f);
    // DIR * d ;

    // struct dirent *f ;

    // d = opendir("../utils") ;
    // if ( !d )
    //     return 1 ;
    // else
    // {
    //     std::cout << d << std::endl;
    // }
    
    // while ( ( f = readdir(d) ) && d != NULL )
    // {
    //     std::cout << f->d_name << std::endl;
    // }

    // free(d) ;

    //---------------
    struct stat s ;

    if ( stat("../utils/splfffit.cpp", &s) == -1 )
    {
        std::cout << "stat() failed" << std::endl;
        return 1;
    }
    // if ( S_ISDIR(s.st_mode) )
    if ( s.st_mode & S_IFDIR )
        std::cout << "is a directory " << std::endl;
    else
        std::cout << "is not a directory " << std::endl;
}