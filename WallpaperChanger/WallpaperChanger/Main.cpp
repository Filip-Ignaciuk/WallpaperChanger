#include <iostream>
#include <chrono>
#include <string>
#include <map>
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
    std::string shortDate = fullDate.substr(0, 3);
    if (shortDate == daysShort[0])
    {
        return daysLong[0];
    }
    else if (shortDate == daysShort[1])
    {
        return daysLong[0];
    }
    else if (shortDate == daysShort[2])
    {
        return daysLong[0];
    }
    else if (shortDate == daysShort[3])
    {
        return daysLong[0];
    }
    else if (shortDate == daysShort[4])
    {
        return daysLong[0];
    }
    else if (shortDate == daysShort[5])
    {
        return daysLong[0];
    }
    else if (shortDate == daysShort[5])
    {
        return daysLong[0];
    }
}




const char* daysLong[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const char* daysShort[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
static std::filesystem::path currentDirPath = std::filesystem::current_path();
static std::string currentDir = ConvertWStrToStr(currentDirPath.c_str());;
static std::string currentDirNormalised = NormaliseDir(currentDir);
std::string imageDirNormalised = currentDirNormalised + (std::string)"/Images";
const std::string currentDay = GetCurrentDay();




// Initialising files if they havent been created.
void initFiles()
{


    if (std::filesystem::create_directory(imageDirNormalised.c_str()) == 0)
    {
        std::cout << "Error With initialising Image Directory" << std::endl;
        std::cin.get();
    }


    for (unsigned int i = 0; i < 7; i++)
    {
        std::string dateDir = imageDirNormalised + "/" + daysLong[i];
        if (std::filesystem::create_directory(dateDir) == false)
        {
            std::cout << "Error with initialising Day Directories" << std::endl;
            std::cin.get();
        }
    }
}












int main()
{
    // Initialising 
    currentDirNormalised = NormaliseDir(currentDirNormalised);
    

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
    unsigned int j = 4;

    for (unsigned int i = 0; i < dirw.length(); i++)
    {
        if (dirw[i] == 'u' && dirw[i + 1] == 'n' && dirw[i + 2] == 'd' && dirw[i + 3] == '\\')
        {
            j = j + i;
        }
    }

    std::string dir = ConvertWStrToStr(dirw);


    std::string nameOfCurrentImage;
    for (j; j < dir.size(); j++)
    {
        nameOfCurrentImage.push_back(dir[j]);
    }

    std::cout << nameOfCurrentImage << std::endl;

    std::cout << imageDirNormalised << std::endl;

    std::string imageDaysDirNormalised;

    std::cout << currentDay << std::endl;

    std::map<std::string, std::string> monImages;
    std::map<std::string, std::string> tueImages;
    std::map<std::string, std::string> wedImages;
    std::map<std::string, std::string> thuImages;
    std::map<std::string, std::string> friImages;
    std::map<std::string, std::string> satImages;
    std::map<std::string, std::string> sunImages;

    for (unsigned int i = 0; i < 7; i++)
    {
        imageDaysDirNormalised = imageDirNormalised + "/" + daysLong[i];
        for (const auto& entry : std::filesystem::directory_iterator(imageDaysDirNormalised))
        {
            
            std::cout << entry.path() << std::endl;
            std::wstring currentImageDirW = entry.path().c_str();
            std::string currentImageDir = ConvertWStrToStr(currentImageDirW);
            std::cout << currentImageDir << std::endl;


            std::string currentEntry = currentImageDir;
            std::string nameOfImage;
            std::reverse(currentEntry.begin(), currentEntry.end());
            std::cout << "Reversed:" << std::endl;
            std::cout << currentEntry << std::endl;
            for (unsigned int i = 0; i < currentEntry.length(); i++)
            {
                if (currentEntry[i] == '/') {
                    break;
                }
                nameOfImage.push_back(currentEntry[i]);
            
            }
            std::reverse(nameOfImage.begin(), nameOfImage.end());
            std::cout << nameOfImage << std::endl;

            if (nameOfImage[0] == 'M' && nameOfImage[1] == 'o' && nameOfImage[2] == 'n')
            {
                monImages.emplace(nameOfImage, daysLong[0]);
            }
            else if (nameOfImage[0] == 'T' && nameOfImage[1] == 'u' && nameOfImage[2] == 'e')
            {
                tueImages.emplace(nameOfImage, daysLong[1]);
            }
            else if (nameOfImage[0] == 'W' && nameOfImage[1] == 'e' && nameOfImage[2] == 'd')
            {
                wedImages.emplace(nameOfImage, daysLong[2]);
            }
            else if (nameOfImage[0] == 'T' && nameOfImage[1] == 'h' && nameOfImage[2] == 'u')
            {
                thuImages.emplace(nameOfImage, daysLong[3]);
            }
            else if (nameOfImage[0] == 'F' && nameOfImage[1] == 'r' && nameOfImage[2] == 'i')
            {
                friImages.emplace(nameOfImage, daysLong[4]);
            }
            else if (nameOfImage[0] == 'S' && nameOfImage[1] == 'a' && nameOfImage[2] == 't')
            {
                satImages.emplace(nameOfImage, daysLong[5]);
            }
            else if (nameOfImage[0] == 'S' && nameOfImage[1] == 'u' && nameOfImage[2] == 'n')
            {
                sunImages.emplace(nameOfImage, daysLong[6]);
            }


        }
            
    }

    if (currentDay == daysLong[0])
    {

    }

    

    std::cin.get();
    
}


//if (currentDay[0] == 'M' && daysString[0] == (const char*)'M' && currentDay[0] == 'o' && daysString[0] == (const char*)'o' && currentDay[0] == 'n' && daysString[0] == (const char*)'n')
//{
//    imageDaysDirNormalised = imageDirNormalised + "/Monday";
//}
//else if (currentDay[0] == 'T' && daysString[0] == (const char*)'T' && currentDay[0] == 'u' && daysString[0] == (const char*)'u' && currentDay[0] == 'e' && daysString[0] == (const char*)'e')
//{
//    imageDaysDirNormalised = imageDirNormalised + "/Tuesday";
//}
//else if (currentDay[0] == 'W' && daysString[0] == (const char*)'W' && currentDay[0] == 'e' && daysString[0] == (const char*)'e' && currentDay[0] == 'd' && daysString[0] == (const char*)'d')
//{
//    imageDaysDirNormalised = imageDirNormalised + "/Wednesday";
//}
//else if (currentDay[0] == 'T' && daysString[0] == (const char*)'T' && currentDay[0] == 'h' && daysString[0] == (const char*)'h' && currentDay[0] == 'u' && daysString[0] == (const char*)'u')
//{
//    imageDaysDirNormalised = imageDirNormalised + "/Thursday";
//}
//else if (currentDay[0] == 'F' && daysString[0] == (const char*)'F' && currentDay[0] == 'r' && daysString[0] == (const char*)'r' && currentDay[0] == 'i' && daysString[0] == (const char*)'i')
//{
//    imageDaysDirNormalised = imageDirNormalised + "/Friday";
//}
//else if (currentDay[0] == 'S' && daysString[0] == (const char*)'S' && currentDay[0] == 'a' && daysString[0] == (const char*)'a' && currentDay[0] == 't' && daysString[0] == (const char*)'t')
//{
//    imageDaysDirNormalised = imageDirNormalised + "/Saturday";
//}
//else if (currentDay[0] == 'S' && daysString[0] == (const char*)'S' && currentDay[0] == 'u' && daysString[0] == (const char*)'u' && currentDay[0] == 'n' && daysString[0] == (const char*)'n')
//{
//    imageDaysDirNormalised = imageDirNormalised + "/Sunday";
//}




// Code for obtaining name and extension of images.
//std::wstring currentEntryW = entry.path().c_str();
//std::string currentEntry = ConvertWStrToStr(currentEntryW);
//currentEntry = NormaliseDir(currentEntry);
//std::string nameOfImage;
//std::reverse(currentEntry.begin(), currentEntry.end());
//std::cout << "Reversed:" << std::endl;
//std::cout << currentEntry << std::endl;
//for (unsigned int i = 0; i < currentEntry.length(); i++)
//{
//    if (currentEntry[i] == '/') {
//        break;
//    }
//    std::cout << currentEntry[i] << std::endl;
//    nameOfImage.push_back(currentEntry[i]);
//
//}
//std::reverse(nameOfImage.begin(), nameOfImage.end());
//std::cout << nameOfImage << std::endl;