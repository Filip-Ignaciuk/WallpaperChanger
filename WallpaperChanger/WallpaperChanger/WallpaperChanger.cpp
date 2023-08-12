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
std::string WallpaperChanger::ConvertWStrToStr(const std::wstring& _wStr)
{
	const std::u16string uStr(_wStr.begin(), _wStr.end());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(uStr);
}

// Conversion of strings to wStrings.
std::wstring WallpaperChanger::ConvertStrToWStr(const std::string& _str)
{

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::u16string dest = convert.from_bytes(_str);
    std::wstring final(dest.begin(), dest.end());
    return final;
}

// Converting all the \\ slashes into one singular forward slash.
std::string WallpaperChanger::NormaliseDir(std::string& _str)
{
    for (unsigned int i = 0; i < _str.size(); i++)
    {
        if (_str[i] == '\\')
        {
            _str[i] = '/';
        }
    }
    return _str;
}

int WallpaperChanger::GetCurrentWeekDay()
{
    time_t rawTime;
    time(&rawTime);
    const tm* timeInfo = localtime(&rawTime);

    const int wDay = timeInfo->tm_wday;

    return wDay;

}

std::string WallpaperChanger::GetDocumentDir()
{
    std::string docPath;
    TCHAR path[MAX_PATH];
    if (!FAILED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path))) 
    {
        docPath = ConvertWStrToStr(path);
        return docPath;
    }

    std::cout << "Error with obtaining document directory." << std::endl;
    std::cin.get();
    
    return docPath;
}

// Initialising files if they haven't been created.
void WallpaperChanger::initFolders()
{

    if (std::filesystem::create_directory(documentDir + "/WallpaperChanger") == 0)
    {
        std::cout << "Error With initialising WallpaperChanger Directory" << std::endl;
        std::cin.get();

    }

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
void WallpaperChanger::InitImages()
{
    std::string imageDateDir;
    for (unsigned int i = 0; i < 7; i++)
    {
        imageDateDir = imageDir + "/" + daysLong[i];

        std::vector<std::string> images;
        GetImages(imageDateDir, images);

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

            if (!textImageFileIF.is_open() || !CheckIfImageTextFilesIsValid(textImageFileIF))
            {
                textImageFileIF.close();

                std::ofstream textImageFileOF;
                textImageFileOF.open(textImageFileDir, std::ofstream::out | std::ofstream::trunc);

                for (size_t i = 0; i < numOfConfigs; i++)
                {
                    std::string input = configurations[i];
                    input.append("=");
                    textImageFileOF << input;
                }
                textImageFileIF.close();
            }
        }
    }
}

bool WallpaperChanger::CheckIfImageTextFilesIsValid(std::ifstream& _file)
{
    int configIncrement = 0;
    std::string config;
    // Checking if each line is concordant with the configurations array.
	while(std::getline(_file, config))
	{
        if(config.find(configurations[configIncrement]) == std::string::npos)
        {
            return false;
        }
        configIncrement++;
	}
    return true;
}


void WallpaperChanger::GetImages(const std::string& _dir, std::vector<std::string>& _vector)
{
    for (const auto& entry : std::filesystem::directory_iterator(_dir))
    {
        std::string currentItemDir = ConvertWStrToStr(entry.path().c_str());
        if (currentItemDir.find(".jpg") != std::string::npos || currentItemDir.find(".png") != std::string::npos)
        {
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
        

    }


    
}


void WallpaperChanger::SetWallpaper(const std::vector<std::string>& _images)
{
    int value = rand() % _images.size();
    std::wstring completeDir = ConvertStrToWStr(currentDateDir + "/" + _images[value]);

    auto imageSpecificDir = completeDir.c_str();
    if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
    {
        std::cout << "Error with setting wallpaper." << std::endl;
        std::cin.get();
    }
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
        initFolders();
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

    
    InitImages();


    // Accessing registry so that we know what the current wallpaper is.
    HKEY hKey;
    LONG lRes = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey);


    std::wstring regName = ConvertStrToWStr("WallPaper");
    WCHAR buffer[512];
    DWORD sizeOfBuffer = sizeof(buffer);
    std::wstring dirW;

    if (RegQueryValueExW(hKey, regName.c_str(), nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer), &sizeOfBuffer) == ERROR_SUCCESS)
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

    std::vector<std::string> imagesOfCurrentDay;
    bool isCorrect = false;

    GetImages(currentDateDir, imagesOfCurrentDay);

    

    for (unsigned int i = 0; i < imagesOfCurrentDay.size(); i++)
    {
        if (imagesOfCurrentDay[i] == nameOfCurrentImage)
        {
            isCorrect = true;
        }
    }

    if (isCorrect)
    {
        std::cout << "Current Wallpaper is valid." << std::endl;
    }
    else if (!imagesOfCurrentDay.empty())
    {
        SetWallpaper(imagesOfCurrentDay);
    }
    else
    {
        std::cout << "No images within the images folder of the current day." << std::endl;
        std::cout << "Go to your images folder located within your documents folder. Then place images to the dates you would like." << std::endl;
        std::cin.get();
    }

}

void WallpaperChanger::SetImageConfiguration(const std::string& _fileName)
{
    std::ifstream file(_fileName);
}

int main()
{
    WallpaperChanger::StartWallpaperChanger();

}


