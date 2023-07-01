#include <iostream>
#include <chrono>
#include <string>
#pragma warning(disable : 4996)


int main()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    const std::string fullDate = std::ctime(&t_c);
    const auto dayDate = fullDate.substr(0, 3);
    std::cout << dayDate << std::endl;
    


}