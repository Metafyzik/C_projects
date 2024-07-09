
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define sqr_size 10

int moore_neighborhood[] = {sqr_size,sqr_size,  sqr_size,0, sqr_size,-sqr_size, 0,sqr_size, 0,-sqr_size, -sqr_size,sqr_size, -sqr_size,0, -sqr_size,-sqr_size};

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

// Button structure
typedef struct {
    float x, y, width, height;
    bool is_pressed;
    const char* text;
} Button;

// Function to draw a button
void draw_button(Button* button) {
    if (button->is_pressed) {
        al_draw_filled_rectangle(button->x, button->y, button->x + button->width, button->y + button->height, al_map_rgb(200, 0, 0));
    } else {
        al_draw_filled_rectangle(button->x, button->y, button->x + button->width, button->y + button->height, al_map_rgb(0, 0, 200));
    }
    al_draw_rectangle(button->x, button->y, button->x + button->width, button->y + button->height, al_map_rgb(0, 0, 0), 2);
    al_draw_text(al_create_builtin_font(), al_map_rgb(255, 255, 255), button->x + button->width / 2, button->y + button->height / 2 - 8, ALLEGRO_ALIGN_CENTER, button->text);
}

// Function to check if a point is inside a button
bool is_point_in_button(float px, float py, Button* button) {
    return px >= button->x && px <= button->x + button->width && py >= button->y && py <= button->y + button->height;
}

struct cell {
    bool alive;
    int x;
    int y;
    struct cell *next;
};

struct cell* add_new_head(int x,int y, struct cell *old_head) {

    struct cell* new_head = (struct cell* )malloc(sizeof(struct cell));
    new_head->x=x;
    new_head->y=y;
    new_head->alive=true;

    // first insert to the list
    new_head->next= old_head;

    return new_head;
}

void draw_cells (struct cell *head) {
    struct cell *cell_ptr = head;

    while (cell_ptr != NULL) {
        al_draw_filled_rectangle(cell_ptr->x, cell_ptr->y, cell_ptr->x + sqr_size, cell_ptr->y + sqr_size, al_map_rgb(0,128,0));
        cell_ptr = cell_ptr->next;
    }
}

bool check_in_linked_list(int x, int y, struct cell *head) {
    struct cell *cell_ptr = head;

    while (cell_ptr != NULL) {
        if (x==cell_ptr->x && y==cell_ptr->y) {
            return true;
        }
        cell_ptr = cell_ptr->next;
    }

    return false;
}

void remove_cell(int x,int y, struct cell **head) {
    struct cell *current = *head;
    struct cell *previous = *head;

    //linked list is empty
    if (*head == NULL) {
        printf("no cells in linked list");
    } else if (x==current->x && y==current->y) {
    //removing head
        *head = current->next;
        free(current);

    } else {
        while(!(x==current->x && y==current->y)) {

            previous = current;
            current = current->next;

        }
        previous->next = current->next;
        free(current);

    }
}

void survive(struct cell *head) {

    struct cell *cell_ptr_1 = head;
    struct cell *cell_ptr_2 = head;

    int live_cells = 0;
    int x, y;

    //loop through cells
    while (cell_ptr_1 != NULL) {
        x = cell_ptr_1->x;
        y = cell_ptr_1->y;

        //check living cells in Moore neighbourhood for each cell
        while (cell_ptr_2 != NULL) {

            //check living cells in Moore neighbourhood for each cell
            if (cell_ptr_2->x==x+sqr_size && cell_ptr_2->y==y)
                live_cells++;
            if (cell_ptr_2->x==x+sqr_size && cell_ptr_2->y==y-sqr_size)
                live_cells++;
            if (cell_ptr_2->x==x+sqr_size && cell_ptr_2->y==y+sqr_size)
                live_cells++;

            if (cell_ptr_2->x==x && cell_ptr_2->y==y+sqr_size)
                live_cells++;

            if (cell_ptr_2->x==x && cell_ptr_2->y==y-sqr_size)
                live_cells++;

            if (cell_ptr_2->x==x-sqr_size && cell_ptr_2->y==y)
                live_cells++;
            if (cell_ptr_2->x==x-sqr_size && cell_ptr_2->y==y-sqr_size)
                live_cells++;
            if (cell_ptr_2->x==x-sqr_size && cell_ptr_2->y==y+sqr_size)
                live_cells++;

            cell_ptr_2 = cell_ptr_2->next;

        }

        if (live_cells<2 || live_cells>3) {
            cell_ptr_1->alive=false;
            printf("to kill cell \n");
        }

        live_cells=0;
        cell_ptr_2=head;
        cell_ptr_1 = cell_ptr_1->next;

    }
}

