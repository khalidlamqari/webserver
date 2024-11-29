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

int main()
{
    std::cout << is_existe_file("z_site/img.png") << std::endl;
}