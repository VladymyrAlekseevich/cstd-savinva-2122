void init_game();
void end_game();

#define SUCCESS 0
#define OUT_OF_BOUNDS 1
#define CELL_IS_OCCUPIED 2

int make_move(unsigned int _i, unsigned int _j);

#define GAME_CONTINUES 0
#define WINNER_FOUND 1
#define DRAW 2

int game_status();