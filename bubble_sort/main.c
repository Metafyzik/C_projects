#include <stdio.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>


#define nums 100
#define highest_num 589
#define col_width 10
#define screen_length 600
#define screen_width 1000

// bubble sort algorithm visualization

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

// Button structure
typedef struct
{
    float x, y, width, height;
    bool is_pressed;
    const char* text;
} Button;

// Function to draw a button
void draw_button(Button* button)
{
    if (button->is_pressed)
    {
        al_draw_filled_rectangle(button->x, button->y, button->x + button->width, button->y + button->height, al_map_rgb(200, 0, 0));
    }
    else
    {
        al_draw_filled_rectangle(button->x, button->y, button->x + button->width, button->y + button->height, al_map_rgb(0, 0, 200));
    }
    al_draw_rectangle(button->x, button->y, button->x + button->width, button->y + button->height, al_map_rgb(0, 0, 0), 2);
    al_draw_text(al_create_builtin_font(), al_map_rgb(255, 255, 255), button->x + button->width / 2, button->y + button->height / 2 - 8, ALLEGRO_ALIGN_CENTER, button->text);
}

// Function to check if a point is inside a button
bool is_point_in_button(float px, float py, Button* button)
{
    return px >= button->x && px <= button->x + button->width && py >= button->y && py <= button->y + button->height;
}


print_array(int array[],int array_len) {

    for(int i =0; i<array_len;i++){
        printf("%d ",array[i]);

    }
    printf(" \n");
}


void draw_lengths(int array[], int array_length)
{
    for (int i = 1; i <=array_length; i++)
    {
        al_draw_filled_rectangle(col_width*(i-1), screen_length, col_width*i-1,screen_length - array[i-1], al_map_rgb(255, 255, 255));
    }
}

int main ()
{
    // Seed the random number generator with the current time
    srand(time(NULL));

    int to_sort[nums] = {};

    int rand_num;

    int array_length = sizeof(to_sort)/sizeof(int);

    //add random nums from 0 to highest_num
    for (int i=0; i <nums; i++)
    {
        rand_num = (rand() % highest_num);

        to_sort[i] = rand_num;
    }


    //ALLERGO related
    must_init(al_init(), "allegro");
    must_init(al_init_primitives_addon(),"primitives");
    must_init(al_install_mouse(),"mouse");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 1.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(screen_width,screen_length);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font(); //do I even use it?
    must_init(font, "font");


    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT ev;
    int i =0;
    int j =0;

    bool running = true;
    bool done = false;

    int temp = 0;

    float new_interval = 1.0;


    // Create a buttons
    Button button_slower = {10, 0, 20, 20, false, "-"};
    Button button_faster = {30, 0, 20, 20, false, "+"};

    al_start_timer(timer);
    while (running)
    {

        //make close button work
        al_wait_for_event(queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            //running = false;
            break;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER)
        {

            if (j==array_length)
            {
                running = false;
                break;
            }
            //printf("i=%d \n",i);

            redraw = true;
            if (i<array_length)
            {
                i++;
            }
            else
            {
                j++;
                i=1;
            }

            if (to_sort[i-1]>to_sort[i])
            {

                temp = to_sort[i-1];

                to_sort[i-1] = to_sort[i];
                to_sort[i] = temp;

            }

        }else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (is_point_in_button(ev.mouse.x, ev.mouse.y, &button_faster))
            {
                button_faster.is_pressed = true;
            }
            else  if (is_point_in_button(ev.mouse.x, ev.mouse.y, &button_slower))
            {
                button_slower.is_pressed = true;
            }

        }


        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            if (button_faster.is_pressed && is_point_in_button(ev.mouse.x, ev.mouse.y, &button_faster))
            {
                // Button click action
                new_interval = new_interval/2;
                al_set_timer_speed(timer, new_interval);

                printf("Timer interval changed to %.2f seconds\n", new_interval);
                button_faster.is_pressed = false;

            } else if (button_slower.is_pressed && is_point_in_button(ev.mouse.x, ev.mouse.y, &button_slower))
            {

                new_interval = new_interval*2;
                al_set_timer_speed(timer, new_interval);
                printf("Timer interval changed to %.2f seconds\n", new_interval);
                button_slower.is_pressed = false;
            }

        }

        if(redraw && al_is_event_queue_empty(queue))
        {

            // Clear screen
            al_clear_to_color(al_map_rgb(0, 0, 0));

            //draw lengths (columns)
            draw_lengths(to_sort,array_length);

            //draw currently compared
            al_draw_filled_rectangle(10*(i-1), screen_length, col_width*i-1,screen_length - to_sort[i-1], al_map_rgb(255, 0, 0));
            al_draw_filled_rectangle(10*(i), screen_length,  col_width*(i+1)-1,screen_length - to_sort[i], al_map_rgb(0, 0, 255));

            //draw buttons
            draw_button(&button_faster);
            draw_button(&button_slower);

            // Flip display
            al_flip_display();
            redraw = false;

        }

    }
    printf("shutting down");
    // Clean up resources
    al_destroy_timer(timer);
    al_destroy_display(disp);
    al_destroy_event_queue(queue);
    al_destroy_font(font);

    return 0;
}
