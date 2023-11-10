#include "Game.h"

// ALLEGRO FUNCTIONS

// function that initialize allegro 4.2
int initAllegro()
{
    // Initialize Allegro
    if (allegro_init() != 0)
    {
        allegro_message("Failed to initialize Allegro!");
        return 0;
    }

    // set the window title when in a GUI environment
    set_window_title("FastLaneFury");

    // true color mode
    set_color_depth(32); // Set the color depth

    // Set up a graphics mode
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, MY_SCREEN_W, MY_SCREEN_H, 0, 0) != 0)
    {
        allegro_message("Failed to set graphics mode!");
        return 0;
    }

    printf("OK: Allegro initialized\n");

    // Install the keyboard
    if (install_keyboard() < 0)
    {
        allegro_message("Failed to install keyboard!");
        return 0;
    }

    // Install the mouse
    if (install_mouse() < 0)
    {
        allegro_message("Failed to install mouse!");
        return 0;
    }

    printf("OK: Keyboard and mouse installed\n");

    // This bitmap will be used as the virtual screen
    buffer = create_bitmap(MY_SCREEN_W, MY_SCREEN_H);

    // init bitmap
    clear_to_color(buffer, makecol(0, 0, 0)); // Clear to black

    // init screen bitmap
    clear_to_color(screen, makecol(0, 0, 0)); // Clear to black

    background = create_bitmap(MY_SCREEN_W, MY_SCREEN_H);

    // create backgroud bitmap
    DrawBackgroundInBitmap();

    return 1;
}

// function that init the backgroung
void DrawBackgroundInBitmap()
{
    rectfill(background, 0, 0, MY_SCREEN_W, (MY_SCREEN_H / (LANE_NUMBER + 1)) * 4, makecol(188, 188, 188));     // road
    rectfill(background, 0, (MY_SCREEN_H / (LANE_NUMBER + 1)) * 4, MY_SCREEN_W, MY_SCREEN_H, makecol(0, 0, 0)); // terminal

    // draw dashed line
    int x1 = 0;
    int y1 = ((MY_SCREEN_H / (LANE_NUMBER + 1)));
    int x2 = MY_SCREEN_W;
    int y2 = ((MY_SCREEN_H / (LANE_NUMBER + 1)));

    int draw = 1;
    int segment_length = 20; // segment length
    int space_length = 20;   // space length

    while (x1 < x2)
    {
        if (draw)
        {
            line(background, x1, y1, x1 + segment_length, y2, LANECOLOR);
            line(background, x1, y1 * 2, x1 + segment_length, y2 * 2, LANECOLOR);
            line(background, x1, y1 * 3, x1 + segment_length, y2 * 3, LANECOLOR);
        }
        x1 += segment_length;
        x1 += space_length;
        draw = !draw;
    }

    char lane[3];
    for (int i = 0; i < 4; i++)
    {
        sprintf(lane, "%d", i);
        textout_ex(background, font, lane, 30, ((MY_SCREEN_H / (LANE_NUMBER + 1))) * i + 25, makecol(0, 0, 0), -1);
    }
}

// function to close allegro
void closeAllegro()
{
    // Destroy the buffer bitmap
    destroy_bitmap(buffer);

    // Destroy the background bitmap
    destroy_bitmap(background);

    // destroy all veicles bitmaps
    for (int i = 0; i < MAX_VEICLE_TYPE; i++)
    {
        destroy_bitmap(Veicles[i]);
    }

    // unistall display
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);

    // Uninstall the mouse
    remove_mouse();

    // Uninstall the keyboard
    remove_keyboard();

    // destroy mutex
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&supportMutex);

    // destroy shared list
    destroySharedList();

    // destroy support list
    destroySupportList();

    // Exit program
    allegro_exit();
}

// function that load all the graphics assets
int loadGraphicsAssets()
{
    int i;
    // load all cars bitmaps in folder Sprites
    for (i = 0; i < MAX_VEICLE_TYPE; i++)
    {
        char path[50];
        sprintf(path, "./Bitmap/VeicleBitmap/bitmap%d.bmp", i);
        Veicles[i] = load_bitmap(path, NULL);

        if (!Veicles[i])
        {
            fprintf(stderr, "ERROR: failed to load car bitmap %d!\n", i);
            return 0;
        }
    }
    printf("OK: Loaded all veicle bitmaps\n");

    for (i = 0; i < MAX_OTHER_BPM; i++)
    {
        char path[50];
        sprintf(path, "./Bitmap/OtherBitmap/bitmap%d.bmp", i);
        otherBmp[i] = load_bitmap(path, NULL);

        if (!otherBmp[i])
        {
            fprintf(stderr, "ERROR: failed to load other bitmap %d!\n", i);
            return 0;
        }
    }

    // load all fonts in folder Fonts
    /*    for (int i = 0; i < MAX_FONT; i++)
    {
        char path[50];
        sprintf(path, "./Fonts/font%d.pcx", i);
        fonts[i] = load_font(path, NULL, NULL);
        if (!fonts[i])
        {
            fprintf(stderr, "ERROR: failed to load font%d!\n", i);
            return 0;
        }
    }
    printf("OK: Loaded all fonts\n");
    */
    return 1;
}

