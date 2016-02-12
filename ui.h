#ifndef UI_H
#define UI_H

#include "game.h"
#include "player.h"

#define set_msg(Pos, F, ...) {\
  sprintf(game.msg[Pos], F, __VA_ARGS__);}

#define ui_refresh_msg(Pos, F, ...) {\
  sprintf(game.msg[Pos], F, __VA_ARGS__);\
  ui_refresh();}

void ui_init();
void ui_destroy();
void ui_refresh();
decision_t ui_selector(player_t* player);

#endif
