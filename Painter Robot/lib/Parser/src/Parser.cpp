#include "Parser.h"

Parser::Parser()
{
    Serial.println("test1");
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

Parser::Parser(const char *path)
{
    Serial.println("test2");
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

    if (open(path))
    {
        Serial.println(F("Parser initialized"));
    }
    else
    {
        Serial.println(F("Failed to initialize parser"));
    }
}

bool Parser::open(const char *path)
{
    if (path == NULL)
    {
        Serial.println(F("Can't open file, NULL pointer passed"));
        return false;
    }

    _current_file = LittleFS.open(path);

    if (!_current_file || _current_file.isDirectory())
    {
        Serial.println(F("Failed to open file for reading"));
        return false;
    }

    return true;
}

Parser::Command Parser::get_next_cmd()
{
    if (!_current_file)
    {
        Serial.println(F("ERROR: File not opened"));
        return Command::INVALID;
    }

    if (_current_file.available())
    {
        char buffer[BUFFER_SIZE];
        uint8_t size = _current_file.readBytesUntil('\n', buffer, BUFFER_SIZE);
        buffer[size] = '\0';

        char command_symbol = (char)buffer[0];

        switch (command_symbol)
        {
        case 'm':
        {
            if (size < 5)
            {
                Serial.println(F("Error: MOVE command is too short"));
                return Command::INVALID;
            }

            if (buffer[1] != ' ')
            {
                Serial.println(F("ERROR: Missing space after 'm'"));
                return Command::END_OF_FILE;
            }
            char *p_pos = &buffer[2];           // Getting to the point with coordinates
            char *p_comma = strchr(p_pos, ','); // Looking for a comma

            if (p_comma == 0)
            {
                Serial.println(F("ERROR: Comma not found"));
                return Command::INVALID;
            }
            
            _target_pos.x = atoi(p_pos);
            _target_pos.y = atoi(p_comma + 1);
            _pos_ready = true;

            return Command::MOVE;
        }

        case 'p':
            if (strlen(buffer) != 3)
            {
                Serial.println(F("ERROR: PEN command must be 3 chars"));
                return Command::INVALID;
            }

            if (buffer[1] != ' ')
            {
                Serial.println("ERROR: Missing space after 'p'");
                return Command::INVALID;
            }

            switch ((char)buffer[2])
            {
            case '0':
                return Command::DOWN;
                break;

            case '1':
                return Command::UP;
                break;

            default:
                Serial.println(F("ERROR: Invalid PEN value (must be 0 or 1)"));
                return Command::INVALID;
            }
            break;

        default:
            return Command::INVALID;
        }

        return Command::MOVE;
    }
    else
    {
        return Command::END_OF_FILE;
    }
}

Pos Parser::get_target_pos()
{
    if (_pos_ready)
    {
        _pos_ready = false;
        return _target_pos;
    }
    Pos temp_pos = {-1, -1};
    return temp_pos;
}

bool Parser::is_done()
{
    return !_current_file.available();
}