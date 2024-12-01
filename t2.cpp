#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>

bool is_existe_file(std::string path)
{
    struct  stat f;
    
    if ( stat(path.c_str(), &f ) == -1 )
        return false;
    if ( f.st_mode & S_IFREG )
        return true;
    return false;
}

void a()
{
    char *s;
    // s[20] = 'w';
    throw ;
}

void b()
{
    a();
}

int main()
{
    // try{
    //     b();
    // }
    // catch(std::exception & e)
    // {
    //     std::cout << "message : " << e.what() << std::endl;
    // }
    std::string * str = NULL;
    delete str;
}
