#pragma once
#include <Arduino.h>
#include <LittleFS.h>

class FileSystem
{
public:
    FileSystem();

    String getFileSizeFormatted(size_t bytes);
    void listDirectory(const char *dirname, uint8_t depth);
    void printFileSystemContents();

    void writeFile(const char *path, const char *message);
    void printFileContents(const char *path);
    bool deleteFile(const char *path);

private:
};