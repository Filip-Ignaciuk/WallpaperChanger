#include <iostream>
#include <chrono>
#include <string>
#include <map>
#include <fstream>
#include <Windows.h>
#include <filesystem>

#include <shlobj.h>

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

std::wstring ConvertStrToWStr(const std::string& str)
{
    std::wstring wString;
    for (unsigned int i = 0; i < str.length(); i++)
    {
        wString.push_back(str[i]);
    }
    return wString;
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

std::wstring NormaliseDir(std::wstring& str)
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

std::string UnNormaliseDir(std::string& str)
{
    for (unsigned int i = 0; i < str.size(); i++)
    {
        if (str[i] == '/')
        {
            str[i] = '\\';
        }
    }
    return str;
}

std::wstring UnNormaliseDir(std::wstring& str)
{
    for (unsigned int i = 0; i < str.size(); i++)
    {
        if (str[i] == '/')
        {
            str[i] = '\\';
        }
    }
    return str;
}



const char* daysLong[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const char* daysShort[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };




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
        return daysLong[1];
    }
    else if (shortDate == daysShort[2])
    {
        return daysLong[2];
    }
    else if (shortDate == daysShort[3])
    {
        return daysLong[3];
    }
    else if (shortDate == daysShort[4])
    {
        return daysLong[4];
    }
    else if (shortDate == daysShort[5])
    {
        return daysLong[5];
    }
    else if (shortDate == daysShort[5])
    {
        return daysLong[6];
    }
}


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


    std::string imageDaysDirNormalised;


    std::vector<std::string> monImages;
    std::vector<std::wstring> monImagesDir;
    std::vector<std::string> tueImages;
    std::vector<std::wstring> tueImagesDir;
    std::vector<std::string> wedImages;
    std::vector<std::wstring> wedImagesDir;
    std::vector<std::string> thuImages;
    std::vector<std::wstring> thuImagesDir;
    std::vector<std::string> friImages;
    std::vector<std::wstring> friImagesDir;
    std::vector<std::string> satImages;
    std::vector<std::wstring> satImagesDir;
    std::vector<std::string> sunImages;
    std::vector<std::wstring> sunImagesDir;

    for (unsigned int i = 0; i < 7; i++)
    {
        imageDaysDirNormalised = imageDirNormalised + "/" + daysLong[i];
        for (const auto& entry : std::filesystem::directory_iterator(imageDaysDirNormalised))
        {
            
            std::wstring currentImageDirW = entry.path().c_str();
            std::string currentImageDir = ConvertWStrToStr(currentImageDirW);


            std::string currentEntry = currentImageDir;
            std::string nameOfImageWithDate;
            std::reverse(currentEntry.begin(), currentEntry.end());
            for (unsigned int i = 0; i < currentEntry.length(); i++)
            {
                if (currentEntry[i] == '/') {
                    break;
                }
                nameOfImageWithDate.push_back(currentEntry[i]);
            
            }
            std::reverse(nameOfImageWithDate.begin(), nameOfImageWithDate.end());

            if (nameOfImageWithDate[0] == 'M' && nameOfImageWithDate[1] == 'o' && nameOfImageWithDate[2] == 'n')
            {
                std::reverse(nameOfImageWithDate.begin(), nameOfImageWithDate.end());
                std::string nameOfImage;
                for (unsigned int i = 0; i < nameOfImageWithDate.length(); i++)
                {
                    if (currentEntry[i] == '\\') {
                        break;
                    }
                    nameOfImage.push_back(nameOfImageWithDate[i]);

                }

                std::reverse(nameOfImage.begin(), nameOfImage.end());
                monImages.emplace_back(nameOfImage);
                
                std::wstring tempWString = entry.path().c_str();
                tempWString = UnNormaliseDir(tempWString);
                monImagesDir.emplace_back(tempWString);

            }
            else if (nameOfImageWithDate[0] == 'T' && nameOfImageWithDate[1] == 'u' && nameOfImageWithDate[2] == 'e')
            {
                std::reverse(nameOfImageWithDate.begin(), nameOfImageWithDate.end());
                std::string nameOfImage;
                for (unsigned int i = 0; i < nameOfImageWithDate.length(); i++)
                {
                    if (currentEntry[i] == '\\') {
                        break;
                    }
                    nameOfImage.push_back(nameOfImageWithDate[i]);

                }

                std::reverse(nameOfImage.begin(), nameOfImage.end());
                tueImages.emplace_back(nameOfImage);

                std::wstring tempWString = entry.path().c_str();
                tempWString = UnNormaliseDir(tempWString);
                tueImagesDir.emplace_back(tempWString);
            }
            else if (nameOfImageWithDate[0] == 'W' && nameOfImageWithDate[1] == 'e' && nameOfImageWithDate[2] == 'd')
            {
                std::reverse(nameOfImageWithDate.begin(), nameOfImageWithDate.end());
                std::string nameOfImage;
                for (unsigned int i = 0; i < nameOfImageWithDate.length(); i++)
                {
                    if (currentEntry[i] == '\\') {
                        break;
                    }
                    nameOfImage.push_back(nameOfImageWithDate[i]);

                }

                std::reverse(nameOfImage.begin(), nameOfImage.end());
                wedImages.emplace_back(nameOfImage);

                std::wstring tempWString = entry.path().c_str();
                tempWString = UnNormaliseDir(tempWString);
                wedImagesDir.emplace_back(tempWString);
            }
            else if (nameOfImageWithDate[0] == 'T' && nameOfImageWithDate[1] == 'h' && nameOfImageWithDate[2] == 'u')
            {
                std::reverse(nameOfImageWithDate.begin(), nameOfImageWithDate.end());
                std::string nameOfImage;
                for (unsigned int i = 0; i < nameOfImageWithDate.length(); i++)
                {
                    if (currentEntry[i] == '\\') {
                        break;
                    }
                    nameOfImage.push_back(nameOfImageWithDate[i]);

                }

                std::reverse(nameOfImage.begin(), nameOfImage.end());
                thuImages.emplace_back(nameOfImage);

                std::wstring tempWString = entry.path().c_str();
                tempWString = UnNormaliseDir(tempWString);
                thuImagesDir.emplace_back(tempWString);
            }
            else if (nameOfImageWithDate[0] == 'F' && nameOfImageWithDate[1] == 'r' && nameOfImageWithDate[2] == 'i')
            {
                std::reverse(nameOfImageWithDate.begin(), nameOfImageWithDate.end());
                std::string nameOfImage;
                for (unsigned int i = 0; i < nameOfImageWithDate.length(); i++)
                {
                    if (currentEntry[i] == '\\') {
                        break;
                    }
                    nameOfImage.push_back(nameOfImageWithDate[i]);

                }

                std::reverse(nameOfImage.begin(), nameOfImage.end());
                friImages.emplace_back(nameOfImage);

                std::wstring tempWString = entry.path().c_str();
                tempWString = UnNormaliseDir(tempWString);
                friImagesDir.emplace_back(tempWString);
            }
            else if (nameOfImageWithDate[0] == 'S' && nameOfImageWithDate[1] == 'a' && nameOfImageWithDate[2] == 't')
            {
                std::reverse(nameOfImageWithDate.begin(), nameOfImageWithDate.end());
                std::string nameOfImage;
                for (unsigned int i = 0; i < nameOfImageWithDate.length(); i++)
                {
                    if (currentEntry[i] == '\\') {
                        break;
                    }
                    nameOfImage.push_back(nameOfImageWithDate[i]);

                }

                std::reverse(nameOfImage.begin(), nameOfImage.end());
                satImages.emplace_back(nameOfImage);

                std::wstring tempWString = entry.path().c_str();
                tempWString = UnNormaliseDir(tempWString);
                satImagesDir.emplace_back(tempWString);
            }
            else if (nameOfImageWithDate[0] == 'S' && nameOfImageWithDate[1] == 'u' && nameOfImageWithDate[2] == 'n')
            {
                std::reverse(nameOfImageWithDate.begin(), nameOfImageWithDate.end());
                std::string nameOfImage;
                for (unsigned int i = 0; i < nameOfImageWithDate.length(); i++)
                {
                    if (currentEntry[i] == '\\') {
                        break;
                    }
                    nameOfImage.push_back(nameOfImageWithDate[i]);

                }

                std::reverse(nameOfImage.begin(), nameOfImage.end());
                sunImages.emplace_back(nameOfImage);

                std::wstring tempWString = entry.path().c_str();
                tempWString = UnNormaliseDir(tempWString);
                sunImagesDir.emplace_back(tempWString);
            }


        }
            
    }

    bool isCorrect = false;

    if (currentDay == daysLong[0])
    {
        for (unsigned int i = 0; i < monImages.size(); i++)
        {
            if (monImages[i] == nameOfCurrentImage) 
            {
                isCorrect = true;
            }
        }
    }
    else if (currentDay == daysLong[1])
    {
        for (unsigned int i = 0; i < tueImages.size(); i++)
        {
            if (tueImages[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }
    }
    else if (currentDay == daysLong[2])
    {
        for (unsigned int i = 0; i < wedImages.size(); i++)
        {
            if (wedImages[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }
    }
    else if (currentDay == daysLong[3])
    {
        for (unsigned int i = 0; i < thuImages.size(); i++)
        {
            if (thuImages[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }
    }
    else if (currentDay == daysLong[4])
    {
        for (unsigned int i = 0; i < friImages.size(); i++)
        {
            if (friImages[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }
    }
    else if (currentDay == daysLong[5])
    {
        for (unsigned int i = 0; i < satImages.size(); i++)
        {
            if (satImages[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }
    }
    else if (currentDay == daysLong[6])
    {
        for (unsigned int i = 0; i < sunImages.size(); i++)
        {
            if (sunImages[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }
    }

    if (!isCorrect)
    {
        if (currentDay == daysLong[0])
        {
            int value = rand() % monImagesDir.size();
            auto image = monImagesDir[value].c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)image, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if(currentDay == daysLong[1])
        {
            int value = rand() % tueImagesDir.size();
            auto image = tueImagesDir[value].c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)image, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[2])
        {
            int value = rand() % wedImagesDir.size();
            auto image = wedImagesDir[value].c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)image, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[3])
        {
            int value = rand() % thuImagesDir.size();
            auto image = thuImagesDir[value].c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)image, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[4])
        {
            int value = rand() % friImagesDir.size();
            auto image = friImagesDir[value].c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)image, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[5])
        {
            int value = rand() % satImagesDir.size();
            auto image = satImagesDir[value].c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)image, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[6])
        {
            int value = rand() % sunImagesDir.size();
            auto image = sunImagesDir[value].c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)image, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        
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