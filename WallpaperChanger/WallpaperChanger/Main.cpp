#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <Windows.h>
#include <filesystem>
#include <codecvt>
#include <ShlObj.h>

#pragma warning(disable : 4996)

// Conversion of wStrings to strings
std::string ConvertWStrToStr(const std::wstring& wStr)
{
    std::u16string uStr(wStr.begin(), wStr.end());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(uStr);
}

// Conversion of strings to wStrings.
const std::wstring ConvertStrToWStr(const std::string& str)
{

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::u16string dest = convert.from_bytes(str);
    std::wstring final(dest.begin(), dest.end());
    return final;
}

// Converting all the \\ shlashes into one singular forward slash.
const std::string NormaliseDir(std::string& str)
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

const std::wstring NormaliseDir(std::wstring& str)
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

const std::string UnNormaliseDir(std::string& str)
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
    else if (shortDate == daysShort[6])
    {
        return daysLong[6];
    }
    else
    {
        return "";
    }
}

std::string GetDocumentDir()
{
    TCHAR path[MAX_PATH];
    if (FAILED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path))) {
        std::cout << "Error with obtaining document directory." << std::endl;
        return "";
    }
    else
    {
        return ConvertWStrToStr(path);
    }
}

const std::string documentDir = GetDocumentDir();
// In this case we can add / at the end of the directory as in all references of the variable it needs it.
std::string documentDirCopy = documentDir;
const std::string imageDir = NormaliseDir(documentDirCopy) + "/WallpaperChanger/Images/"; 
const std::string currentDay = GetCurrentDay();






// Initialising files if they havent been created.
void initFiles()
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







int main()
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
        file.close();
        std::ofstream file(documentDir + "/WallpaperChanger/Initialised.txt");
        file << "1";
        file.close();
        return 0;

    }


    
    // Accessing registry so that we know what the current wallpaper is.
    HKEY hKey;
    LONG lRes = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey);


    std::wstring RegName = ConvertStrToWStr("WallPaper");
    WCHAR buffer[512];
    DWORD sizeOfBuffer = sizeof(buffer);
    ULONG error = RegQueryValueExW(hKey, RegName.c_str(), 0, NULL, (LPBYTE)buffer, &sizeOfBuffer);


    std::wstring dirw;
    if (error == ERROR_SUCCESS)
    {
        dirw = buffer;
    }
    else
    {
        std::cout << "Failed retrieving the current wallpaper." << std::endl;
    }

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


    std::string imageDirDay;


    std::vector<std::string> monImages;
    std::vector<std::string> tueImages;
    std::vector<std::string> wedImages;
    std::vector<std::string> thuImages;
    std::vector<std::string> friImages;
    std::vector<std::string> satImages;
    std::vector<std::string> sunImages;

    for (unsigned int i = 0; i < 7; i++)
    {
        imageDirDay = imageDir + daysLong[i];
        for (const auto& entry : std::filesystem::directory_iterator(imageDirDay))
        {

            std::string currentItemDir = ConvertWStrToStr(entry.path().c_str());

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

            std::reverse(nameOfItemWithDate.begin(), nameOfItemWithDate.end());
            std::reverse(nameOfImage.begin(), nameOfImage.end());

            if (nameOfItemWithDate[0] == 'M')
            {
                
                monImages.emplace_back(nameOfImage);

            }
            else if (nameOfItemWithDate[0] == 'T' && nameOfItemWithDate[1] == 'u')
            {
                
                tueImages.emplace_back(nameOfImage);
            }
            else if (nameOfItemWithDate[0] == 'W')
            {
                
                wedImages.emplace_back(nameOfImage);
            }
            else if (nameOfItemWithDate[0] == 'T')
            {
                
                thuImages.emplace_back(nameOfImage);
            }
            else if (nameOfItemWithDate[0] == 'F')
            {
                
                friImages.emplace_back(nameOfImage);

            }
            else if (nameOfItemWithDate[0] == 'S' && nameOfItemWithDate[1] == 'a')
            {
                
                satImages.emplace_back(nameOfImage);

            }
            else
            {
                
                sunImages.emplace_back(nameOfImage);
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
        std::wstring imageDirW = ConvertStrToWStr(imageDir);
        // Better to convert image names here than at list, as we only have to do it once instead of multiple times.
        if (currentDay == daysLong[0])
        {
            int value = rand() % monImages.size();
            std::wstring completeDir = imageDirW + ConvertStrToWStr("Monday/") + ConvertStrToWStr(monImages[value]);
            auto imageSpecificDir = completeDir.c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if(currentDay == daysLong[1])
        {
            int value = rand() % tueImages.size();
            std::wstring completeDir = imageDirW + ConvertStrToWStr("Tuesday/") + ConvertStrToWStr(tueImages[value]);
            auto imageSpecificDir = completeDir.c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[2])
        {
            int value = rand() % wedImages.size();
            std::wstring completeDir = imageDirW + ConvertStrToWStr("Wednesday/") + ConvertStrToWStr(wedImages[value]);
            auto imageSpecificDir = completeDir.c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[3])
        {
            int value = rand() % thuImages.size();
            std::wstring completeDir = imageDirW + ConvertStrToWStr("Thursday/") + ConvertStrToWStr(thuImages[value]);
            auto imageSpecificDir = completeDir.c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[4])
        {
            int value = rand() % friImages.size();
            std::wstring completeDir = imageDirW + ConvertStrToWStr("Friday/") + ConvertStrToWStr(friImages[value]);
            auto imageSpecificDir = completeDir.c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[5])
        {
            int value = rand() % satImages.size();
            std::wstring completeDir = imageDirW + ConvertStrToWStr("Saturday/") + ConvertStrToWStr(satImages[value]);
            auto imageSpecificDir = completeDir.c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        else if (currentDay == daysLong[6])
        {
            int value = rand() % sunImages.size();
            std::wstring completeDir = imageDirW + ConvertStrToWStr("Sunday/") + ConvertStrToWStr(sunImages[value]);
            auto imageSpecificDir = completeDir.c_str();
            if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
            {
                std::cout << "Error" << std::endl;
            }
        }
        
    }

    return 0;
}