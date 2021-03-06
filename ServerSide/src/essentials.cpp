#ifdef ARDUINO_MOCK
#   include "../../Tests/headers/Arduino.h"
    void delay(const int _m){};
#else 
#   include <Arduino.h>
#endif

#include "../headers/kernel.h"
#include "../headers/input.h"
#include "../headers/ai.h"

void game_session()
{   
    while(1)
    {
        char *in = receive_input();

        if( ( in[0] & 0xFF ) == SAVE_REQUEST )
        {   
            Serial.write((const uint8_t*)in, 1);
            game_status();
            continue;
        }

        char move_stat = make_move(in[0], in[1]);
        Serial.write((const uint8_t*)&move_stat, 1);

        char game_stat = game_status();
        
        announce_player();
        Serial.write((const uint8_t*)&game_stat, 1);
        

        if (game_stat != GAME_CONTINUES)
            break;
    }

    end_game();
}


void AI_game_session()
{
    while(1)
    {
        char move_stat = comp_move( get_board() );
        Serial.write((const uint8_t*)&move_stat, 1);

        char game_stat = game_status();
        
        announce_player();
        Serial.write((const uint8_t*)&game_stat, 1);

        delay(2000);

        if (game_stat != GAME_CONTINUES)
            break;
    }

    end_game();
}

void combined_game_session()
{
    bool isAI = false;

    while(1)
    {   
        char move_stat;
        if(!isAI)
        {
            char *in = receive_input();

            if( ( in[0] & 0xFF ) == SAVE_REQUEST )
            {   
                Serial.write((const uint8_t*)in, 1);
                game_status();
                continue;
            }

            char move_stat = make_move(in[0], in[1]);
        }
        else
        {
            char move_stat = comp_move( get_board() );
        }

        Serial.write((char)move_stat);

        char game_stat = game_status();
        
        announce_player();
        Serial.write((char)game_stat);
        
        isAI = !isAI;

        if (game_stat != GAME_CONTINUES)
            break;
    }

    end_game();
}