void kill_cells(struct cell **head) {
    struct cell *current = *head;
    struct cell *previous = *head;

    //linked list is empty
    if (*head == NULL) {
        printf("no cells in linked list; kill_cells() \n");
    } else {

        if (current->alive==false) {
        //removing head
            printf("killing head \n");
            *head = current->next;
            free(current);
            current=*head;
        }

        while(current!=NULL) {

            if (current->alive==false){

                previous->next = current->next;
                free(current);
                current=previous->next;
            } else {
                previous = current;
                current = current->next;
            }

        }
    }
}



struct cell* spawn(struct cell *head) {
    // for each cell of Moore neigborhood of Mooreneighboorhod, check if not in the list already

    struct cell *ptr = head;
    struct cell *new_head = head;
    struct cell *cell_ptr_2 = head;

    int live_cells = 0;
    int x = 0;
    int y = 0;


    while(ptr!=NULL) {

        for (int i=0; i < 16; i=i+2) {
            x=ptr->x+moore_neighborhood[i];
            y=ptr->y+moore_neighborhood[i+1];


            if (!check_in_linked_list(x,y,new_head)) {
                //does "death" cell with coordinates x,y has three live cells in its moore neighborhood

                while (cell_ptr_2 != NULL) {

                    //check living cells in Moore neighbourhood for each cell
                    if (cell_ptr_2->x==x+sqr_size && cell_ptr_2->y==y)
                        live_cells++;
                    if (cell_ptr_2->x==x+sqr_size && cell_ptr_2->y==y-sqr_size)
                        live_cells++;
                    if (cell_ptr_2->x==x+sqr_size && cell_ptr_2->y==y+sqr_size)
                        live_cells++;

                    if (cell_ptr_2->x==x && cell_ptr_2->y==y+sqr_size)
                        live_cells++;

                    if (cell_ptr_2->x==x && cell_ptr_2->y==y-sqr_size)
                        live_cells++;

                    if (cell_ptr_2->x==x-sqr_size && cell_ptr_2->y==y)
                        live_cells++;
                    if (cell_ptr_2->x==x-sqr_size && cell_ptr_2->y==y-sqr_size)
                        live_cells++;
                    if (cell_ptr_2->x==x-sqr_size && cell_ptr_2->y==y+sqr_size)
                        live_cells++;


                    cell_ptr_2 = cell_ptr_2->next;

                }

                if (live_cells==3) {
                    printf("creating new cell,  live_cells %d \n",live_cells);
                    new_head = add_new_head(x,y,new_head);
                }

                live_cells=0;
                cell_ptr_2=head;

            }

        }

        ptr=ptr->next;

    }

    return new_head;
}

int main() {

    int screen_length = 600;
    int screen_width = 600;

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

    ALLEGRO_DISPLAY* disp = al_create_display(screen_length, screen_width);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    al_register_event_source(queue, al_get_keyboard_event_source()); //do I need this?
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    float new_interval = 0.5;
    bool redraw = true;
    ALLEGRO_EVENT ev;

    // Create a buttons
    Button button_slower = {10, 0, 20, 20, false, "-"};
    Button button_faster = {30, 0, 20, 20, false, "+"};

    //create a ptr to a cell
    struct cell *cell_ptr = NULL;//

    bool running = true;
    bool done = false;
    while (running) {

        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
           //intial stage: user adding cells
        } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int x = ev.mouse.x - ev.mouse.x % sqr_size;
            int y = ev.mouse.y - ev.mouse.y % sqr_size;
            //printf("Mouse clicked at: (%d, %d)\n", x, y);

            // Clear screen
            al_clear_to_color(al_map_rgb(0, 0, 0));

            //add or remove living cells
            if (check_in_linked_list(x,y,cell_ptr)) {
                //remove from the list
                remove_cell(x,y,&cell_ptr);
            } else {
                cell_ptr = add_new_head(x,y,cell_ptr);
            }

            //draw living cells
            draw_cells(cell_ptr);

            //draw button
            draw_button(&button_faster);
            draw_button(&button_slower);

            // Flip display
            al_flip_display();

        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            break;
        }
    }

    al_start_timer(timer);
    while (running) {

                al_wait_for_event(queue, &ev);

                //make close button work
                if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = false;
                    done = true;
                } else if (ev.type == ALLEGRO_EVENT_TIMER) {
                    redraw = true;
                } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {

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
                if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {

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

                if(redraw && al_is_event_queue_empty(queue)) {
                  //check if live cells does have 2 or 3 neighbour
                  survive(cell_ptr);

                  //check for condition for spawing new cells
                  cell_ptr = spawn(cell_ptr);

                  //get rid of cells with alive == false;
                  kill_cells(&cell_ptr);

                  // Clear screen
                  al_clear_to_color(al_map_rgb(0, 0, 0));

                  //draw living cells
                  draw_cells(cell_ptr);

                  //draw buttons
                  draw_button(&button_faster);
                  draw_button(&button_slower);

                  // Flip display
                  al_flip_display();
                  redraw = false;
                }
    }

    // Clean up resources
    al_destroy_font(font);
    al_destroy_event_queue(queue);
    al_destroy_display(disp);
    al_destroy_timer(timer);

    return 0;
}

