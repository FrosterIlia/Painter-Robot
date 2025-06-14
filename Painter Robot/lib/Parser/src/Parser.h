#pragma once
#include <Arduino.h>
#include <LittleFS.h>

#define BUFFER_SIZE 15 // Maximum line size

/*
Sample instructions file (exclude comments):
m 0,0;      # move (0, 0)
m 12,10;    # move (12, 10)
p 1;        # pen up
m 7,3;      # move (7, 3)
p 0;        # pen down
*/

typedef struct
{
    int x;
    int y;
} Pos;

class Parser
{
public:
    enum class Command
    {
        UP,
        DOWN,
        MOVE,
        INVALID,
        END_OF_FILE
    };

    Parser();
    Parser(const char *path);

    bool open(const char *path);
    Command get_next_cmd();
    Pos get_target_pos();
    uint8_t get_progress();
    bool is_done();

private:
    File _current_file;
    Pos _target_pos;
    bool _pos_ready = false;
};