// GRAPHICS FUNCTIONS

// funtion that clear the display
void clearDisplay()
{
    clear_to_color(buffer, makecol(0, 0, 0));
}

// function that flip the display
void flipDisplay()
{
    acquire_screen();
    blit(buffer, screen, 0, 0, 0, 0, MY_SCREEN_W, MY_SCREEN_H);
    vsync();
    release_screen();
}

// fucntion that draws info
void DrawInfo(pthread_mutex_t *mutex, struct SharedList *shared)
{
    char info[50];
    double speedKmH = 0;
    int x = 0;
    int y = 0;

    // print active veicles
    pthread_mutex_lock(mutex);
    sprintf(info, "Active Veicles: %d", shared->size);
    textout_ex(buffer, font, info, 20, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 20, makecol(255, 255, 255), -1);
    pthread_mutex_unlock(mutex);

    // print dead line missed
    sprintf(info, "Total Deadline Miss: %d", get_deadline_miss());
    textout_ex(buffer, font, info, 20, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 40, makecol(255, 255, 255), -1);

    if (selectedVeicle != -1)
    {
        // print info about the task with id
        sprintf(info, "Task: %d", selectedVeicle);
        textout_ex(buffer, font, info, 450, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 20, makecol(255, 255, 255), -1);
        sprintf(info, "Period: %d", task_get_period(selectedVeicle));
        textout_ex(buffer, font, info, 450, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 40, makecol(255, 255, 255), -1);
        sprintf(info, "Deadline: %d", task_get_deadline(selectedVeicle));
        textout_ex(buffer, font, info, 450, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 60, makecol(255, 255, 255), -1);
        sprintf(info, "Priority: %d", task_get_priority(selectedVeicle));
        textout_ex(buffer, font, info, 450, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 80, makecol(255, 255, 255), -1);

        // print info about the veicle with id
        pthread_mutex_lock(mutex);
        struct Node *current = shared->head;
        while (current != NULL)
        {
            if (current->id == selectedVeicle)
            {
                sprintf(info, "Veicle: %d", current->id);
                textout_ex(buffer, font, info, 650, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 20, makecol(255, 255, 255), -1);
                sprintf(info, "Type: %d", current->Veicle.veicle);
                textout_ex(buffer, font, info, 650, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 40, makecol(255, 255, 255), -1);
                sprintf(info, "Lane: %d", current->Veicle.lane);
                textout_ex(buffer, font, info, 650, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 60, makecol(255, 255, 255), -1);
                // speed in km/h
                speedKmH = (int)round(current->Veicle.speed * 3.6);
                sprintf(info, "Speed: %.2f", speedKmH);
                textout_ex(buffer, font, info, 650, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 80, makecol(255, 255, 255), -1);
                sprintf(info, "Acceleration: %.2f", current->Veicle.acceleration);
                textout_ex(buffer, font, info, 650, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 100, makecol(255, 255, 255), -1);
                sprintf(info, "Position: (%.2f, %.2f)", current->Veicle.pos.x, current->Veicle.pos.y);
                textout_ex(buffer, font, info, 650, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 120, makecol(255, 255, 255), -1);
                sprintf(info, "Steering Angle: %.2f", current->Veicle.steeringAngle);
                textout_ex(buffer, font, info, 800, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 20, makecol(255, 255, 255), -1);
                sprintf(info, "State %d", current->Veicle.state);
                textout_ex(buffer, font, info, 800, ((SCREEN_H / (LANE_NUMBER + 1))) * 4 + 40, makecol(255, 255, 255), -1);

                // DrawFOV(current->Veicle.pos.x, current->Veicle.pos.y, SMAX, current->Veicle.veicle);

                // Front sensor
                x = (current->Veicle.pos.x) * SCALE_FACTOR - 10;
                y = (current->Veicle.pos.y * SCALE_FACTOR) + (getVeicleHeight(current->Veicle.veicle) / 2);
                DrawLine(x, y, x - 150, y, FOVCOLOR);
                DrawPoint(x, y, SENSORCOLOR);

                // Right sensor
                x = (current->Veicle.pos.x * SCALE_FACTOR) + (getVeicleWidth(current->Veicle.veicle) / 2);
                y = (current->Veicle.pos.y * SCALE_FACTOR) - 10;
                DrawArch(x, y, 150, 190.0, 350.0, FOVCOLOR);
                DrawPoint(x, y, SENSORCOLOR);

                // Left sensor
                x = (((current->Veicle.pos.x) * SCALE_FACTOR) + (getVeicleWidth(current->Veicle.veicle) / 2));
                y = ((current->Veicle.pos.y * SCALE_FACTOR) + getVeicleHeight(current->Veicle.veicle)) + 10;
                DrawArch(x, y, 150, 10.0, 170.0, FOVCOLOR);
                DrawPoint(x, y, SENSORCOLOR);

                // Back sensor
                x = (((current->Veicle.pos.x) * SCALE_FACTOR) + getVeicleWidth(current->Veicle.veicle)) + 10;
                y = (((current->Veicle.pos.y) * SCALE_FACTOR) + (getVeicleHeight(current->Veicle.veicle) / 2));
                DrawLine(x, y, x + 50, y, FOVCOLOR);
                DrawPoint(x, y, SENSORCOLOR);

                break;
            }
            current = current->next;
        }
        pthread_mutex_unlock(mutex);
    }
}

