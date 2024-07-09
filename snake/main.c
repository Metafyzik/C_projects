#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define sqr_size 30

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int food_coordinate(int upper_bound_coordinate) {
    int rand_coordinate;
    rand_coordinate = (rand() % upper_bound_coordinate);
    rand_coordinate -= rand_coordinate%sqr_size;

    return rand_coordinate;
}

struct body {
    int x;
    int y;
    struct body *next;
};

void draw_snake(struct body *head) {
    struct body *node = head;

    while (node != NULL) {
        al_draw_filled_rectangle(node->x, node->y, node->x + sqr_size, node->y + sqr_size, al_map_rgb(255, 0, 0));
        node = node->next;
    }
}

void print_link_list(struct body *head){
    struct body *ptr = head;
    int count=0;

    while(ptr!=NULL) {
        count++;
        ptr=ptr->next;
    }
}

struct body* add_new_head(int x,int y, struct body *old_head) {

    struct body* new_head = (struct body* )malloc(sizeof(struct body));
    new_head->x=x;
    new_head->y=y;

    new_head->next= old_head;

    return new_head;
}

struct body* pop_tail(struct body *linked_list) {
    struct body *ptr = (struct body* )malloc(sizeof(struct body));
    ptr = linked_list;

    while(ptr->next->next!=NULL) {
        ptr=ptr->next;
    }

    free(ptr->next);
    ptr->next=NULL;
    return ptr;
}


struct food_obj {
    int x;
    int y;
};

struct body* create_body ()  {
    return (struct body* )malloc(sizeof(struct body));
};

void default_direction(int *head_dir_x,int *head_dir_y){
    int rand_num = rand() % 4;

    switch (rand_num) {
        case 0:
            *head_dir_x=-1;
            break;
        case 1:
            *head_dir_x=1;
            break;
        case 2:
            *head_dir_y=-1;
            break;
        case 3:
            *head_dir_y=1;
            break;
    }
}

bool collision_check(struct body *snake_head,int score) {
    struct body *ptr = snake_head->next;

    while(ptr!=NULL) {

        if (snake_head->x==ptr->x && snake_head->y==ptr->y) {
            return true;
        }
        ptr=ptr->next;
    }

    return false;
}




int main()
{
    // Seed the random number generator with the current time
    srand(time(NULL));

    int screen_length = 600;
    int screen_width = 600;

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 10.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(screen_length, screen_width);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    must_init(al_init_primitives_addon(), "primitives");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    must_init(al_install_audio(), "audio");
    must_init(al_init_acodec_addon(), "audio codecs");
    must_init(al_reserve_samples(16), "reserve samples");

    ALLEGRO_SAMPLE* snake = al_load_sample("snake.wav");
    must_init(snake, "snake");

    // Initialize the Font Addon and TTF Addon
    al_init_font_addon();  // Initialize the font addon
    al_init_ttf_addon();   // Initialize the ttf (TrueType Font) addon

    ALLEGRO_FONT *game_over_font = al_load_ttf_font("GEO_AI__.ttf", 50, 0);  // Change "arial.ttf" to the path of your TTF font

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    //create head
    struct body *head = (struct body* )malloc(sizeof(struct body));
    head->x = screen_length/2;
    head->y = screen_width/2;
    head->next=NULL;

    //create food
    struct food_obj food;
    food.x = food_coordinate(screen_width);
    food.y = food_coordinate(screen_length);

    int d = sqr_size;
    int score = 0;

    int head_dir_x = 0;
    int head_dir_y = 0;

    default_direction(&head_dir_x,&head_dir_y);

    struct body* snake_head = (struct body* )malloc(sizeof(struct body));
    snake_head->x=head->x;
    snake_head->y=head->y;
    snake_head->next=NULL;

    struct body* snake_tail = (struct body* )malloc(sizeof(struct body));
    snake_tail->x=head->x;
    snake_tail->y=head->y;
    snake_tail->next=NULL;
    struct body* new_body = NULL;

    ALLEGRO_KEYBOARD_STATE ks;

    al_start_timer(timer);

    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                if(event.keyboard.keycode == ALLEGRO_KEY_UP && (score==0 || head_dir_y!=1)) {
                    head_dir_y = -1;
                    head_dir_x = 0;
                } else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN && (score==0 || head_dir_y!=-1)) {
                    head_dir_y = 1;
                    head_dir_x = 0;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT && (score==0 || head_dir_x!=1)) {
                    head_dir_x = -1;
                    head_dir_y = 0;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT && (score==0 || head_dir_x!=-1) ) {
                    head_dir_x = 1;
                    head_dir_y = 0;
                }

                if(event.keyboard.keycode != ALLEGRO_KEY_ESCAPE)
                    break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            //movement
            head->x+= d*head_dir_x;
            head->y+= d*head_dir_y;

            //check out of bounds
            if (head->y> screen_length) {
                head->y =0;
            } else if (head->y < 0) {
                head->y =screen_length-sqr_size;
            } else if (head->x < 0) {
                head->x =screen_width-sqr_size;
            }else if (head->x > screen_width) {
                head->x =0;
            }

            snake_head = add_new_head(head->x,head->y,snake_head);

            //check food eaten
            if (head->x==food.x && head->y==food.y) {
                //position of a new food
                food.x = food_coordinate(screen_width);
                food.y = food_coordinate(screen_length);

                score++;
            } else {
               snake_tail = pop_tail(snake_head);
            }

            //collision detection
            if (collision_check(snake_head,score)) {
                al_draw_textf(game_over_font, al_map_rgb(255, 255, 255), screen_width/2, screen_length/2, ALLEGRO_ALIGN_CENTER, " GAME OVER: %d",  score);
                al_flip_display();
                al_play_sample(snake, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

                //Pause
                al_rest(6.0);
                break;
            }

            al_clear_to_color(al_map_rgb(0, 0, 0));
            //draw score
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %d Y: %d score: %d", head->x, head->y, score);

            //draw snake
            draw_snake(snake_head);

            //draw food
            al_draw_filled_rectangle(food.x, food.y, food.x + sqr_size, food.y + sqr_size, al_map_rgb(255,250,205));

            al_flip_display();
            redraw = false;
        }
    }

    printf("YOUR SCORE %d",score);

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_sample(snake);
    al_destroy_event_queue(queue);

    return 0;
}
