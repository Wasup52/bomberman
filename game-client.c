// #include "jeu.h"
#include "utils.c"

// #define FPS 10

pthread_mutex_t dmutex = PTHREAD_MUTEX_INITIALIZER;


//https://stackoverflow.com/questions/41928673/implementing-a-keypress-event-in-c-with-multiple-threads
void * get_keystrock(void *arg)
{

    bomber_t * bomber = (bomber_t *)arg;

    char c;
    tcgetattr(0, &origtc);
    newtc = origtc;
    newtc.c_lflag &= ~ICANON;
    newtc.c_lflag &= ~ECHO;

    int bd;
    char str_c[10];

    while(1) {
        if (kbhit()) {
            // flush the chars in the buffer to get the last one
            while (kbhit()) {
                tcsetattr(0, TCSANOW, &newtc);
                c = getchar();
                tcsetattr(0, TCSANOW, &origtc);
            }
           
            pthread_mutex_lock(&dmutex);
            // char tmp[2] = {c, '\0'};
            // strcpy(str_c, tmp);
            // strcpy(bomber->game->globals->sent_c, str_c);

            // printf("Sending: %s\n", str_c);
            // send_data(bomber->socket, "localhost", str_c, strlen(str_c));

            if (c == 'z') {
                bomber->direction = UP;
                // printf("UP\n");
            } else if (c == 's') {
                bomber->direction = DOWN;
                // printf("DOWN\n");
            } else if (c == 'q') {
                bomber->direction = LEFT;
                // printf("LEFT\n");
            } else if (c == 'd') {
                bomber->direction = RIGHT;
                // printf("RIGHT\n");
            }
            else if (c == ' ') {
                bomber->placed_bomb = 1;
                // place_bomb(bomber);
                // printf("-------- BOMB\n");
                // printf("BOMB\n");
            }
            pthread_mutex_unlock(&dmutex);
        } else {
            pthread_mutex_lock(&dmutex);
            // strcpy(str_c, "-1");
            // strcpy(bomber->game->globals->sent_c, str_c);

            // printf("Sending: %s\n", str_c);
            // send_data(bomber->socket, "localhost", str_c, strlen(str_c));
            bomber->direction = IDDLE;
            // printf("IDLE\n");
            pthread_mutex_unlock(&dmutex);
        }

        // move(bomber);

        // send_data(bomber->socket, "localhost", bomber->game, sizeof(game_t));


        // printf("sent key: %s\n", bomber->game->globals->sent_c);
        // move(bomber);

        sleep_ms(1.0 / FPS * 1000);
    }
}

void *game_loop(void *arg)
{
    bombers_array_t *bomers = (bombers_array_t *)arg;

    bomber_t *bomber1 = bomers->bomber1;
    bomber_t *bomber2 = bomers->bomber2;
    game_t *game = bomber2->game;

    // char *serv_key;

    while(1)
    {

        // printf("waiting for server key...\n");
        // serv_key = receive_data(game->socket, 2);
       
        game = receive_data(game->socket, sizeof(game_t));
        // printf("waiting for server game...\n");
        pthread_mutex_lock(&dmutex);

        bomber1->game = game;
        bomber2->game = game;

        move(bomber2);
        if (bomber2->placed_bomb) {
            place_bomb(bomber2);
            bomber2->placed_bomb = 0;
        }

        check_bombs(game);
        check_particles(game);

        // printf("sending game data...\n");
        send_data(game->socket, "localhost", bomber2->game, sizeof(game_t));

        if (game->is_over)
        {
            printf("Game over, player %d won\n", game->winner);

            send_data(game->socket, "localhost", game, sizeof(game_t));

            pthread_mutex_unlock(&dmutex);
            break;
        } else {
            draw_game(*game);
            print_scores(*game);
            // printf("Bomer1: %f, Bomber2: %f\n", get_score(bomber1), get_score(bomber2));
            // printf("sent key: %s\n", game->globals->sent_c);
            // printf("server key: %s\n", serv_key);

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
    int socket = init_client();

    int bomber_bomb_n;
    printf("n: ");
    scanf("%d", &bomber_bomb_n);

    printf("sending n to server...\n");
    send_data(socket, "localhost", &bomber_bomb_n, sizeof(int));

    int *serv_bomb_n = receive_data(socket, sizeof(int));
    printf("server n: %d\n", *serv_bomb_n);
   
    game_t game;
    game.bombs_count = 0;
    game.particles_count = 0;
    game.socket = socket;
    game.is_over = 0;

    bomber_t bomber1;
    bomber1.game = &game;
    bomber1.id = BOMBER1;
    bomber1.bomb_n = *serv_bomb_n;
    bomber1.placed_bomb = 0;
    bomber1.socket = socket;

    stats_t bomber1_stats;
    bomber1_stats.bombs_placed = 0;
    bomber1_stats.obstacles_destroyed = 0;
    bomber1_stats.bomb_n = *serv_bomb_n;

    game.bomber1_stats = bomber1_stats;

    bomber_t bomber2;
    bomber2.game = &game;
    bomber2.id = BOMBER2;
    bomber2.bomb_n = bomber_bomb_n;
    bomber2.placed_bomb = 0;
    bomber2.socket = socket;

    stats_t bomber2_stats;
    bomber2_stats.bombs_placed = 0;
    bomber2_stats.obstacles_destroyed = 0;
    bomber2_stats.bomb_n = bomber_bomb_n;

    game.bomber2_stats = bomber2_stats;

   
    get_board("board.txt", &game);

    // place_bomber(&bomber1);
    place_bomber(&bomber2);
    place_obstacles(&game);

    // char init_pos[10];
    // sprintf(init_pos, "%d;%d", bomber2.pos_l, bomber2.pos_c);
    // printf("sending initial position to server...\n");
    // send_data(socket, "localhost", init_pos, strlen(init_pos));

    printf("sending game to server...\n");
    send_data(socket, "localhost", &game, sizeof(game));

    char *serv_init_pos = receive_data(socket, 10);
    printf("server initial pos: %s\n", serv_init_pos);

    bomber1.pos_l = atoi(strtok(serv_init_pos, ";")); // strtok returns the first token
    bomber1.pos_c = atoi(strtok(NULL, ";")); // strtok returns the second token


    game.board_array[bomber1.pos_l][bomber1.pos_c] = BOMBER1;

    bomber1.game = &game;
    bomber2.game = &game;

    draw_game(game);
    print_scores(game);


    bombers_array_t bombers;
    bombers.bomber1 = &bomber1;
    bombers.bomber2 = &bomber2;

    pthread_t anim, keyboard;
    pthread_create(&anim, NULL, game_loop, &bombers);
    pthread_create(&keyboard, NULL, get_keystrock, &bomber2);
   
    pthread_join(anim, NULL);
    return 0;
}