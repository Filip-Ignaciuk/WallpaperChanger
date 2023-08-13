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

    // Gives week day starting at sunday as 0.
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
void WallpaperChanger::InitImages(const int _repairType)
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
            std::string line;
            // std::getline doesn't like being called multiple times on one file. We need to create a "master" line for all to view.
            std::getline(textImageFileIF, line);
            bool isOpen = textImageFileIF.is_open();
            bool isValid = CheckIfImageTextFilesIsValid(line);
            // We need to check if its open as we are dependent on that later on within the code.
            if (isOpen && !isValid)
            {
                if(_repairType)
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
                    textImageFileOF.close();
                }
                else
                {
                    const std::string equalSign = "=";
                    std::string newLine;
                    std::string currentConfig;
                    textImageFileIF.close();
            
            
                    for (size_t i = 0; i < numOfConfigs; i++)
                    {
                        currentConfig = configurations[i];
                        if(line.find(currentConfig) == std::string::npos)
                        {
                            
                            newLine.append(currentConfig + equalSign);
                        }
                        else
                        {
                            
                            int pos = line.find(currentConfig) + currentConfig.size() + 1;
                            std::string currentConfigData;
            
                            while (isdigit(line[pos]))
                            {
                                currentConfigData.insert(currentConfigData.size(), 1, line[pos]);
                                pos++;
                            }
                            newLine.append(currentConfig + equalSign + currentConfigData);
            
                        }
                        
                    }
            
                    std::ofstream textImageFileOF;
                    textImageFileOF.open(textImageFileDir, std::ofstream::trunc);
                    textImageFileOF << newLine;
                    textImageFileOF.close();
            
                }
            }
            else if (isValid)
            {
                continue;
            }
            else
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
                textImageFileOF.close();
            }
        }
    }
}

bool WallpaperChanger::CheckIfImageTextFilesIsValid(std::ifstream& _file)
{
    std::string config;
    std::getline(_file, config);
    // Checking if each line is concordant with the configurations array.
    for (size_t i = 0; i < numOfConfigs; i++)
    {
        if (config.find(configurations[i]) == std::string::npos)
        {
            return false;
        }
    }
    return true;
}

bool WallpaperChanger::CheckIfImageTextFilesIsValid(std::string& _line)
{
    // Checking if each line is concordant with the configurations array.
    for (size_t i = 0; i < numOfConfigs; i++)
    {
        if (_line.find(configurations[i]) == std::string::npos)
        {
            return false;
        }
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


bool WallpaperChanger::IsDigit(const char* _num)
{
	if (_num == "0" || _num == "1" || _num == "2" || _num == "3" || _num == "4" || _num == "5" || _num == "6" || _num == "7" || _num == "8" || _num == "9")
	{
        return true;
	}
    return false;
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

    
    InitImages(1);


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


void WallpaperChanger::SetImageConfiguration(const std::string& _fileName, const int _dayOfImage, const std::string& _config, const std::string& _data)
{
    std::string documentDirCopy = documentDir;
    std::string fileDir = NormaliseDir(documentDirCopy) + "/WallpaperChanger/Images/" + daysLong[_dayOfImage] + "/" + _fileName;
    std::ifstream fileR(fileDir);
    
    std::string line;
    int pos;
    if(fileR.is_open())
    {
        std::getline(fileR, line);
        // Get position of data equal to the config.
        pos = line.find(_config) + _config.size() + 1;
        fileR.close();
        line.insert(pos, _data);
        std::ofstream fileW(fileDir, std::ofstream::trunc);
        if(fileW.is_open())
        {
            fileW << line;

            fileW.close();
        }
        else
        {
            std::cout << "Failed writing new data into image config file." << std::endl;
        }
        
    }
    else
    {
        std::cout << "Reading image config file failed." << std::endl;
    }
    

    

    
}

void WallpaperChanger::GetImageConfiguration(const std::string& _fileName,const int _dayOfImage, int _configs[3])
{
    std::string documentDirCopy = documentDir;
    std::string fileDir = NormaliseDir(documentDirCopy) + "/WallpaperChanger/Images/" + daysLong[_dayOfImage] + "/" + _fileName;
    std::ifstream fileR(fileDir);
    std::string line;
    if(fileR.is_open())
    {
        std::getline(fileR, line);
        for (size_t i = 0; i < numOfConfigs; i++)
        {
            std::string config = configurations[i];
            std::string currentConfigData;
        	int pos = line.find(config) + config.size() + 1;
            while(isdigit(line[pos]))
            {
                currentConfigData.insert(currentConfigData.size(), 1, line[pos]);
                pos++;
            }
            int num = 0;

            if (!currentConfigData.empty())
            {
            	num = std::stoi(currentConfigData);
            }
            _configs[i] = num;
        }
    }
    else
    {
        std::cout << "Writing to image config file failed." << std::endl;
    }
}



int main()
{
    int configs[WallpaperChanger::numOfConfigs] = {};
    WallpaperChanger::StartWallpaperChanger();
    WallpaperChanger::SetImageConfiguration("Example.txt", 1, "TimeLimit", "0");
    WallpaperChanger::GetImageConfiguration("Example.txt", 1, configs);

}


