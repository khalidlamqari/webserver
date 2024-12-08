
#include <ctime>
#include <string>
#include <iostream>




int main()
{

    std::time_t currentTime = std::time(0);
    std::cout << currentTime << std::endl;
    
}