#include "game.h"

/*
 * Get the board array fom a file
 */
void get_board(char *fichier, game_t *game)
{
    FILE *f;
    char str[100];
    int i, j;
    char ch;

    game->lines_nb = 0;
    f = fopen(fichier, "r");
    if (f == NULL)
    {
        printf("!!! Can't open file: %s\n", fichier);
        exit(-1);
    }

    while (fgets(str, sizeof(str), f) != NULL)
    {
        str[strlen(str) - 1] = '\0';
        game->columns_nb = strlen(str);
        game->lines_nb++;
    }

    fclose(f);
    // game->board_array = (int **)malloc(game->lines_nb * sizeof(int *));

    // for (i = 0; i < game->lines_nb; i++)
    // {
    //     game->board_array[i] = (int *)malloc(game->columns_nb * sizeof(int));
    // }

    f = fopen(fichier, "r");
    i = 0;
    j = 0;
    while ((ch = getc(f)) != EOF)
    {
        if (ch != '\n')
        {
            if (ch == '#')
            {
                game->board_array[i][j] = 1;
            }
            else
            {
                game->board_array[i][j] = 0;
            }
            j++;
            if (j == game->columns_nb)
            {
                j = 0;
                i++;
            }
        }
    }
}

/*
 * Sleep for ms milliseconds
 */
void sleep_ms(float ms)
{
    usleep(ms * 1000);
}

/*
 * Returns 1 if a key was pressed, 0 otherwise.
 * https://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
 */
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;     // char
    int oldf;   // old flags

    tcgetattr(STDIN_FILENO, &oldt);          // save old settings
    newt = oldt;                             // copy old settings to new settings
    newt.c_lflag &= ~(ICANON | ECHO);        // change the new settings to not wait for enter keypress
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply the new settings immediatly

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);          // save old fcntl flags
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // make fcntl flags non-blocking (return even if there is no input)

    ch = getchar(); // check for input

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reapply the old settings
    fcntl(STDIN_FILENO, F_SETFL, oldf);      // reapply the old fcntl flags

    if (ch != EOF) // if a character was read
    {
        ungetc(ch, stdin); // put the character back into the input stream
        return 1;          // return 1 to indicate that a key was pressed
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
        printf("\n\n\n\n\n\n\n\n\n\n");
}

/*
 * Draw the game in the terminal
 */
void draw_game(game_t game)
{
    clearScreen();

    for (int i = 0; i < game.lines_nb; i++)
    {
        for (int j = 0; j < game.columns_nb; j++)
        {
            if (game.board_array[i][j] == WALL)
            {
                printf("#");
            }
            else if (game.board_array[i][j] == BOMBER1)
            {
                printf("1");
            }
            else if (game.board_array[i][j] == BOMBER2)
            {
                printf("2");
            }
            else if (game.board_array[i][j] == BOMB)
            {
                printf("o");
            }
            else if (game.board_array[i][j] == OBSTACLE)
            {
                printf("x");
            }
            else if (game.board_array[i][j] == PARTICLE)
            {
                printf("@");
            }
            else
            { // empty space
                printf(" ");
            }
        }
        printf("\n");
    }

    for (int i = 0; i < game.bombs_count; i++)
    {
        game.board_array[game.bombs_list[i].pos_l][game.bombs_list[i].pos_c] = BOMB;
    }
}

/*
 * Print the scores of the two bombers
 */
void print_scores(game_t game)
{
    // printf("Bomber 1 bombs_placed: %d | Bomber 1 obstacles_destroyed: %d | Bomber 1 bomb_n: %d\n", game.bomber1_stats.bombs_placed, game.bomber1_stats.obstacles_destroyed, game.bomber1_stats.bomb_n);
    // printf("Bomber 2 bombs_placed: %d | Bomber 2 obstacles_destroyed: %d | Bomber 2 bomb_n: %d\n", game.bomber2_stats.bombs_placed, game.bomber2_stats.obstacles_destroyed, game.bomber2_stats.bomb_n);
    printf("Bomber 1 score: %f | Bomber 2 score: %f\n", get_score(game.bomber1_stats), get_score(game.bomber2_stats));
}

