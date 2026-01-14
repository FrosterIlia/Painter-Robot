/**
 * @file FileSystem.h
 * @author Ilia Moroz (iliamorozim@gmail.com)
 * @brief Class for convenient use of the file system on ESP32 using Serial port
 * @version 1.0
 * @date 2025-07-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once
#include <Arduino.h>
#include <LittleFS.h>

class FileSystem
{
public:
    FileSystem();
    bool begin(bool format_on_fail = true);

    String get_file_size_formatted(size_t bytes);
    void list_directory(const char *dirname, uint8_t depth);
    void print_file_system_contents();

    void write_file(const char *path, const char *message);
    void print_file(const char *path);
    bool delete_file(const char *path);

private:
};
