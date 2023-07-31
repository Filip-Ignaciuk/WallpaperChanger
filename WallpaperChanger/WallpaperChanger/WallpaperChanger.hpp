#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <Windows.h>
#include <filesystem>
#include <codecvt>
#include <ShlObj.h>

class WallpaperChanger
{
private:
    static std::string ConvertWStrToStr(const std::wstring& wStr);

    // Conversion of strings to wStrings.
    static const std::wstring ConvertStrToWStr(const std::string& str);

    // Converting all the \\ shlashes into one singular forward slash.
    static const std::string NormaliseDir(std::string& str);

    static const int GetCurrentWeekDay();

    static std::string GetDocumentDir();

    static const char* daysLong[7];
    static const std::string documentDir;
    // In this case we can add / at the end of the directory as in all references of the variable it needs it.
    static std::string documentDirCopy;
    static const std::string imageDir;
    static const int currentDay;

    // Initialising files if they havent been created.
    static void initFiles();


    static void GetImages(std::filesystem::directory_entry _entry, std::vector<std::string>& _vector);

    static void SetColorAccent();

public:
    static void StartWallpaperChanger();

};