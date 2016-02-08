#ifndef UI_H
#define UI_H

#include "game.h"
#include "player.h"

void ui_init();
void ui_destroy();
void ui_refresh();
void ui_refresh_msg(char* msg);
void ui_msg(char* msg, int n_line);
decision_t ui_selector(player_t* player);

#endif
