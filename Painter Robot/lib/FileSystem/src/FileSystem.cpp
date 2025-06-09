#include "FileSystem.h"

FileSystem::FileSystem()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS mount failed, attempting to format...");

        // Full format
        if (LittleFS.format())
        {
            Serial.println("Format successful, retrying mount...");
            if (!LittleFS.begin(true))
            {
                Serial.println("Mount after format failed! Check partition table.");
                while (1) // Halt on failure
                {
                }
            }
        }
        else
        {
            Serial.println("Format failed! Check flash memory.");
            while (1)
            {
            }
        }
    }
}

void FileSystem::writeFile(const char *path, const char *contents)
{
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

void FileSystem::printFileContents(const char *path)
{
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

bool FileSystem::deleteFile(const char *path)
{
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

void FileSystem::printFileSystemContents()
{
    Serial.println(F("\nLittleFS File System Contents:"));
    Serial.println(F("============================="));

    // Start with the root directory
    listDirectory("/", 0);

    Serial.println(F("============================="));
    Serial.println(F("End of file system contents\n"));
}

void FileSystem::listDirectory(const char *dirname, uint8_t depth)
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
            Serial.print(getFileSizeFormatted(0)); // Directories show 0 size
            Serial.println(F(")"));

            // Recursively list directory contents
            listDirectory(file.name(), depth + 1);
        }
        else
        {
            // Print file info
            Serial.print(F("- [FILE] "));
            Serial.print(file.name());
            Serial.print(F("  ("));
            Serial.print(getFileSizeFormatted(file.size()));
            Serial.print(F(")"));

            Serial.println();
        }
        file = root.openNextFile();
    }

    file.close();
    root.close();
}

String FileSystem::getFileSizeFormatted(size_t bytes)
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