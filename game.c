// #include "jeu.h"
#include "utils.c"

#define FPS 10

pthread_mutex_t dmutex = PTHREAD_MUTEX_INITIALIZER;


//https://stackoverflow.com/questions/41928673/implementing-a-keypress-event-in-c-with-multiple-threads
void * get_keystrock(void *arg) 
{

    bombers_array_t * bombers = (bombers_array_t *)arg;
    bomber_t * bomber1 = bombers->bomber1;
    bomber_t * bomber2 = bombers->bomber2;

    char c;
    tcgetattr(0, &origtc);
    newtc = origtc;
    newtc.c_lflag &= ~ICANON;
    newtc.c_lflag &= ~ECHO;

    int count = 0;
    while(1) {
        if (kbhit()) {
            // flush the chars in the buffer to get the last one
            while (kbhit()) {
                tcsetattr(0, TCSANOW, &newtc);
                c = getchar();
                tcsetattr(0, TCSANOW, &origtc);
            }
            
            pthread_mutex_lock(&dmutex);
            if (c == bomber1->up) {
                bomber1->direction = UP;
            } else if (c == bomber1->down) {
                bomber1->direction = DOWN;
            } else if (c == bomber1->left) {
                bomber1->direction = LEFT;
            } else if (c == bomber1->right) {
                bomber1->direction = RIGHT;
            } 
            else if (c == bomber1->bomb) {
                place_bomb(bomber1);
            }

            if (c == bomber2->up) {
                bomber2->direction = UP;
            } else if (c == bomber2->down) {
                bomber2->direction = DOWN;
            } else if (c == bomber2->left) {
                bomber2->direction = LEFT;
            } else if (c == bomber2->right) {
                bomber2->direction = RIGHT;
            } 
            else if (c == bomber2->bomb) {
                place_bomb(bomber2);
            }
            pthread_mutex_unlock(&dmutex);
        } else {
            pthread_mutex_lock(&dmutex);
            printf("IDDLE");
            bomber1->direction = IDDLE;
            bomber2->direction = IDDLE;
            pthread_mutex_unlock(&dmutex);
        }

        move(bomber1);
        move(bomber2);

        sleep_ms(1.0 / FPS * 1000);
    }
}

int can_move(bomber_t * bomber, int direction) {
    // check if the next position is a wall or an obstacle or out of the board or a bomber
    if (direction == UP) {
        return (bomber->pos_l > 0 
            && bomber->game->board_array[bomber->pos_l - 1][bomber->pos_c] != WALL 
            && bomber->game->board_array[bomber->pos_l - 1][bomber->pos_c] != OBSTACLE
            && bomber->game->board_array[bomber->pos_l - 1][bomber->pos_c] != BOMBER1
            && bomber->game->board_array[bomber->pos_l - 1][bomber->pos_c] != BOMBER2);
    } else if (direction == DOWN) {
        return (bomber->pos_l < bomber->game->lines_nb - 1
            && bomber->game->board_array[bomber->pos_l + 1][bomber->pos_c] != WALL
            && bomber->game->board_array[bomber->pos_l + 1][bomber->pos_c] != OBSTACLE
            && bomber->game->board_array[bomber->pos_l + 1][bomber->pos_c] != BOMBER1
            && bomber->game->board_array[bomber->pos_l + 1][bomber->pos_c] != BOMBER2);
    } else if (direction == RIGHT) {
        return (bomber->pos_c < bomber->game->columns_nb - 1
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c + 1] != WALL
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c + 1] != OBSTACLE
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c + 1] != BOMBER1
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c + 1] != BOMBER2);
    } else if (direction == LEFT) {
        return (bomber->pos_c > 0
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c - 1] != WALL
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c - 1] != OBSTACLE
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c - 1] != BOMBER1
            && bomber->game->board_array[bomber->pos_l][bomber->pos_c - 1] != BOMBER2);
    }
}

