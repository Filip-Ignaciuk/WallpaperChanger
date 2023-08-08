#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <Windows.h>
#include <filesystem>
#include <codecvt>
#include <ShlObj.h>
#include "WallpaperChanger.hpp"


#pragma warning(disable : 4996)
std::string WallpaperChanger::ConvertWStrToStr(const std::wstring& wStr)
{
    std::u16string uStr(wStr.begin(), wStr.end());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(uStr);
}

// Conversion of strings to wStrings.
const std::wstring WallpaperChanger::ConvertStrToWStr(const std::string& str)
{

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::u16string dest = convert.from_bytes(str);
    std::wstring final(dest.begin(), dest.end());
    return final;
}

// Converting all the \\ slashes into one singular forward slash.
const std::string WallpaperChanger::NormaliseDir(std::string& str)
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

const int WallpaperChanger::GetCurrentWeekDay()
{
    time_t rawtime;
    tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    int wday = timeinfo->tm_wday;

    return wday;

}

std::string WallpaperChanger::GetDocumentDir()
{
    TCHAR path[MAX_PATH];
    if (!FAILED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path))) {
        return ConvertWStrToStr(path);
    }

    std::cout << "Error with obtaining document directory." << std::endl;
    std::cin.get();
    return "";
}

const char* WallpaperChanger::daysLong[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const std::string WallpaperChanger::documentDir = GetDocumentDir();
// In this case we can add / at the end of the directory as in all references of the variable it needs it.
std::string WallpaperChanger::documentDirCopy = documentDir;
const std::string WallpaperChanger::imageDir = NormaliseDir(documentDirCopy) + "/WallpaperChanger/Images/";
const int WallpaperChanger::currentDay = GetCurrentWeekDay();


// Initialising files if they havent been created.
void WallpaperChanger::initFiles()
{
    std::string documentDirCopy = documentDir;
    std::string imageDir = NormaliseDir(documentDirCopy) + "/WallpaperChanger";

    if (std::filesystem::create_directory(imageDir) == 0)
    {
        std::cout << "Error With initialising WallpaperChanger Directory" << std::endl;
        std::cin.get();

    }

    imageDir = imageDir + "/Images";

    if (std::filesystem::create_directory(imageDir) == 0)
    {
        std::cout << "Error With initialising Image Directory" << std::endl;
        std::cin.get();

    }


    for (unsigned int i = 0; i < 7; i++)
    {
        std::string dateDir = imageDir + "/" + daysLong[i];
        if (std::filesystem::create_directory(dateDir) == 0)
        {
            std::cout << "Error with initialising Day Directories" << std::endl;
            std::cin.get();
        }
    }
}

// Add text files to each image to customise how it is displayed.
void WallpaperChanger::initImages()
{
    std::string imageDateDir;
    for (unsigned int i = 0; i < 7; i++)
    {
        imageDateDir = imageDir + daysLong[i];

        for (const auto& entry : std::filesystem::directory_iterator(imageDateDir))
        {
            std::vector<std::string> images;
            GetImages(entry, images);

            for (const std::string& image : images)
            {
                std::string textImageFileDir = imageDateDir + "/" + image;
                for (size_t i = 0; i < 4; i++)
                {
                    textImageFileDir.pop_back();
                }
                textImageFileDir += ".txt";
                std::ifstream textImageFileIF;
                textImageFileIF.open(textImageFileDir);
                if (!textImageFileIF.is_open())
                {
                    textImageFileIF.close();
                    std::ofstream textImageFileOF(textImageFileDir);
                    // Add stuff
                    textImageFileOF.close();
                }
            }
        }
    }
}


void WallpaperChanger::GetImages(std::filesystem::directory_entry _entry, std::vector<std::string>& _vector)
{
    std::string currentItemDir = ConvertWStrToStr(_entry.path().c_str());

    std::string nameOfItemWithDate;
    std::reverse(currentItemDir.begin(), currentItemDir.end());
    for (unsigned int i = 0; i < currentItemDir.length(); i++)
    {
        if (currentItemDir[i] == '/') {
            break;
        }
        nameOfItemWithDate.push_back(currentItemDir[i]);

    }

    std::string nameOfImage;
    for (unsigned int i = 0; i < nameOfItemWithDate.length(); i++)
    {
        if (currentItemDir[i] == '\\') {
            break;
        }
        nameOfImage.push_back(nameOfItemWithDate[i]);

    }

    std::reverse(nameOfImage.begin(), nameOfImage.end());
    _vector.emplace_back(nameOfImage);
}



// To do
void WallpaperChanger::SetColorAccent()
{

}


void WallpaperChanger::StartWallpaperChanger()
{
    // Checking/Initialising files
    std::ifstream file;
    file.open(documentDir + "/WallpaperChanger/Initialised.txt");
    if (!file.is_open())
    {
        initFiles();
        std::cout << "Initialising..." << std::endl;
        std::cout << "Restart application after adding images to the generated subfolders." << std::endl;
        std::cout << "The subfolders are located within your documents folder." << std::endl;
        std::cin.get();
        file.close();
        std::ofstream file(documentDir + "/WallpaperChanger/Initialised.txt");
        file << "1";
        file.close();
        return;
    }



    // Accessing registry so that we know what the current wallpaper is.
    HKEY hKey;
    LONG lRes = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey);


    std::wstring RegName = ConvertStrToWStr("WallPaper");
    WCHAR buffer[512];
    DWORD sizeOfBuffer = sizeof(buffer);
    std::wstring dirW;

    if (RegQueryValueExW(hKey, RegName.c_str(), 0, NULL, (LPBYTE)buffer, &sizeOfBuffer) == ERROR_SUCCESS)
    {
        dirW = buffer;
    }
    else
    {
        std::cout << "Failed retrieving the current wallpaper." << std::endl;
        std::cin.get();
    }

    // Location of the start of the image name.
    unsigned int j = 4;

    for (unsigned int i = 0; i < dirW.length(); i++)
    {
        if (dirW[i] == 'u' && dirW[i + 1] == 'n' && dirW[i + 2] == 'd' && dirW[i + 3] == '\\')
        {
            j = j + i;
        }
    }

    std::string dirOfCurrentImage = ConvertWStrToStr(dirW);
    std::string nameOfCurrentImage;


    for (j; j < dirOfCurrentImage.size(); j++)
    {
        nameOfCurrentImage.push_back(dirOfCurrentImage[j]);
    }


    std::string imageDirDay;
    std::wstring dateDirW;


    std::vector<std::string> images;
    std::wstring imageDirW = ConvertStrToWStr(imageDir);

    bool isCorrect = false;

    if (currentDay == 1)
    {
        imageDirDay = imageDir + daysLong[0];
        dateDirW = ConvertStrToWStr("Monday/");

    }
    else if (currentDay == 2)
    {
        imageDirDay = imageDir + daysLong[1];
        dateDirW = ConvertStrToWStr("Tuesday/");

    }
    else if (currentDay == 3)
    {
        imageDirDay = imageDir + daysLong[2];
        dateDirW = ConvertStrToWStr("Wednesday/");
    }
    else if (currentDay == 4)
    {
        imageDirDay = imageDir + daysLong[3];
        dateDirW = ConvertStrToWStr("Thursday/");
    }
    else if (currentDay == 5)
    {
        imageDirDay = imageDir + daysLong[4];
        dateDirW = ConvertStrToWStr("Friday/");
    }
    else if (currentDay == 6)
    {
        imageDirDay = imageDir + daysLong[5];
        dateDirW = ConvertStrToWStr("Saturday/");
    }
    else if (currentDay == 7)
    {
        imageDirDay = imageDir + daysLong[6];
        dateDirW = ConvertStrToWStr("Sunday/");
    }

    for (const auto& entry : std::filesystem::directory_iterator(imageDirDay))
    {
        GetImages(entry, images);
    }

    initImages();

    for (unsigned int i = 0; i < images.size(); i++)
    {
        if (images[i] == nameOfCurrentImage)
        {
            isCorrect = true;
        }
    }

    if (isCorrect)
    {
        std::cout << "Current Wallpaper is valid." << std::endl;
    }
    else if (!images.empty())
    {
        int value = rand() % images.size();
        std::wstring completeDir = imageDirW + dateDirW + ConvertStrToWStr(images[value]);

        auto imageSpecificDir = completeDir.c_str();
        if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
        {
            std::cout << "Error with setting wallpaper." << std::endl;
            std::cin.get();
        }
    }
    else
    {
        std::cout << "No images within the images folder of the current day." << std::endl;
        std::cout << "Go to your images folder located within your documents folder. Then place images to the dates you would like." << std::endl;
        std::cin.get();
    }
    return;

}


int main()
{
    WallpaperChanger::StartWallpaperChanger();
}
