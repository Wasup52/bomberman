#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <strings.h>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
struct sockaddr_in *p_exp; //pointeur adresse internet de l'expediteur (depuis UDP recu)

#include <termios.h>
#include <fcntl.h>
static struct termios origtc, newtc;

#define PORT 5677

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
    char sent_c[10];
} globals_t;

typedef struct 
{
    int **board_array; // the board
    int lines_nb; // number of lines in the board
    int columns_nb; // number of columns in the board
    int bombs_count; // number of bombs
    bomb_t bombs_list[MAX_BOMBS]; // list of bombs
    int is_over; // 1 if the game is over
    int winner; // 1 if the winner is the first player, 2 if the winner is the second player

    int socket;
    globals_t *globals;
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

    int socket;
    globals_t *globals;
} bomber_t;



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

int init_serveur();
int init_client();
void *receive_data(int s, size_t data_size);
void send_data(int s, char* adresse, void* message, size_t data_size);