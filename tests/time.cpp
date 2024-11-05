#include <iostream>
#include <time.h>
#include <sys/stat.h>

int main()
{
    std::string s("hello world");
    std::cout << s << std::endl;
    // struct stat s;
    // struct tm * time ;
    // stat("../utils", &s);

    // std::cout << s.st_mtime << std::endl;

    // time = localtime(&s.st_mtime);

    // std::cout << time->tm_sec << std::endl;
    // std::cout << time->tm_min << std::endl;
    // std::cout << time->tm_hour << std::endl;
    // std::cout << time->tm_year + 1900 << std::endl;

    // char t[100] ;

    // strftime(t, sizeof(t), "%Y-%m-%d %H:%M:%S", time) ;

    // std::cout << t<< std::endl;
}
