#include <iostream>
#include <chrono>
#include <string>
#include <direct.h>
#include <fstream>
#include <Windows.h>


#pragma warning(disable : 4996)

const char* days[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

// Gives you the current day
const std::string GetCurrentDay()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    const std::string fullDate = std::ctime(&t_c);
    return fullDate.substr(0, 3);
}

// Obtain the current directory the exe is located in.
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
    // Checking/Initialising files
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


    
    // Accessing registry so that we know what the current wallpaper is.
    //WallPaper
    HKEY hKey;
    LONG lRes = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey);
    
    // Just to convert std::string to std::wstring.
    std::string RegName = "WallPaper";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &RegName[0], (int)RegName.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &RegName[0], (int)RegName.size(), &wstrTo[0], size_needed);


    WCHAR buffer[512];
    DWORD sizeOfBuffer = sizeof(buffer);
    ULONG error = RegQueryValueExW(hKey, wstrTo.c_str(), 0, NULL, (LPBYTE)buffer, &sizeOfBuffer);


    std::wstring dir;
    if (error == ERROR_SUCCESS)
    {
        dir = buffer;
        std::cout << dir.c_str() << std::endl;
    }
    else
    {
        std::cout << "Failed retrieving the current wallpaper." << std::endl;
    }
    //C:\Users\Ignac\AppData\Local\Packages\Microsoft.Windows.Photos_8wekyb3d8bbwe\LocalState\PhotosAppBackground\Portal Wallpaper Dual Monitors 20 Amazing Dual Screen Moni.jpg
    // Location of the start of the image name.
    int j = 4;
    for (unsigned int i = 0; i < dir.length(); i++)
    {
        if (dir[i] == 'u' && dir[i + 1] == 'n' && dir[i + 2] == 'd' && dir[i + 3] == '\\')
        {
            j = j + i;
        }
    }

    // Gives current image name with extension.
    std::string nameOfCurrentImage;

    for (unsigned int i = j; i < dir.length(); i++)
    {
        nameOfCurrentImage[i] = dir[i];
    }




    std::cin.get();
    
}





