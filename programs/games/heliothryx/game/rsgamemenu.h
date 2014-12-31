#ifndef RS_GMENU_H
#define RS_GMENU_H

#include "rsgame.h"
#include "rs/rsplatform.h"


#define MENUS_COUNT     6

#define MENU_MAIN       0
#define MENU_SETTINGS   1
#define MENU_ABOUT      2
#define MENU_LEVEL_PASSED   3
#define MENU_GAME_OVER      4
#define MENU_PAUSE      5

#define MENU_ITEM_WINDOW_SCALE  1

extern char* menu_main_titles[];
extern char* menu_settings_titles[];
extern char* menu_about_titles[];
extern char **menu_titles[];
extern PRSFUNC0 menu_actions[];

extern char window_scale_str[];
extern char level_passed_score_str[];

void menu_cursor_down();
void menu_cursor_up();
void menu_open(int i);
void menu_cursor_click();

void menu_action_start();
void menu_action_exit();
void menu_action_change_window_scale();
void menu_action_resume();

#endif
