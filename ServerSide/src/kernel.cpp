#include <stdlib.h>

#ifdef ARDUINO_MOCK
#   include "../../Tests/headers/Arduino.h"
#else 
#   include <Arduino.h>
#endif

#include <string.h>
#include "math.h"

#include "../headers/kernel.h"
#include "../headers/ai.h"

#define FIELD_SIZE 3
char **_game_field;
const size_t g_FIELD_SIZE = FIELD_SIZE;

size_t step = 0;


enum role
{
    X = 'X',
    O = 'O'
};

void _render_board();
int _detect_winner();
void _announce_winner(char _winner);
void _announce_draw();
void _count_step();

char resolve_opponent_player(char _p)
{
    return _p == 'X'? 'O': 'X';
}

char resolve_current_player()
{
    return step % 2 ? O : X;
}

char** get_board()
{
    return _game_field;
}

void init_game(char *_load_info)
{   
    if(!_load_info)
    {
        _game_field = (char**)malloc(FIELD_SIZE * sizeof(char*));
        for(int i = 0; i < FIELD_SIZE; i++)
        { 
            _game_field[i] = (char*)malloc(FIELD_SIZE);
            memset(_game_field[i], EMPTY_CELL, FIELD_SIZE);
        }
    }
    else
    {
        _game_field = (char**)malloc(FIELD_SIZE * sizeof(char*));
        for(int i = 0; i < FIELD_SIZE; i++)
        { 
            _game_field[i] = (char*)malloc(FIELD_SIZE);
            for (int j = 0; j < FIELD_SIZE; j++)
            {
                _game_field[i][j] = _load_info[i * FIELD_SIZE + j];
            }

            _count_step();
        }
    }
}

void set_up_ai()
{
    char AI_mode = receive_mode();
    set_complexity(AI_mode);
    
    Serial.write((char)1);
}

void end_game()
{
    step = 0;
    for(int i = 0; i < FIELD_SIZE; i++)
        memset(_game_field[i], EMPTY_CELL, FIELD_SIZE);
}

char resolve_winner_player()
{
    if(_detect_winner())
    {
        return resolve_current_player();
    }

    return 0;
}

int make_move(unsigned int _i, unsigned int _j)
{   
    if (_i >= FIELD_SIZE || _j >= FIELD_SIZE) 
        return OUT_OF_BOUNDS;
    if(_game_field[_i][_j] != EMPTY_CELL) 
        return CELL_IS_OCCUPIED;

    _game_field[_i][_j] = resolve_current_player();

    ++step;

    return SUCCESS;
}

int game_status()
{
    int winner = _detect_winner();
    int ret_val = GAME_CONTINUES;

    if(winner)
        ret_val = WINNER_FOUND;

    if (step == pow(FIELD_SIZE, 2))
        ret_val = DRAW;

    _render_board();

    return ret_val;
}

void announce_player()
{   
    char *bytes = (char*)malloc(2);
    bytes[0] = step % 2 ? 1 : 0;
    bytes[1] = step;

    Serial.write((const uint8_t*)bytes, 2);
}

void _count_step()
{
    step = 0;
    for (int i = 0; i < FIELD_SIZE; i++)
    {
        for (int j = 0; j < FIELD_SIZE; j++)
        {
            if(_game_field[i][j] != EMPTY_CELL) step++;
        }
    }
}

int _detect_winner()
{
    for(int i = 0; i < FIELD_SIZE; i++)
    {   
        int isLineComplete = 1;
        char prev_el = _game_field[i][0];
        for(int j = 0; j < FIELD_SIZE; j++)
        {
            isLineComplete &= ( prev_el == _game_field[i][j] ) &&  ( _game_field[i][j] != EMPTY_CELL );
            prev_el = _game_field[i][j];
        }
        
        if(isLineComplete) return  prev_el;
    }

    for(int i = 0; i < FIELD_SIZE; i++)
    {   
        int isColumnComplete = 1; 
        char prev_el = _game_field[0][i];
        for(int j = 0; j < FIELD_SIZE; j++)
        {
            isColumnComplete &= ( prev_el == _game_field[j][i] ) && ( _game_field[j][i] != EMPTY_CELL );
            prev_el = _game_field[j][i];
        }
        
        if(isColumnComplete) return prev_el;
    }

    if (
        _game_field[0][0] == _game_field[1][1] &&
        _game_field[1][1] == _game_field[2][2] && 
        _game_field[0][0] != EMPTY_CELL
    )
        return _game_field[1][1];
          
    if (
        _game_field[0][2] == _game_field[1][1] &&
        _game_field[1][1] == _game_field[2][0] &&
        _game_field[0][2] != EMPTY_CELL
    )
        return _game_field[1][1];

    return 0;
}

void _render_board()
{
    for (int i = 0; i < FIELD_SIZE; i++)
    {
        for (int j = 0; j < FIELD_SIZE; j++)
        {   
            Serial.print(_game_field[i][j]);
        }
    }
}

