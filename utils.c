#include "game.h"

/*
 * Get the board array fom a file
 */
void get_board(char *fichier, game_t *game) {
	FILE *f;
	char str[100];
	int i,j;
	char ch;

    game->lines_nb = 0;
	f = fopen(fichier,"r");
    if (f == NULL){
        printf("!!! Can't open file: %s\n",fichier);
        exit(-1);
    }

	while (fgets(str,sizeof(str),f) != NULL){
		str[strlen(str)-1] = '\0';
		game->columns_nb = strlen(str);
		game->lines_nb++;
	}

	fclose(f);
	game->board_array = (int **)malloc(game->lines_nb*sizeof(int *));

	for (i=0; i < game->lines_nb;i++){
		game->board_array[i] = (int *)malloc(game->columns_nb*sizeof(int));
	}
    
	f = fopen(fichier,"r");
	i = 0; j = 0;
	while ((ch = getc(f)) != EOF) {
        if (ch != '\n') {
            if (ch == '#') {
                game->board_array[i][j] = 1;
            }else{
                game->board_array[i][j] = 0;
            }
            j++;
            if (j == game->columns_nb) {
                j = 0;
                i++;
            }
        }
	}
}

/*
 * Sleep for ms milliseconds
 */
void sleep_ms(float ms) {
    usleep(ms * 1000);
}

/*
 * Returns 1 if a key was pressed, 0 otherwise.
 * https://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
 */
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt); // save old settings
    newt = oldt; // copy old settings to new settings
    newt.c_lflag &= ~(ICANON | ECHO); // change the new settings to not wait for enter keypress
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply the new settings immediatly
    
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0); // save old fcntl flags
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // make fcntl flags non-blocking (return immediately)

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reapply the old settings
    fcntl(STDIN_FILENO, F_SETFL, oldf); // reapply the old fcntl flags

    if(ch != EOF) // if a character was read
    {
        ungetc(ch, stdin); // put the character back into the input stream
        return 1; // return 1 to indicate that a key was pressed
    }

    return 0; // return 0 to indicate that no key was pressed
}

/*
 * Removes the content in the terminal by printing a lot of new lines
 * Check this for more details : http://www.cplusplus.com/articles/4z18T05o/
 */
void clearScreen()
{
    int n;
    for (n = 0; n < 10; n++)
        printf( "\n\n\n\n\n\n\n\n\n\n");
}

/*
 * Draw the game in the terminal
 */
void draw_game(game_t game) {
    clearScreen();

	for (int i = 0; i < game.lines_nb; i++) {
        for (int j = 0; j < game.columns_nb; j++) {
            if (game.board_array[i][j] == WALL) {
                printf("#");
            } else if (game.board_array[i][j] == BOMBER1) {
                printf("1");
            } else if (game.board_array[i][j] == BOMBER2) {
                printf("2");
            } else if (game.board_array[i][j] == BOMB) {
                printf("o");
            } else if (game.board_array[i][j] == OBSTACLE) {
                printf("x");
            } else if (game.board_array[i][j] == PARTICLE) {
                printf("@");
            } else { // empty space
                printf(" ");
            }
        }
        printf("\n");
    }

    for (int i = 0; i < game.bombs_count; i++) {
        game.board_array[game.bombs_list[i].pos_l][game.bombs_list[i].pos_c] = BOMB;
    }
}

/*
 * Get precise time in nanoseconds
 */
long get_time()
{
    struct timespec timeValues = {0,0};
    clock_gettime(CLOCK_MONOTONIC, &timeValues);
    return timeValues.tv_nsec;
}

/*
 * Place the bomber on the board
 */
void place_bomber(bomber_t *bomber) {
    int row = rand() % bomber->game->lines_nb;
    int col = rand() % bomber->game->columns_nb;

    while (bomber->game->board_array[row][col] != 0) {
        // printf("get_time(): %lld\n", get_time());
        srand(get_time());
        row = rand() % bomber->game->lines_nb;
        col = rand() % bomber->game->columns_nb;
    }

    bomber->pos_l = row;
    bomber->pos_c = col;
    bomber->game->board_array[row][col] = bomber->id;
}

/*
 * Place 10 obstacle at random on the bomber on the board
 */
void place_obstacles(game_t *game) 
{
    int row, col;

    for (int i = 0; i < 10; i++) {
        row = rand() % game->lines_nb;
        col = rand() % game->columns_nb;

        while (game->board_array[row][col] != 0) {
            srand(get_time());
            row = rand() % game->lines_nb;
            col = rand() % game->columns_nb;
        }

        game->board_array[row][col] = OBSTACLE;
    }
}