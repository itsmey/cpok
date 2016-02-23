#include <ncurses.h>
#include <string.h>

#include "defs.h"
#include "settings.h"

settings_t settings;

int get_new_value(int current, int inc, int min, int max) {
  int new_value = current + inc;

  if (new_value < min) return max;
  if (new_value > max) return min;

  return new_value;
}

char* ai_diff_to_str(ai_diff_t d) {
  switch (d) {
    case DUMB: return "DUMB"; break;
    case SMARTER: return "SMARTER"; break;
  }
  return "";
}

char* game_speed_to_str(game_speed_t s) {
  switch (s) {
    case SLOW: return "SLOW"; break;
    case FASTER: return "FASTER"; break;
    case FAST: return "FAST"; break;
  }
  return "";
}

void print_labels(WINDOW *w, int y, int x) {
   counter_t field;
   char buf[16];
   FOREACH_FIELD {
     (field == settings.selected) ? mvwprintw(w, y, x - 2, "*") :
                                    mvwprintw(w, y, x - 2, " ");
     field_label(field, buf);

     if (field == settings.selected) wattron(w, A_REVERSE);
     mvwprintw(w, y, x, buf);
     if (field == settings.selected) wattroff(w, A_REVERSE);
     y = y + 2;
   }
}

void print_values(WINDOW *w, int y, int x) {
   counter_t field;
   char buf[16];
   FOREACH_FIELD {
     field_value(field, buf);
     mvwprintw(w, y, x, "             ");
     mvwprintw(w, y, x, buf);
     y = y + 2;
   }
}

void settings_init(settings_t *s) {
  s->cash = INITIAL_CASH_DEF;
  s->blind = BIG_BLIND_DEF;
  s->max_bet = MAX_RAISE_DEF;
  s->ai_diff = AI_DIFF_DEF;
  s->speed = SPEED_DEF;
  s->selected = F_SPEED;
}

void settings_show() {
  WINDOW* w_settings;
  int max_y, max_x;
  char buf[255];
  bool_t done = FALSE;
  int c, old_c;

  LOG("%s\n", "Printing settings window");


  getmaxyx(stdscr, max_y, max_x);

  w_settings = newwin(W_SETTINGS_HEIGHT,
                  W_SETTINGS_WIDTH,
                  max_y / 2 -  W_SETTINGS_HEIGHT / 2,
                  max_x / 2 -  W_SETTINGS_WIDTH / 2);

  keypad(w_settings, TRUE);

  wbkgd(w_settings, COLOR_PAIR(CP_PCURRENT));
  box(w_settings, 0, 0);

  WPRINT(w_settings, 0, W_SETTINGS_WIDTH / 2, "%s", "Game settings");
  WPRINT(w_settings, W_SETTINGS_HEIGHT - 2, W_SETTINGS_WIDTH / 2, "%s",
    "Press RETURN to continue.");

  while (!done) {
    wrefresh(w_settings);
    print_labels(w_settings, 3, 5);
    print_values(w_settings, 3, 24);
    c = wgetch(w_settings);
    old_c = settings.selected;
    switch (c) {
      case KEY_DOWN:
        settings.selected =
          (old_c == FIELDS_COUNT - 1) ? 0 : (old_c % FIELDS_COUNT) + 1;
        break;
      case KEY_UP:
        settings.selected =
          (old_c == 0) ? FIELDS_COUNT - 1 : (old_c % FIELDS_COUNT) - 1;
        break;
      case KEY_LEFT:
        field_update(settings.selected, -1);
        break;
      case KEY_RIGHT:
        field_update(settings.selected, 1);
        break;
      case RETURN:
        done = TRUE;
        break;
    }
  }

  delwin(w_settings);
  refresh();
}

void field_label(field_t f, char* buf) {
  switch (f) {
    case F_SPEED: strcpy(buf, "Speed"); break;
    case F_AI_DIFF: strcpy(buf, "AI level"); break;
    case F_CASH: strcpy(buf, "Initial cash"); break;
    case F_BLIND: strcpy(buf, "Big blind"); break;
    case F_BET: strcpy(buf, "Max bet"); break;
  }
}

void field_value(field_t f, char* buf) {
  switch (f) {
    case F_SPEED: sprintf(buf, "< %s >", game_speed_to_str(settings.speed)); break;
    case F_AI_DIFF: sprintf(buf, "< %s >", ai_diff_to_str(settings.ai_diff)); break;
    case F_CASH: sprintf(buf, "< %u >", settings.cash); break;
    case F_BLIND: sprintf(buf, "< %u >", settings.blind); break;
    case F_BET: sprintf(buf, "< %u >", settings.max_bet); break;
  }
}

void field_update(field_t f, int sign) {
  switch (f) {
    case F_SPEED:
      settings.speed = get_new_value(settings.speed, 1*sign, SLOW, FAST);
      break;
    case F_AI_DIFF:
      settings.ai_diff = get_new_value(settings.ai_diff, 1*sign, DUMB, SMARTER);
      break;
    break;
    case F_CASH:
      settings.cash = get_new_value(settings.cash, 50*sign, 50, 10000);
      break;
    break;
    case F_BLIND:
      settings.blind = get_new_value(settings.blind, 2*sign, 2, 100);
      break;
    break;
    case F_BET:
      settings.max_bet = get_new_value(settings.max_bet, 20*sign, 20, 1000);
      break;
    break;
  }
}