// function that draws veicle
void DrawVeicle(double x, double y, int type)
{
    int xg = (int)round(x * SCALE_FACTOR); // conversion of x in pixel
    int yg = (int)round(y * SCALE_FACTOR); // conversion of y in pixel

    draw_sprite(buffer, Veicles[type], xg, yg);
}

// function that draws background
void DrawBackground()
{
    blit(background, buffer, 0, 0, 0, 0, MY_SCREEN_W, MY_SCREEN_H);
}

// function that draws mouse
void DrawMouse(int x, int y)
{
    circlefill(buffer, x, y, 5, makecol(255, 0, 0));
}

// function that draws line distance from the other veicle
void DrawDistance(double x, double y, double distance, double alpha)
{
    for (int i = 0; i < 10; i++)
    {
        line(buffer, x + i, y + i, x - (distance * cos(alpha)) + i, y - (distance * sin(alpha)) + i, LINECOLOR);
    }
}

// function that draws pause if the veicle is paused
void DrawPause()
{
    circlefill(buffer, 40, SCREEN_H - 40, 30, makecol(255, 255, 255));
    // make the pause sign 5 pixel more hight and in the left
    rectfill(buffer, 30, SCREEN_H - 60, 35, SCREEN_H - 20, makecol(0, 0, 0));
    rectfill(buffer, 45, SCREEN_H - 60, 50, SCREEN_H - 20, makecol(0, 0, 0));
}

// function that draws point where we need
void DrawPoint(int x, int y, int color)
{
    circlefill(buffer, x, y, 5, color);
}

// draw arch
void DrawArch(int x, int y, int radius, double startAngle, double endAngle, int color)
{
    // draw arch using line to fill it
    double angle;
    int px, py;

    // convert angle in rad
    startAngle = (startAngle * M_PI) / 180;
    endAngle = (endAngle * M_PI) / 180;

    for (angle = startAngle; angle <= endAngle; angle += 0.01)
    {
        px = x + radius * cos(angle);
        py = y + radius * sin(angle);
        line(buffer, x, y, px, py, color);
    }
}

// function that draws line where we need
void DrawLine(int x1, int y1, int x2, int y2, int color)
{
    for (int i = 0; i < 5; i++)
    {
        line(buffer, x1, y1 + i, x2, y2 + i, color);
        line(buffer, x1, y1 - i, x2, y2 - i, color);
    }
}

// VEICLE FUNCTIONS

// function that return the bitmap width of a veicle
int getVeicleWidth(int veicle)
{
    return Veicles[veicle]->w;
}

// function that return the bitmap height of a veicle
int getVeicleHeight(int veicle)
{
    return Veicles[veicle]->h;
}

