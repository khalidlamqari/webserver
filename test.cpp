#include <fstream>
#include <iostream>

int main() {
    std::ofstream file_content;

    // Attempt to open a file


    // Check if the file was successfully opened
    if (file_content.is_open()) {
        std::cout << "File successfully opened for writing." << std::endl;
    } else {
        std::cout << "Failed to open file for writing." << std::endl;
    }

    // Always good practice to close the file
    file_content.close();

    return 0;
}
