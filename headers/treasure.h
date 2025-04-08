#pragma once

#include <stdint.h>

#define MAX_ID_LENGTH 16
#define MAX_USER_NAME_LENGTH 32
#define MAX_CLUE_TEXT_LENGTH 128

/*
    16bytes - id
    32bytes - user_name
    16bytes - coordinates
    128bytes - clue_text
    2bytes - value
*/


typedef struct {
    char id[16];
    char user_name[32];
    struct
    {
        double latitude;
        double longitude;
    } coordinates;
    char clue_text[128];
    uint16_t value;
    
} Treasure;