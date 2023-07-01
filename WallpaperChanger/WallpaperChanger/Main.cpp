#include <iostream>
#include <chrono>
#include <string>
#include <direct.h>
#include <fstream>


#pragma warning(disable : 4996)

const char* days[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };


const std::string GetCurrentDay()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    const std::string fullDate = std::ctime(&t_c);
    return fullDate.substr(0, 3);
}


std::string GetCurrentDir()
{
    char buffer[FILENAME_MAX];
    (void)_getcwd(buffer, FILENAME_MAX);
    return buffer;
}

// Initialising files if they havent been created.
void initFiles()
{
    std::string currentDir = GetCurrentDir();

    for (unsigned int i = 0; i < currentDir.size(); i++)
    {
        if (currentDir[i] == '\\')
        {
            currentDir[i] = '/';
        }
    }

    std::string imageDir = currentDir + (std::string)"/Images";
    if (mkdir(imageDir.c_str()) == -1)
    {
        std::cout << "Error With initialising Image Directory" << std::endl;
        std::cin.get();
    }
    std::string buffer;
    buffer = imageDir.c_str();
    buffer = buffer + '/';
    std::cout << buffer << std::endl;

    for (unsigned int i = 0; i < 7; i++)
    {
        std::string dateDir = buffer + days[i];
        if (mkdir(dateDir.c_str()) == -1)
        {
            std::cout << "Error with initialising Day Directories" << std::endl;
            std::cin.get();
        }
    }
}

int main()
{
    std::ifstream file;
    file.open("Initialised.txt");
    if (!file.is_open())
    {
        std::cout << "Initialising..." << std::endl;
        file.close();
        std::ofstream file("Initialised.txt");
        file << "1";
        file.close();
        initFiles();
    }
    





    std::string date = GetCurrentDay();
    std::cout << date << std::endl;

    
    
    


    std::cin.get();
    
}