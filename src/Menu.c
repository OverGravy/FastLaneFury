#include "../libs/Menu.h"

// function that init menu configuration
void menu_init(int screen_w, int screen_h, int width, int height, int bg_color, int optNumber, struct MenuConfig *config)
{

    // init menu configuration
    config->screen_w = screen_w;
    config->screen_h = screen_h;
    config->width = width;
    config->height = height;
    config->bg_color = bg_color;
    config->optionNumber = optNumber;

    // calculate relative origin of the box
    config->RelativeX = (config->screen_w - config->width) / 2;
    config->RelativeY = (config->screen_h - config->height) / 2;
}

// function that draw menu
void menu_draw_box(struct MenuConfig *config, BITMAP *buffer)
{

    rectfill(buffer, config->RelativeX, config->RelativeY, config->RelativeX + config->width, config->RelativeY + config->height, config->bg_color);
}

// function that draws menu title
void menu_draw_title(char *title, int x, int y, struct MenuConfig *config, int color, BITMAP *buffer)
{
    textout_ex(buffer, font, title, config->RelativeX + x, config->RelativeY + y, color, -1);
}

// function that draws menu frame
void menu_draw_frame(struct MenuConfig *config, BITMAP *buffer, int color)
{
    rect(buffer, config->RelativeX, config->RelativeY, config->RelativeX + config->width, config->RelativeY + config->height, color);
    rect(buffer, config->RelativeX + 1, config->RelativeY + 1, config->RelativeX + config->width - 1, config->RelativeY + config->height - 1, color);
}

// function that draws menu options
void menu_draw_option(struct option *option, BITMAP *buffer)
{
    // draw option text
    textout_ex(buffer, font, option->text, option->pos.x, option->pos.y, makecol(255, 255, 255), -1);
    if (option->selected == 0)
    {
        // draw square
        rect(buffer, option->pos.x + 80, option->pos.y - 15, option->pos.x + 120, option->pos.y + 25, makecol(255, 255, 255));
    }
    else
    {
         // draw sqare not filled with a filled circle in the middle
        circlefill(buffer, option->pos.x + 100, option->pos.y + 5, 5, makecol(255, 255, 255));
        rect(buffer, option->pos.x + 80, option->pos.y - 15, option->pos.x + 120, option->pos.y + 25, makecol(255, 255, 255));
        
    }
}

// function that draw bit map inside the menu box
void menu_draw_bitmap(BITMAP *bitmap, int x, int y, struct MenuConfig *config, BITMAP *buffer)
{
    draw_sprite(buffer, bitmap, config->RelativeX + x, config->RelativeY + y);
}

// function that draws text inside the menu box
void menu_draw_text(char *text, int x, int y, struct MenuConfig *config, BITMAP *buffer)
{
    textout_ex(buffer, font, text, config->RelativeX + x, config->RelativeY + y, makecol(255, 255, 255), -1);
}