/*
 * Get precise time in nanoseconds
 */
long get_time()
{
    struct timespec timeValues = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &timeValues);
    return timeValues.tv_nsec;
}

/*
 * Place the bomber on the board
 */
void place_bomber(bomber_t *bomber)
{
    // could give arithemetic exception if lines_nb = 0 or columns_nb = 0
    int row = rand() % bomber->game->lines_nb;
    int col = rand() % bomber->game->columns_nb;

    while (bomber->game->board_array[row][col] != 0)
    {
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
 * Place 10 obstacle randomly on the bomber on the board
 */
void place_obstacles(game_t *game)
{
    int row, col;

    for (int i = 0; i < OBSTACLES_NB; i++)
    {
        row = rand() % game->lines_nb;
        col = rand() % game->columns_nb;

        while (game->board_array[row][col] != 0)
        {
            srand(get_time());
            row = rand() % game->lines_nb;
            col = rand() % game->columns_nb;
        }

        game->board_array[row][col] = OBSTACLE;
    }
}

/*
 * Check if the bomber can move in the given direction
 */
int can_move(bomber_t * bomber, int direction) {
    // check if the next position is a wall or an obstacle or out of the board or a bomber
    if (direction == UP) {
        return (bomber->pos_l > 0 
            && bomber->game->board_array[bomber->pos_l - 1][bomber->pos_c] != WALL 
            && bomber->game->board_array[bomber->pos_l - 1][bomber->pos_c] != OBSTACLE
        );
        //     && bomber->game->board_array[bomber->pos_l - 1][bomber->pos_c] != BOMBER1
        //     && bomber->game->board_array[bomber->pos_l - 1][bomber->pos_c] != BOMBER2
        // );
    } else if (direction == DOWN) {
        return (bomber->pos_l < bomber->game->lines_nb - 1
            && bomber->game->board_array[bomber->pos_l + 1][bomber->pos_c] != WALL
            && bomber->game->board_array[bomber->pos_l + 1][bomber->pos_c] != OBSTACLE
        );
        //     && bomber->game->board_array[bomber->pos_l + 1][bomber->pos_c] != BOMBER1
        //     && bomber->game->board_array[bomber->pos_l + 1][bomber->pos_c] != BOMBER2
        // );
    } else if (direction == RIGHT) {
        return (bomber->pos_c < bomber->game->columns_nb - 1
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c + 1] != WALL
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c + 1] != OBSTACLE
        );
        //     && bomber->game->board_array[bomber->pos_l][bomber->pos_c + 1] != BOMBER1
        //     && bomber->game->board_array[bomber->pos_l][bomber->pos_c + 1] != BOMBER2
        // );
    } else if (direction == LEFT) {
        return (bomber->pos_c > 0
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c - 1] != WALL
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c - 1] != OBSTACLE
        );
        //     && bomber->game->board_array[bomber->pos_l][bomber->pos_c - 1] != BOMBER1
        //     && bomber->game->board_array[bomber->pos_l][bomber->pos_c - 1] != BOMBER2
        // );
    }
}

/*
 * Move the bomber
 */
