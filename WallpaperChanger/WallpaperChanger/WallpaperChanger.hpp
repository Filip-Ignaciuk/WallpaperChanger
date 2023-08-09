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
    static std::string ConvertWStrToStr(const std::wstring& _wStr);

    // Conversion of strings to wStrings.
    static std::wstring ConvertStrToWStr(const std::string& _str);

    // Converting all the \\ slashes into one singular forward slash.
    static std::string NormaliseDir(std::string& _str);

    static int GetCurrentWeekDay();

    static std::string GetDocumentDir();

    static const char* daysLong[7];
    static const std::string documentDir;
	static std::string documentDirCopy;
    static const std::string imageDir;
    static const int currentDay;
    static constexpr int numOfConfigs = 3;
    static const char* configurations[numOfConfigs];

    // Initialising files if they haven't been created.
    static void initFiles();

    static void InitImages();

    static bool CheckIfImageTextFilesIsValid(std::ifstream& _file);

    static void GetImages(const std::filesystem::directory_entry& _entry, std::vector<std::string>& _vector);

    static void SetColorAccent();

public:
    static void StartWallpaperChanger();

};