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


const int GetCurrentDay()
{
    time_t rawtime;
    tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    int wday = timeinfo->tm_wday;

    return wday;

}

std::string GetDocumentDir()
{
    TCHAR path[MAX_PATH];
    if (!FAILED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path))) {
        return ConvertWStrToStr(path);
    }

    std::cout << "Error with obtaining document directory." << std::endl;
    return "";
}


const char* daysLong[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const std::string documentDir = GetDocumentDir();
// In this case we can add / at the end of the directory as in all references of the variable it needs it.
std::string documentDirCopy = documentDir;
const std::string imageDir = NormaliseDir(documentDirCopy) + "/WallpaperChanger/Images/"; 
const int currentDay = GetCurrentDay();


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


void GetImages(std::filesystem::directory_entry _entry, std::vector<std::string> &_vector)
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


    std::vector<std::string> Images;
    std::wstring imageDirW = ConvertStrToWStr(imageDir);

    bool isCorrect = false;

    if (currentDay == 1)
    {
        imageDirDay = imageDir + daysLong[0];

        for (const auto& entry : std::filesystem::directory_iterator(imageDirDay))
        {
            GetImages(entry, Images);
        }

        for (unsigned int i = 0; i < Images.size(); i++)
        {
            if (Images[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }
        // Better to convert image names here than at list, as we only have to do it once instead of multiple times.
        int value = rand() % Images.size();
        std::wstring completeDir = imageDirW + ConvertStrToWStr("Monday/") + ConvertStrToWStr(Images[value]);
        auto imageSpecificDir = completeDir.c_str();
        if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
        {
            std::cout << "Error" << std::endl;
        }
    }
    else if (currentDay == 2)
    {
        imageDirDay = imageDir + daysLong[1];

        for (const auto& entry : std::filesystem::directory_iterator(imageDirDay))
        {
            GetImages(entry, Images);
        }

        for (unsigned int i = 0; i < Images.size(); i++)
        {
            if (Images[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }

        int value = rand() % Images.size();
        std::wstring completeDir = imageDirW + ConvertStrToWStr("Tuesday/") + ConvertStrToWStr(Images[value]);
        auto imageSpecificDir = completeDir.c_str();
        if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
        {
            std::cout << "Error" << std::endl;
        }

    }
    else if (currentDay == 3)
    {
        imageDirDay = imageDir + daysLong[2];

        for (const auto& entry : std::filesystem::directory_iterator(imageDirDay))
        {
            GetImages(entry, Images);
        }

        for (unsigned int i = 0; i < Images.size(); i++)
        {
            if (Images[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }

        int value = rand() % Images.size();
        std::wstring completeDir = imageDirW + ConvertStrToWStr("Wednesday/") + ConvertStrToWStr(Images[value]);
        auto imageSpecificDir = completeDir.c_str();
        if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
        {
            std::cout << "Error" << std::endl;
        }
    }
    else if (currentDay == 4)
    {
        imageDirDay = imageDir + daysLong[3];

        for (const auto& entry : std::filesystem::directory_iterator(imageDirDay))
        {
            GetImages(entry, Images);
        }

        for (unsigned int i = 0; i < Images.size(); i++)
        {
            if (Images[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }

        int value = rand() % Images.size();
        std::wstring completeDir = imageDirW + ConvertStrToWStr("Thursday/") + ConvertStrToWStr(Images[value]);
        auto imageSpecificDir = completeDir.c_str();
        if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
        {
            std::cout << "Error" << std::endl;
        }

    }
    else if (currentDay == 5)
    {
        imageDirDay = imageDir + daysLong[4];

        for (const auto& entry : std::filesystem::directory_iterator(imageDirDay))
        {
            GetImages(entry, Images);
        }

        for (unsigned int i = 0; i < Images.size(); i++)
        {
            if (Images[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }

        int value = rand() % Images.size();
        std::wstring completeDir = imageDirW + ConvertStrToWStr("Friday/") + ConvertStrToWStr(Images[value]);
        auto imageSpecificDir = completeDir.c_str();
        if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
        {
            std::cout << "Error" << std::endl;
        }

    }
    else if (currentDay == 6)
    {
        imageDirDay = imageDir + daysLong[5];

        for (const auto& entry : std::filesystem::directory_iterator(imageDirDay))
        {
            GetImages(entry, Images);
        }

        for (unsigned int i = 0; i < Images.size(); i++)
        {
            if (Images[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }

        int value = rand() % Images.size();
        std::wstring completeDir = imageDirW + ConvertStrToWStr("Saturday/") + ConvertStrToWStr(Images[value]);
        auto imageSpecificDir = completeDir.c_str();
        if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
        {
            std::cout << "Error" << std::endl;
        }
    }
    else if (currentDay == 7)
    {
        imageDirDay = imageDir + daysLong[6];

        for (const auto& entry : std::filesystem::directory_iterator(imageDirDay))
        {
            GetImages(entry, Images);
        }

        for (unsigned int i = 0; i < Images.size(); i++)
        {
            if (Images[i] == nameOfCurrentImage)
            {
                isCorrect = true;
            }
        }

        int value = rand() % Images.size();
        std::wstring completeDir = imageDirW + ConvertStrToWStr("Sunday/") + ConvertStrToWStr(Images[value]);
        auto imageSpecificDir = completeDir.c_str();
        if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)imageSpecificDir, SPIF_SENDCHANGE))
        {
            std::cout << "Error" << std::endl;
        }

    }
    return 0;
}