void move(bomber_t *bomber)
{
    bomber->prev_l = bomber->pos_l;
    bomber->prev_c = bomber->pos_c;

    if (bomber->pos_c == bomber->game->columns_nb - 1 && bomber->direction == RIGHT
        // && bomber->game->board_array[bomber->pos_l][0] != BOMBER1
        // && bomber->game->board_array[bomber->pos_l][0] != BOMBER2
        ) {
        bomber->pos_c = 0;
    } else if (bomber->pos_c == 0 && bomber->direction == LEFT
        // && bomber->game->board_array[bomber->pos_l][bomber->game->columns_nb - 1] != BOMBER1
        // && bomber->game->board_array[bomber->pos_l][bomber->game->columns_nb - 1] != BOMBER2
        ) {
        bomber->pos_c = bomber->game->columns_nb - 1;
    } else if (bomber->direction == UP) {
        if (can_move(bomber, UP)) {
            bomber->pos_l--;
        }
    } else if (bomber->direction == DOWN) {
        if (can_move(bomber, DOWN)) {
            bomber->pos_l++;
        }
    } else if (bomber->direction == RIGHT) {
        if (can_move(bomber, RIGHT)) {
            bomber->pos_c++;
        }
    } else if (bomber->direction == LEFT) {
        if (can_move(bomber, LEFT)) {
            bomber->pos_c--;
        }
    }

    bomber->game->board_array[bomber->prev_l][bomber->prev_c] = 0;
    bomber->game->board_array[bomber->pos_l][bomber->pos_c] = bomber->id;
}

/*
 * Place a bomb
 */
void place_bomb(bomber_t *bomber) 
{
    bomb_t bomb;
    bomb.pos_l = bomber->pos_l;
    bomb.pos_c = bomber->pos_c;
    bomb.range = bomber->bomb_n;
    bomb.timer = bomber->bomb_n * FPS;
    bomb.owner_id = bomber->id;

    if (bomber->game->bombs_count < MAX_BOMBS)
    {
        if (bomber->id == BOMBER1) {
            bomber->game->bomber1_stats.bombs_placed++;
        } 
        else if (bomber->id == BOMBER2) 
        {
            bomber->game->bomber2_stats.bombs_placed++;
        } 
        else 
        {   
            bomber->game->bomber1_stats.bombs_placed = 404;
            bomber->game->bomber2_stats.bombs_placed = 404;
            printf("Error: bomber id not recognized\n");
        }

        bomber->game->board_array[bomb.pos_l][bomb.pos_c] = BOMB;
        bomber->game->bombs_list[bomber->game->bombs_count] = bomb;
        bomber->game->bombs_count++;
    } else {
        printf("Too many bombs on the board\n");
    }
}

/*
 * Check the bombs: decrement the timer and explode the bomb if the timer is 0
 */
void check_bombs(game_t *game) 
{
    for (int i = 0; i < game->bombs_count; i++) {
        // redraw the bombs only if the bomber is not strait on it
        if (game->board_array[game->bombs_list[i].pos_l][game->bombs_list[i].pos_c] != BOMBER1
            && game->board_array[game->bombs_list[i].pos_l][game->bombs_list[i].pos_c] != BOMBER2) 
        {
            game->board_array[game->bombs_list[i].pos_l][game->bombs_list[i].pos_c] = BOMB;
        }

        game->bombs_list[i].timer--;
        
        if (game->bombs_list[i].timer == 0) {
            explode_bomb(game->bombs_list[i], game);
        }
    }
}

/*
 * Check the particles: decrement the timer and remove the particle if the timer is 0
 */
void check_particles(game_t *game) 
{
    for (int i = 0; i < game->particles_count; i++) {
        game->particles_array[i].timer--;

        if (game->particles_array[i].timer == 0) {
            game->board_array[game->particles_array[i].pos_l][game->particles_array[i].pos_c] = 0;

            // swap with last element of array and decrease array size
            game->particles_array[i] = game->particles_array[game->particles_count - 1];
            game->particles_count--;
        }
    }
}

/*
 Interpolate between two points
 */
float linear_interpolation(float x1, float y1, float x2, float y2, float t)
{
    return y1 + (y2 - y1) / (x2 - x1) * (t - x1);
}

/*
 * Create a linearly spaced array
 */
double *linspace(double a, double b, int n)
{
    double *array = malloc(n * sizeof(double));
    double step = (b - a) / (n - 1);

    for (int i = 0; i < n; i++)
        array[i] = a + step * i;

    return array;
}