// function that initialize veicle
void initVeicleState(struct VeicleState *state, struct VeicleStatistics *statistics, int veicle)
{
    state->veicle = rand() % MAX_VEICLE_TYPE;
    state->lane = 1;
    state->pos.x = (MY_SCREEN_W - 2) / SCALE_FACTOR;                                                     // in meter
    int margin = ((MY_SCREEN_H / (LANE_NUMBER + 1)) - ((Veicles[state->veicle]->h) * VEICLE_SCALE)) / 2; // margin in pixel
    state->pos.y = (((MY_SCREEN_H / (LANE_NUMBER + 1)) * state->lane) + margin) / SCALE_FACTOR;          // in meter
    state->steeringAngle = 0.0;                                                                          // in degree

    if (veicle == 0)
    {
        state->speed = 10.0;       // speed in ms
        state->acceleration = 0.0; // acceleration in ms^2
        statistics->maxSpeed = 10.0;
        statistics->maxAcceleration = 0.0;
        statistics->maxDeceleration = 0.0;
        statistics->minDistance = 0.0;
    }
    else if (veicle == 2)
    {
        state->speed = 40.0;       // speed in ms
        state->acceleration = 5.0; // acceleration in ms^2
        state->lane = 2;
        state->pos.x = (MY_SCREEN_W - 2) / SCALE_FACTOR;                                                     // in meter
        margin = ((MY_SCREEN_H / (LANE_NUMBER + 1)) - ((Veicles[state->veicle]->h) * VEICLE_SCALE)) / 2; // margin in pixel
        state->pos.y = (((MY_SCREEN_H / (LANE_NUMBER + 1)) * state->lane) + margin) / SCALE_FACTOR;          // in meter
        statistics->maxSpeed = 40.0;
        statistics->maxAcceleration = 0.0;
        statistics->maxDeceleration = 0.0;
        statistics->minDistance = 0.0;
    }
    else
    {
        state->speed = 15.0;       // speed in ms
        state->acceleration = 0.5; // acceleration in ms^2
        statistics->maxSpeed = 20.0;
        statistics->maxAcceleration = 1.5;
        statistics->maxDeceleration = -7.0;
        statistics->minDistance = 10.0;
    }
}

// function that returns the distance from the other veicle
double proximitySensor(double x, double y, int range, double alpha)
{

    // the sensor return the distance from the other veicle in meter and obtain position in pixel and range in pixel

    double distance = -1.0;
    int i = 0;
    int color;

    // convert alpha in rad
    alpha = (alpha * M_PI) / 180;

    // check if the range is in the limit
    if (range > SMAX)
    {
        range = SMAX;
    }

    // check for veicle in front
    for (i = SMIN; i < range; i += SSTEP)
    {
        color = getpixel(screen, x + (i * cos(alpha)), y + (i * sin(alpha)));                                                                                 // get color
        if (color != BGCOLOR && color != -1 && color != FOVCOLOR && color != CURSORCOLOR && color != LINECOLOR && color != LANECOLOR && color != SENSORCOLOR) // if there is a veicle
        {
            distance = (double)i / SCALE_FACTOR; // distance in meter
            break;
        }
    }
    return distance;
}

// USER FUNCTIONS

// function that return the index of the selected veicle
int getSelection(int x, int y, pthread_mutex_t *mutex, struct SharedList *shared)
{
    int selection = -1;

    // 1) check if the mouse is in the screen
    // 2) check if the mouse is on a veicle
    // 3) check if the mouse is on a button

    if (x > 0 && x < MY_SCREEN_W && y > 0 && y < MY_SCREEN_H) // check if mouse is in the screen
    {
        // check if the mouse is on a veicle
        pthread_mutex_lock(mutex);

        // check all the veicles
        struct Node *current = shared->head;
        while (current != NULL)
        {
            int xg = (int)round((current->Veicle.pos.x) * SCALE_FACTOR);
            int yg = (int)round((current->Veicle.pos.y) * SCALE_FACTOR);
            int width = ((Veicles[current->Veicle.veicle]->w) * VEICLE_SCALE);
            int height = (Veicles[current->Veicle.veicle]->h) * VEICLE_SCALE;

            if (x > xg && x < xg + width && y > yg && y < yg + height)
            {
                selection = VEICLE;
                selectedVeicle = current->id;
                break;
            }
            current = current->next;
        }

        pthread_mutex_unlock(mutex);

        // check if the mouse is on a button
    }

    return selection;
}

// function that return the index of the selected veicle
int getSelectedVeicle()
{
    return selectedVeicle;
}

// function that set the index of the selected veicle
void setSelectedVeicle(int id)
{
    selectedVeicle = id;
}

// function that return the index of the selected button
int getSelectedButton()
{
    return selectedButton;
}

// PAUSE AND RESUME FUNCTIONS

// function that pause all the veicles
void pauseVeicles(pthread_mutex_t *mutex, struct SharedList *shared)
{
    cleanSupportList();
    paused[0] = 1; // to inform grapfic task to draw pause
    pthread_mutex_lock(mutex);
    struct Node *current = shared->head;
    while (current != NULL)
    {
        paused[current->id] = 1; // set pause to 1
        printf("OK: Veicle %d paused\n", current->id);
        current = current->next;
    }
    pthread_mutex_unlock(mutex);
}

// function that resume all the veicles
void resumeVeicles(pthread_mutex_t *mutex, struct SharedList *shared)
{
    pthread_mutex_lock(mutex);
    paused[0] = 0;
    struct Node *current = shared->head; // current node
    while (current != NULL)
    {
        paused[current->id] = 0; // set pause to 0
        current = current->next;
    }
    pthread_mutex_unlock(mutex);
}

// function that check if the veicle is paused
int checkPause(int id)
{
    return paused[id];
}
