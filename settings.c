#include <ncurses.h>
#include <string.h>

#include "defs.h"
#include "settings.h"

settings_t settings;

void print_labels(WINDOW *w, int y, int x) {
   mvwprintw(w, y, x, "Game speed");
   mvwprintw(w, y+2, x, "AI difficulty");
   mvwprintw(w, y+4, x, "Initial cash");
   mvwprintw(w, y+6, x, "Big blind");
   mvwprintw(w, y+8, x, "Max bet");
}

void settings_init(settings_t s) {
  s.cash = INITIAL_CASH_DEF;
  s.blind = BIG_BLIND_DEF;
  s.max_bet = MAX_RAISE_DEF;
  s.ai_diff = AI_DIFF_DEF;
  s.speed = SPEED_DEF;
}

void settings_show() {
  WINDOW* w_settings;
  int max_y, max_x;
  char buf[255];

  LOG("%s\n", "Printing settings window");

  getmaxyx(stdscr, max_y, max_x);

  w_settings = newwin(W_SETTINGS_HEIGHT,
                  W_SETTINGS_WIDTH,
                  max_y / 2 -  W_SETTINGS_HEIGHT / 2,
                  max_x / 2 -  W_SETTINGS_WIDTH / 2);

  wbkgd(w_settings, COLOR_PAIR(CP_PCURRENT));
  box(w_settings, 0, 0);

  WPRINT(w_settings, 0, W_SETTINGS_WIDTH / 2, "%s", "Game settings");

  print_labels(w_settings, 3, 5);

  WPRINT(w_settings, W_SETTINGS_HEIGHT - 2, W_SETTINGS_WIDTH / 2, "%s",
    "Press any key to continue.");

  wrefresh(w_settings);

  wgetch(w_settings);
  delwin(w_settings);
  refresh();
}
