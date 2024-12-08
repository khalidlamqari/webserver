#include <fstream>
#include <iostream>
#include <signal.h>

#include <sstream>
#include<string>
#include <unistd.h>

static size_t file_num = 0;

std::string get_rand_file_name(size_t & file_num)
{
    std::ostringstream file_name;
    std::string path = "/tmp/.web_server_response_in_file_";

    file_name << path;
    file_name << file_num;

    if (access(file_name.str().c_str(), F_OK) == 0)
    {
        file_num += 1;
        return get_rand_file_name(file_num);
    }
    else
    {
        return file_name.str();
    }
}


int main() {
    // std::string str = "1234567890";
    // std::string sub = str.substr(0, 2);
    // std::cout << sub << std::endl;
    std::ofstream myfile;
    std::cout <<  get_rand_file_name(file_num) << std::endl;

    myfile.open(get_rand_file_name(file_num));

    if ( myfile.is_open())
        std::cout << "opened" << std::endl;
    else
        std::cout << "not opened" << std::endl;
    // std::cout << "file_num : " << file_num << std::endl;

    // myfile << "hello world" << std::endl;
    // for ( int i = 21375 ; i <= 21379; i++)
    // {
    //     kill(i, SIGKILL);
    // }
    // std::string a = "hello";
    // std::string * b = new std::string ("hello");
    
    // if ( a == *b)
    //     std::cout << "true " << std::endl;
    // else
    //     std::cout << "false " << std::endl;
    // std::ofstream file_content;

    // // Attempt to open a file


    // // Check if the file was successfully opened
    // if (file_content.is_open()) {
    //     std::cout << "File successfully opened for writing." << std::endl;
    // } else {
    //     std::cout << "Failed to open file for writing." << std::endl;
    // }

    // // Always good practice to close the file
    // file_content.close();

    // return 0;


    
}
