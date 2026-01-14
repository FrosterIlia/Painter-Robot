#include "FileSystem.h"

FileSystem::FileSystem()
{
}

bool FileSystem::begin(bool format_on_fail)
{
    if (!LittleFS.begin(format_on_fail))
    {
        Serial.println(F("LittleFS mount failed"));
        return false;
    }

    Serial.println(F("FileSystem initialized successfully"));
    return true;
}

void FileSystem::write_file(const char *path, const char *contents)
{
    if (path == NULL || contents == NULL)
    {
        Serial.printf("Null pointer passed, File: %s, Line: %d", __FILE__, __LINE__);
        return;
    }
    Serial.printf("Writing file: %s\n", path);

    File file = LittleFS.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println(F("Failed to open file for writing"));
        return;
    }

    if (file.print(contents))
    {
        Serial.println(F("File written"));
    }
    else
    {
        Serial.println(F("Write failed"));
    }
    file.close();
}

void FileSystem::print_file(const char *path)
{
    if (path == NULL)
    {
        Serial.printf("Null pointer passed, File: %s, Line: %d", __FILE__, __LINE__);
        return;
    }
    Serial.printf("Reading file: %s\n", path);

    File file = LittleFS.open(path);
    if (!file || file.isDirectory())
    {
        Serial.println(F("Failed to open file for reading"));
        return;
    }

    Serial.println(F("File content:"));
    while (file.available())
    {
        Serial.write(file.read());
    }
    Serial.println();
    file.close();
}

bool FileSystem::delete_file(const char *path)
{
    if (path == NULL)
    {
        Serial.printf("Null pointer passed, File: %s, Line: %d", __FILE__, __LINE__);
        return false;
    }
    // Safety check - prevent accidental deletion of root directory
    if (strcmp(path, "/") == 0)
    {
        Serial.println(F("Error: Cannot delete root directory"));
        return false;
    }

    // Check if file exists first
    if (!LittleFS.exists(path))
    {
        Serial.printf("Error: File '%s' does not exist\n", path);
        return false;
    }

    // Check if it's actually a file (not a directory)
    File file = LittleFS.open(path);
    if (file.isDirectory())
    {
        file.close();
        Serial.printf("Error: '%s' is a directory, not a file\n", path);
        return false;
    }
    file.close();

    // Attempt deletion
    if (LittleFS.remove(path))
    {
        Serial.printf("Successfully deleted file '%s'\n", path);
        return true;
    }
    else
    {
        Serial.printf("Failed to delete file '%s'\n", path);
        return false;
    }
}

void FileSystem::print_file_system_contents()
{
    Serial.println(F("\nLittleFS File System Contents:"));
    Serial.println(F("============================="));

    // Start with the root directory
    list_directory("/", 0);

    Serial.println(F("============================="));
    Serial.println(F("End of file system contents\n"));
}

void FileSystem::list_directory(const char *dirname, uint8_t depth)
{
    // Open the directory
    File root = LittleFS.open(dirname);
    if (!root)
    {
        Serial.println(F("- Failed to open directory"));
        return;
    }

    if (!root.isDirectory())
    {
        Serial.println(F("- Not a directory"));
        return;
    }

    // Process all files/directories in the current directory
    File file = root.openNextFile();
    while (file)
    {
        // Indent based on directory depth
        for (int i = 0; i < depth; i++)
        {
            Serial.print(F("  "));
        }

        if (file.isDirectory())
        {
            // Print directory info
            Serial.print(F("+ [DIR]  "));
            Serial.print(file.name());
            Serial.print(F("  ("));
            Serial.print(get_file_size_formatted(0)); // Directories show 0 size
            Serial.println(F(")"));

            // Recursively list directory contents
            list_directory(file.name(), depth + 1);
        }
        else
        {
            // Print file info
            Serial.print(F("- [FILE] "));
            Serial.print(file.name());
            Serial.print(F("  ("));
            Serial.print(get_file_size_formatted(file.size()));
            Serial.print(F(")"));

            Serial.println();
        }
        file = root.openNextFile();
    }

    file.close();
    root.close();
}

String FileSystem::get_file_size_formatted(size_t bytes)
{
    if (bytes < 1024)
    {
        return String(bytes) + F(" bytes");
    }
    else if (bytes < (1024 * 1024))
    {
        return String(bytes / 1024.0, 2) + F(" KB");
    }
    else
    {
        return String(bytes / 1024.0 / 1024.0, 2) + F(" MB");
    }
}
