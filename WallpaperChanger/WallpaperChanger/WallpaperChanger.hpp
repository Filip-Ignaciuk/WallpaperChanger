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

    inline static const char* daysLong[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    inline static const std::wstring daysLongW[7] = { ConvertStrToWStr("Monday"),  ConvertStrToWStr("Sunday"), ConvertStrToWStr("Tuesday") , ConvertStrToWStr("Wednesday") , ConvertStrToWStr("Thursday") , ConvertStrToWStr("Friday") , ConvertStrToWStr("Saturday") };
    inline static const std::string documentDir = GetDocumentDir();
    // Need to fix this, gives out bad alloc errors without copy.
    inline static  std::string documentDirCopy = documentDir;
    inline static const std::string imageDir = NormaliseDir(documentDirCopy) + "/WallpaperChanger/Images";;
    inline static int currentDay = GetCurrentWeekDay();
    inline static std::string currentDateDir = imageDir + "/" + daysLong[currentDay];

public:
    static constexpr int numOfConfigs = 3;
private:

    inline static const char* configurations[numOfConfigs] = { "TimeLimit", "TimeActive", "TimeActivePriority" };;

    // Initialising files if they haven't been created.
    static void initFolders();

    static void InitImages();

    static bool CheckIfImageTextFilesIsValid(std::ifstream& _file);

    static bool CheckIfImageTextFilesIsValid(std::string& _line);

    static void GetImages(const std::string& _entry, std::vector<std::string>& _vector);

    static void SetWallpaper(const std::vector<std::string>& _images);

    static void SetColorAccent();

    static bool IsDigit(const char* _num);

public:



    static void StartWallpaperChanger();

    static void EndWallpaperChanger();

    static void SetImageConfiguration(const std::string& _fileName, const int _dayOfImage, const std::string& _config, const std::string& _data);

    static void GetImageConfiguration(const std::string& _fileName, const int _dayOfImage, int _configs[numOfConfigs]);
};