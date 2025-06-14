#pragma once
#include <Arduino.h>
#include <LittleFS.h>

class FileSystem
{
public:
    FileSystem();

    String get_file_size_formatted(size_t bytes);
    void list_directory(const char *dirname, uint8_t depth);
    void print_file_system_contents();

    void write_file(const char *path, const char *message);
    void print_file(const char *path);
    bool delete_file(const char *path);

private:
};