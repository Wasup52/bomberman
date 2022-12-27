#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

// FOR LINUX
#include <termios.h>
#include <fcntl.h>
static struct termios origtc, newtc;

#define CLOCK_MONOTONIC 1 // used for clock_gettime

#define TRUE 256
#define FALSE 0

#define WALL 1
#define BOMB 2
#define OBSTACLE 3
#define PARTICLE 4

#define BOMBER1 5
#define BOMBER2 6

#define MAX_BOMBS 10

enum direction {UP, DOWN, LEFT, RIGHT, IDDLE};
// enum bomber {BOMBER1, BOMBER2};


typedef struct
{
    int pos_l; // the current line of the bomb
    int pos_c; // the current column of the bomb
    int range; // the range of the bomb
    int timer; // the timer of the bomb
} bomb_t;

// typedef struct
// {
//     int pos_l; // the current line of the particle
//     int pos_c; // the current column of the particle
//     int timer; // the timer of the particle
// } particle_t;

typedef struct 
{
    int **board_array; // the board
    int lines_nb; // number of lines in the board
    int columns_nb; // number of columns in the board
    int bombs_count; // number of bombs
    bomb_t bombs_list[MAX_BOMBS]; // list of bombs
    int is_over; // 1 if the game is over
    int winner; // 1 if the winner is the first player, 2 if the winner is the second player
} game_t;

typedef struct
{
    game_t *game;
    int id; // the id of the bomber
    int pos_l; // the current line of the bomber
    int prev_l; // the previous line of the bomber
    int pos_c; // the current column of the bomber
    int prev_c; // the previous column of the bomber
    int bomb_n; // the timer and range of the bomb
    enum direction direction; // the current direction of the bomber
    char up; // the key to move up
    char down; // the key to move down
    char left; // the key to move left
    char right; // the key to move right
    char bomb; // the key to drop a bomb
} bomber_t;

typedef struct
{
    bomb_t *bomber1;
    bomb_t *bomber2;
} bombers_array_t;

void *get_keystrock(void *arg);
void get_board(char *fichier, game_t *board);
void clear_screen();
void sleep_ms(float ms);
void draw_board(game_t board);
void place_bomber(bomber_t *bomber);
void place_bomb(bomber_t *bomber);
void move(bomber_t *bomber);
int can_move(bomber_t * bomber, int direction);
void *game_loop(void *arg);
void explode_bomb(int i, game_t *game);
void check_bombs(game_t *game);
int custom_round(float x);
float linear_interpolation(float x1, float y1, float x2, float y2, float t);
int is_in_explode_zone(int pos_l, int pos_c, bomb_t bomb, game_t game);
int get_bomb_index(int pos_l, int pos_c, game_t game);
double *linspace(double a, double b, int n);