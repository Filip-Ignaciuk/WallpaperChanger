#include <iostream>
#include <chrono>
#include <string>

#include <fstream>
#include <Windows.h>
#include <filesystem>

#pragma warning(disable : 4996)


// Conversion of Wstrings to strings
std::string ConvertWStrToStr(const std::wstring& wStr)
{
    std::string normString;
    for (unsigned int i = 0; i < wStr.size(); i++)
    {
        normString.push_back(wStr[i]);
    }
    return normString;

    // Offical way
    //std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    //std::string dest = convert.to_bytes(source);
}

// Converting all the \\ shlashes into one singular forward slash.
std::string NormaliseDir(std::string& str)
{
    for (unsigned int i = 0; i < str.size(); i++)
    {
        if (str[i] == '\\')
        {
            str[i] = '/';
        }
    }
    return str;
}


// Gives you the current day
const std::string GetCurrentDay()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    const std::string fullDate = std::ctime(&t_c);
    return fullDate.substr(0, 3);
}




const char* days[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
static std::filesystem::path currentDirPath = std::filesystem::current_path();
static std::string* currentDir = &(ConvertWStrToStr(currentDirPath.c_str()));;
static std::string* currentDirNormalised = &(*currentDir);
std::string imageDir = (*currentDirNormalised) + (std::string)"/Images";




// Initialising files if they havent been created.
void initFiles()
{


    if (std::filesystem::create_directory(imageDir.c_str()) == 0)
    {
        std::cout << "Error With initialising Image Directory" << std::endl;
        std::cin.get();
    }

    std::string buffer;
    buffer = imageDir.c_str();
    buffer = buffer + '/';
    std::cout << buffer << std::endl;

    //for (unsigned int i = 0; i < 7; i++)
    //{
    //    std::string dateDir = buffer + days[i];
    //    if (mkdir(dateDir.c_str()) == -1)
    //    {
    //        std::cout << "Error with initialising Day Directories" << std::endl;
    //        std::cin.get();
    //    }
    //}
}









int main()
{
    // Initialising 
    *currentDirNormalised = NormaliseDir(*currentDirNormalised);
    

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


    std::wstring dirw;
    if (error == ERROR_SUCCESS)
    {
        dirw = buffer;
    }
    else
    {
        std::cout << "Failed retrieving the current wallpaper." << std::endl;
    }
    //C:\Users\Ignac\AppData\Local\Packages\Microsoft.Windows.Photos_8wekyb3d8bbwe\LocalState\PhotosAppBackground\Portal Wallpaper Dual Monitors 20 Amazing Dual Screen Moni.jpg
    // Location of the start of the image name.
    int j = 4;
    for (unsigned int i = 0; i < dirw.length(); i++)
    {
        if (dirw[i] == 'u' && dirw[i + 1] == 'n' && dirw[i + 2] == 'd' && dirw[i + 3] == '\\')
        {
            j = j + i;
        }
    }

    std::string dir = ConvertWStrToStr(dirw);


    std::string nameOfCurrentImage;
    for (unsigned j; j < dir.size(); j++)
    {
        nameOfCurrentImage.push_back(dir[j]);
    }

    std::cout << nameOfCurrentImage << std::endl;

    for (unsigned int i = 0; i < 7; i++)
    {
        for (const auto& entry : std::filesystem::directory_iterator(imageDir + (std::string)"/Images"))
        {
            std::cout << entry.path() << std::endl;
        }
            
    }

    

    std::cin.get();
    
}





