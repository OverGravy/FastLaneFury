#include "../libs/Graphics.h"

// periodic task function that perioducally updates the screen
void *graphicsTask(void *arg)
{

    int running = 1;
    struct Node *current;

    // gets the arguments
    struct argument GraphicsArg = get_task_argument(arg);

    int ti = get_task_index(arg);
    wait_for_activation(ti);

    while (running == 1)
    {
        // DRAW SCENE

        // clear display
        clearDisplay();

        // draw background
        DrawBackground();

        // draw info
        DrawInfo(GraphicsArg.mutex, GraphicsArg.shared);

        // draw veicles
        pthread_mutex_lock(GraphicsArg.mutex);

        if (GraphicsArg.shared->size > 0) // draw veicles only if there are veicles in the list
        {
            current = GraphicsArg.shared->head;
            while (current != NULL)
            {
                DrawVeicle(current->Veicle.pos.x, current->Veicle.pos.y, current->Veicle.veicle);
                current = current->next;
            }
        }
        pthread_mutex_unlock(GraphicsArg.mutex);

        // check for pause menu
        if (checkPause(1))
        {
            DrawConfigMenu();
        }

        // drawpause sinbol
        if (checkPause(0))
        {
            DrawPauseSymbol();
        }

        // draw mouse
        DrawMouse(mouse_x, mouse_y);

        // flip the display
        flipDisplay();

        // check for miss deadline
        if (deadline_miss(ti))
        {
            printf("GRAPHICS: deadline missed\n");
        }

        // wait for next activation
        wait_for_period(ti);
    }
    printf("OK: Graphics task terminated\n");
    return NULL;
}