/*
 Check if a point is in the explosion zone of a bomb
 */
int is_in_explode_zone(int pos_l, int pos_c, bomb_t bomb, game_t game)
{
    if (pow((pos_l - bomb.pos_l), 2) + pow((pos_c - bomb.pos_c), 2) <= pow(bomb.range, 2))
    {
        int x1 = bomb.pos_c;
        int y1 = bomb.pos_l;

        int x2 = pos_c;
        int y2 = pos_l;

        // if the line is vertical
        if (x1 == x2) {
            if (y1 < y2) {
                for (int i = y1; i <= y2; i++) {
                    if (game.board_array[i][x1] == WALL) {
                        return 0;
                    }
                }
            } else {
                for (int i = y1; i >= y2; i--) {
                    if (game.board_array[i][x1] == WALL) {
                        return 0;
                    }
                }
            }
        }
        // if the line is horizontal
        else if (y1 == y2) {
            if (x1 < x2) {
                for (int i = x1; i <= x2; i++) {
                    if (game.board_array[y1][i] == WALL) {
                        return 0;
                    }
                }
            } else {
                for (int i = x1; i >= x2; i--) {
                    if (game.board_array[y1][i] == WALL) {
                        return 0;
                    }
                }
            }
        }
        // if the line is diagonal
        else {
            int n = 0;
            int n1 = abs(x1-x2)+1;
            int n2 = abs(y1-y2)+1;

            if (n1 < n2) {
                n = n2;
            } else {
                n = n1;
            }

            double *linspace_array = linspace(x1, x2, n);

            if (x1 < x2) {
                for (int i = 0; i < n; i++) {
                    int x = round(linspace_array[i]);
                    int y = round(linear_interpolation(x1, y1, x2, y2, linspace_array[i]));

                    if (game.board_array[y][x] == WALL) {
                        return 0;
                    }
                }
            } else {
                for (int i = 0; i < n; i++) {
                    int x = round(linspace_array[i]);
                    int y = round(linear_interpolation(x1, y1, x2, y2, linspace_array[i]));

                    if (game.board_array[y][x] == WALL) {
                        return 0;
                    }
                }
            }
        }

        return 1;
    } else {
        return 0;
    }
}

/*
 * Get the index of a bomb in the bombs list
 */
int get_bomb_index(int pos_l, int pos_c, game_t game)
{
    for (int i = 0; i < game.bombs_count; i++) {
        if (game.bombs_list[i].pos_l == pos_l && game.bombs_list[i].pos_c == pos_c) {
            return i;
        }
    }

    return -1;
}

/*
 * Explode every obstacle in the range of the bomb in a circle
 */
void explode_bomb(bomb_t bomb, game_t *game)
{
    for (int row = 0; row < game->lines_nb; row++) 
    {
        for (int col = 0; col < game->columns_nb; col++) 
        {
            if (is_in_explode_zone(row, col, bomb, *game)) 
            {
                if (row != bomb.pos_l && col != bomb.pos_c && game->board_array[row][col] == BOMB) 
                {
                    explode_bomb(game->bombs_list[get_bomb_index(row, col, *game)], game);
                }
                else if (game->board_array[row][col] == BOMBER1)
                {
                    game->board_array[row][col] = 0;
                    game->is_over = 1;
                    game->winner = 2;
                }
                else if (game->board_array[row][col] == BOMBER2)
                {
                    game->board_array[row][col] = 0;
                    game->is_over = 1;
                    game->winner = 1;
                } 
                else if (game->board_array[row][col] == OBSTACLE)
                {
                    game->board_array[row][col] = 0;

                    if (bomb.owner_id == BOMBER1) {
                        game->bomber1_stats.obstacles_destroyed += 1;
                    } 
                    else if (bomb.owner_id == BOMBER2) 
                    {
                        game->bomber2_stats.obstacles_destroyed += 1;
                    }
                    else 
                    {   
                        game->bomber1_stats.bombs_placed = 404;
                        game->bomber2_stats.bombs_placed = 404;
                        printf("Error: bomber id not recognized\n");
                    }
                }

                game->board_array[row][col] = PARTICLE;
                game->particles_array[game->particles_count].pos_l = row;
                game->particles_array[game->particles_count].pos_c = col;
                game->particles_array[game->particles_count].timer = 0.5*FPS; // display the particle for 0.5 seconds
                game->particles_count++;
            }
        }
    }

    int i = get_bomb_index(bomb.pos_l, bomb.pos_c, *game);

    // remove the bomb from the game
    game->board_array[game->bombs_list[i].pos_l][game->bombs_list[i].pos_c] = 0;
    game->bombs_list[i] = game->bombs_list[game->bombs_count - 1];
    game->bombs_count--;
}

