#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define nums 128
#define highest_num 589
#define col_width 10
#define screen_length 600
#define screen_width  1280

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

typedef struct
{
    float x, y, width, height;
    bool is_pressed;
    const char* text;
} Button;

//drawing a button
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


// Function to merge two subarrays of arr[]
void merge(int arr[], int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temporary arrays
    int L[n1], R[n2];

    // Copy data to temporary arrays L[] and R[]
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    int i = 0; // Initial index of the first subarray
    int j = 0; // Initial index of the second subarray
    int k = left; // Initial index of merged subarray

    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}


void draw_array (int array[], int array_size)
{
    for (int i = 1; i <=array_size; i++)
    {
        al_draw_filled_rectangle(col_width*(i-1), screen_length, col_width*i-1,screen_length - array[i-1], al_map_rgb(255, 255, 255));
    }
}


void draw_subarrays(int array[],int left_start,int mid, int right_end)
{
    //left array
    for (int i = left_start; i <= mid ; i++)
    {
        al_draw_filled_rectangle(col_width*(i), screen_length, col_width*(i+1)-1,screen_length - array[i], al_map_rgb(0, 0, 255));
    }

    //right array
    for (int i = mid+1; i <= right_end; i++)
    {
        al_draw_filled_rectangle(col_width*(i), screen_length,col_width*(i+1)-1,screen_length - array[i], al_map_rgb(255, 0, 0));
    }

}

int main ()
{
    // Seed the random number generator with the current time
    srand(time(NULL));

    int to_sort[nums] = {};

    int rand_num;

    int array_size = sizeof(to_sort)/sizeof(int);
    int n = array_size; //TODO: redo

    //add random nums from 0 to highest_num
    for (int i=0; i <nums; i++)
    {
        rand_num = (rand() % highest_num);
        to_sort[i] = rand_num;
    }

    //ALLERGO related
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
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

    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT ev;

    bool running = true;
    bool done = false;
    float new_interval = 1.0;

    int mid =0;
    int right_end=0;

    int curr_size = 1;  // For current size of subarrays to be merged
    int left_start = 0; // For picking starting index of left subarray to be merged

    // Create buttons
    Button button_slower = {10, 0, 20, 20, false, "-"};
    Button button_faster = {30, 0, 20, 20, false, "+"};


    al_start_timer(timer);


    while (running)
    {
        //close button functionality
        al_wait_for_event(queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            running = false;
            done = true;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            redraw = true;

        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
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

        // Buttons speed up and slow down button functionality
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            if (button_faster.is_pressed && is_point_in_button(ev.mouse.x, ev.mouse.y, &button_faster))
            {
                new_interval = new_interval/2;
                al_set_timer_speed(timer, new_interval);

                printf("Timer interval changed to %.2f seconds\n", new_interval);
                button_faster.is_pressed = false;

            }
            else if (button_slower.is_pressed && is_point_in_button(ev.mouse.x, ev.mouse.y, &button_slower))
            {
                new_interval = new_interval*2;
                al_set_timer_speed(timer, new_interval);

                printf("Timer interval changed to %.2f seconds\n", new_interval);
                button_slower.is_pressed = false;
            }

        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            // Find ending point of left subarray. mid+1 is starting point of right
            mid = left_start + curr_size - 1;
            right_end = (left_start + 2*curr_size - 1 < array_size-1) ? left_start + 2*curr_size - 1 : array_size-1;

            al_clear_to_color(al_map_rgb(0, 0, 0));

            draw_array(to_sort,array_size);

            //draw currently compared subarrays
            if (left_start<array_size-1)
            {
                draw_subarrays(to_sort, left_start, mid,right_end);
            }

            //draw buttons
            draw_button(&button_faster);
            draw_button(&button_slower);

            // Flip display
            al_flip_display();
            redraw = false;

            //sorting logic
            // Merge subarrays arr[left_start...mid] & arr[mid+1...right_end]
            if (mid < right_end)
                merge(to_sort, left_start, mid, right_end);

            if (left_start < array_size-1)
            {
                left_start += 2*curr_size;
            }
            else
            {
                left_start=0;
                curr_size = 2*curr_size;

                //is sorted
                if (curr_size > array_size-1)
                {
                    running = false;
                    break;
                }
            }
        }

    }

    //show sorted array
    if(!done)
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        draw_array(to_sort,n);
        al_flip_display();
        al_rest(2.0);
    }

    printf("shutting down");
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 1;
}

