// #include "jeu.h"
#include "utils.c"



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

            // // printf("Sending: %s\n", str_c);
            // bd = sendto(bomber->socket, str_c, strlen(str_c), 0, (struct sockaddr *)p_exp, sizeof(struct sockaddr_in));
            // if (bd == -1) {
            //     printf("Error while sending\n");
            // }
           
            if (c == 'z') {
                bomber->direction = UP;
            } else if (c == 's') {
                bomber->direction = DOWN;
            } else if (c == 'q') {
                bomber->direction = LEFT;
            } else if (c == 'd') {
                bomber->direction = RIGHT;
            }
            else if (c == ' ') {
                bomber->placed_bomb = 1;
                printf("-------- BOMB\n");
            }
            pthread_mutex_unlock(&dmutex);
        } else {
            pthread_mutex_lock(&dmutex);

            bomber->direction = IDDLE;
            
            pthread_mutex_unlock(&dmutex);
        }

        sleep_ms(1.0 / FPS * 1000);
    }
}

void *game_loop(void *arg)
{
    bombers_array_t *bomers = (bombers_array_t *)arg;

    bomber_t *bomber1 = bomers->bomber1;
    bomber_t *bomber2 = bomers->bomber2;
    game_t *game = bomber1->game;

    int bd;

    while(1)
    {

        pthread_mutex_lock(&dmutex);

        move(bomber1);
        if (bomber1->placed_bomb) {
            place_bomb(bomber1);
            bomber1->placed_bomb = 0;
        }

        check_bombs(game);
        check_particles(game);

        // printf("sending game data...\n");
        bd = sendto(game->socket, bomber1->game, sizeof(game_t), 0, (struct sockaddr *)p_exp, sizeof(struct sockaddr_in));
        if (bd == -1) {
            printf("Error while sending game data\n");
        }

        // printf("waiting for client game...\n");
        game = receive_data(game->socket, sizeof(game_t));

        bomber1->game = game;
        bomber2->game = game;

        if (game->is_over)
        {
            printf("Game over, player %d won\n", game->winner);

            bd = sendto(game->socket, game, sizeof(game_t), 0, (struct sockaddr *)p_exp, sizeof(struct sockaddr_in));
            if (bd == -1) {
                printf("Error while sending game data\n");
            }

            pthread_mutex_unlock(&dmutex);
            break;
        } else {
            draw_game(*game);
            print_scores(*game);

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
    int socket = init_serveur();
    int bd;

    int bomber_bomb_n;
    printf("n: ");
    scanf("%d", &bomber_bomb_n);

    printf("waiting for client...\n");
    int *client_bomb_n = receive_data(socket, 10);
    printf("client ip adress: %s\n", inet_ntoa(p_exp->sin_addr));
    printf("client n: %d\n", *client_bomb_n);

    printf("sending n to client...\n");
    bd = sendto(socket, &bomber_bomb_n, sizeof(int), 0, (struct sockaddr *)p_exp, sizeof(struct sockaddr_in));
    if (bd == -1) {
        printf("Error while sending\n");
    }

    bomber_t bomber1;
    bomber1.id = BOMBER1;
    bomber1.bomb_n = bomber_bomb_n;
    bomber1.placed_bomb = 0;
    bomber1.socket = socket;

    bomber_t bomber2;
    bomber2.id = BOMBER2;
    bomber2.bomb_n = *client_bomb_n;
    bomber2.placed_bomb = 0;
    bomber2.socket = socket;

    game_t *game_ptr = receive_data(socket, sizeof(game_t));
    game_t game = *game_ptr;

    bomber1.game = &game;
    bomber2.game = &game;

    place_bomber(&bomber1);

    char init_pos[10];
    sprintf(init_pos, "%d;%d", bomber1.pos_l, bomber1.pos_c);

    bd = sendto(socket, init_pos, strlen(init_pos), 0, (struct sockaddr *)p_exp, sizeof(struct sockaddr_in));
    if (bd == -1) {
        printf("Error while sending\n");
    }

    draw_game(*bomber1.game);
    print_scores(*bomber1.game);

    bombers_array_t bombers;
    bombers.bomber1 = &bomber1;
    bombers.bomber2 = &bomber2;

    pthread_t anim, keyboard;
    pthread_create(&anim, NULL, game_loop, &bombers);
    pthread_create(&keyboard, NULL, get_keystrock, &bomber1);
   
    pthread_join(anim, NULL);
    return 0;
}