void move(bomber_t *bomber)
{
    bomber->prev_l = bomber->pos_l;
    bomber->prev_c = bomber->pos_c;

    if (bomber->pos_c == bomber->game->columns_nb - 1 && bomber->direction == RIGHT
        && bomber->game->board_array[bomber->pos_l][0] != BOMBER1
        && bomber->game->board_array[bomber->pos_l][0] != BOMBER2) {
        bomber->pos_c = 0;
    } else if (bomber->pos_c == 0 && bomber->direction == LEFT
        && bomber->game->board_array[bomber->pos_l][bomber->game->columns_nb - 1] != BOMBER1
        && bomber->game->board_array[bomber->pos_l][bomber->game->columns_nb - 1] != BOMBER2) {
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

void place_bomb(bomber_t *bomber) 
{
    bomb_t bomb;
    bomb.pos_l = bomber->pos_l;
    bomb.pos_c = bomber->pos_c;
    bomb.range = bomber->bomb_n;
    bomb.timer = bomber->bomb_n * FPS;

    if (bomber->game->bombs_count < MAX_BOMBS)
    {
        bomber->game->board_array[bomb.pos_l][bomb.pos_c] = BOMB;
        bomber->game->bombs_list[bomber->game->bombs_count] = bomb;
        bomber->game->bombs_count++;
    } else {
        printf("Too many bombs on the board\n");
    }
}

void check_bombs(game_t *game) 
{
    for (int i = 0; i < game->bombs_count; i++) {
        game->bombs_list[i].timer--;
        
        if (game->bombs_list[i].timer == 0) {
            explode_bomb(i, game);
        }
    }
}

float linear_interpolation(float x1, float y1, float x2, float y2, float t)
{
    return y1 + (y2 - y1) / (x2 - x1) * (t - x1);
}

double *linspace(double a, double b, int n)
{
    double *array = malloc(n * sizeof(double));
    double step = (b - a) / (n - 1);

    for (int i = 0; i < n; i++)
        array[i] = a + step * i;

    return array;
}

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

            // free(linspace_array);
        }

        return 1;
    } else {
        return 0;
    }
}

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
void explode_bomb(int i, game_t *game)
{
    bomb_t bomb = game->bombs_list[i];

    for (int row = 0; row < game->lines_nb; row++) 
    {
        for (int col = 0; col < game->columns_nb; col++) 
        {
            if (is_in_explode_zone(row, col, bomb, *game)) 
            {
                if (row != bomb.pos_l && col != bomb.pos_c && game->board_array[row][col] == BOMB) 
                {
                    explode_bomb(get_bomb_index(row, col, *game), game);
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
                }

                game->board_array[row][col] = PARTICLE;
            }
        }
    }

    // remove the bomb from the game
    game->board_array[game->bombs_list[i].pos_l][game->bombs_list[i].pos_c] = 0;
    game->bombs_list[i] = game->bombs_list[game->bombs_count - 1];
    game->bombs_count--;
}

void *game_loop(void *arg) 
{
    game_t *game = (game_t *)arg;

    while(1) 
    {
        pthread_mutex_lock(&dmutex);

        check_bombs(game);

        if (game->is_over)
        {
            printf("Game over, palyer %d won\n", game->winner);

            pthread_mutex_unlock(&dmutex);
            break;
        } else {
            draw_game(*game);

            pthread_mutex_unlock(&dmutex);
            sleep_ms(1.0 / FPS * 1000);
        }
    }
}

void init_threads(bomber_t * bomber) {
    pthread_t anim, keyboard;
    pthread_create(&anim, NULL, game_loop, bomber);
    pthread_create(&keyboard, NULL, get_keystrock, bomber);
    pthread_join(anim, NULL);
}

int main() 
{
    game_t game;
    game.bombs_count = 0;

    bomber_t bomber1;
    bomber1.game = &game;
    bomber1.id = BOMBER1;
    bomber1.bomb_n = 2;
    bomber1.up = 'z';
    bomber1.down = 's';
    bomber1.left = 'q';
    bomber1.right = 'd';
    bomber1.bomb = ' ';

    bomber_t bomber2;
    bomber2.game = &game;
    bomber2.id = BOMBER2;
    bomber2.bomb_n = 2;
    bomber2.up = 'A'; // up arrow
    bomber2.down = 'B'; // down arrow
    bomber2.left = 'D'; // left arrow
    bomber2.right = 'C'; // right arrow
    bomber2.bomb = '*';

    bombers_array_t bombers;
    bombers.bomber1 = &bomber1;
    bombers.bomber2 = &bomber2;

	get_board("board.txt", &game);

    place_bomber(&bomber1);
    place_bomber(&bomber2);

    place_obstacles(&game);
    draw_game(game);

    // init_threads(&bomber);
    pthread_t anim, keyboard;
    pthread_create(&anim, NULL, game_loop, &game);
    pthread_create(&keyboard, NULL, get_keystrock, &bombers);
    
    // pthread_create(&keyboard2, NULL, get_keystrock, &bomber2);
    
    // init_threads(&bomber2);
    // pthread_create(&anim, NULL, game_loop, &bomber2);
    
    pthread_join(anim, NULL);
	return 0;
}