/*
 Calculate the score given the stats of a player
 */
float get_score(stats_t stats)
{
    if (stats.bombs_placed == 0) {
        return 0;
    }
    return (float) stats.obstacles_destroyed / (stats.bombs_placed * stats.bomb_n);
}

int init_serveur()
{
    int errno;
    int s = socket(AF_INET, SOCK_DGRAM, AF_UNSPEC);
    if (s == -1)
    {
        printf("Socket creation error %d\n", errno);
        exit(-1);
    }
    printf("The socket is identified by : %d \n", s);

    int taille = sizeof(struct sockaddr_in);
    struct sockaddr_in *padin;
    padin = (struct sockaddr_in *)(malloc(taille));
    bzero((char *)padin, taille);

    padin->sin_family = AF_INET;
    padin->sin_port = htons(PORT);

    int bd = bind(s, (struct sockaddr *)padin, taille);
    if (bd == -1)
    {
        printf("Error while binding %d\n", errno);
        exit(-1);
    }
    return s;
}

int init_client()
{
    int errno;
    int s = socket(AF_INET, SOCK_DGRAM, AF_UNSPEC);
    if (s == -1)
    {
        printf("Socket creation error %d\n", errno);
        exit(-1);
    }
    printf("The socket is identified by : %d \n", s);
    return s;
}

void *receive_data(int s, size_t data_size)
{
    void *msg = malloc(data_size);
    p_exp = (struct sockaddr_in *)(malloc(sizeof(struct sockaddr_in)));
    socklen_t p_lgexp = sizeof(struct sockaddr_in);
    int bd = recvfrom(s, msg, data_size, 0, (struct sockaddr *)p_exp, &p_lgexp);
    if (bd == -1)
    {
        free(msg);
        printf("Error while receiving\n");
        exit(-1);
    }
    // printf("Received %d bytes from %s:%d\n", bd, inet_ntoa(p_exp->sin_addr), ntohs(p_exp->sin_port));
    // printf("adresse IP du client: %s\n",inet_ntoa(p_exp->sin_addr));

    return msg;
}

// pour envoyer un texte vers un serveur dont l'adresse est connue
void send_data(int s, char *adresse, void *message, size_t data_size)
{
    struct sockaddr_in padin;
    struct hostent *host;

    int taille = sizeof(struct sockaddr_in);
    bzero((char *)&padin, taille);

    if ((host = gethostbyname(adresse)) == NULL)
    {
        perror("gethostbyname");
        exit(2);
    };
    bcopy(host->h_addr_list[0], &padin.sin_addr, host->h_length);

    padin.sin_family = AF_INET;
    padin.sin_port = htons(PORT);
    // printf("preparation a un envoi \n");
    int bd = sendto(s, message, data_size, 0, &padin, sizeof(padin));
    if (bd == -1)
    {
        printf("Error while sending\n");
        exit(-1);
    }
    // printf("Successfully sent %d bytes\n", bd);
}