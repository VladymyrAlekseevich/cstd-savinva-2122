#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "headers/kernel.h"
#include "headers/USB.h"

#define PACKAGE_SIZE 13
#define BOARD_SIZE 3

#define HANDSHAKE_CONST 0xFE

char* _parse_package(char *_package)
{   
    switch(_package[0]){
        case SUCCESS:
            printf("X  ");
            for(int i = 0; i < BOARD_SIZE; i++) printf("%d ", i);
            puts(" ");
            puts("Y|——————");

            for (int i = 0; i < BOARD_SIZE; i++)
            {   
                printf("%d| ", i);
                for (char *p = _package + 1 + i*BOARD_SIZE, j = 0; j < BOARD_SIZE; j++, p++)
                {
                    printf("%c ", *p);
                }
                puts("");
            }
            puts("");
            break;
        case OUT_OF_BOUNDS:
            puts("Error: the index is out of bounds!");
            break;
        case CELL_IS_OCCUPIED:
            puts("Error: the cell is already occupied!");
            break;
    }

    fflush(stdout);

    return _package + 10;
}

void input_coords()
{
    char *X_str = malloc(1);
    char *Y_str = malloc(1);

    puts("Enter X coord:");
    gets(X_str);

    puts("Enter Y coord:");
    gets(Y_str);

    char X = atoi(X_str), Y = atoi(Y_str);
    char package =  ( Y << 2 ) | X;

    usb_write(&package, 1);
}

void receive_response()
{
    char *package = malloc(PACKAGE_SIZE);

    //wait until some bytes are read
    while(!usb_read(package, PACKAGE_SIZE));

    char *game_status = _parse_package(package);
    if(game_status[0] != GAME_CONTINUES)
    {
        printf("Step: %d\n", game_status[1]);
        
        char *str = game_status[0] == WINNER_FOUND
            ? "%s won!\n"
            : "Draft\n";

        printf(str, game_status[2] ? "O": "X" ); 
    }
}

void handshake(){
    char *res = malloc(1);
    do
    {
        char HNDHK_CONST = HANDSHAKE_CONST;
        usb_write(&HNDHK_CONST, 1);

        usb_read(res, 1);
        printf("The response was gotten: %2X\n", *res);

        usb_write(res, 1);


        usb_read(res, 1);

        usleep(1000 * 1000); //50 ms
    } while ( (*res) & 0xFF != HANDSHAKE_CONST);

    puts("Stable communication channel was established!");
    free(res);
}