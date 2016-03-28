#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "defs.h"
#include "ui.h"
#include "game.h"
#include "obscene.h"

byte_t fill_choices(char** choices, player_t* player, size_t size);
void print_choices(int y, int x, char** choices, size_t size);
decision_t decode_choice(char* choice);

void print_msg(char* msg, int n_line) {
  const int start_x = 2;
  int max_y, max_x, y, x;

  getmaxyx(stdscr, max_y, max_x);
  y = max_y - n_line - 2;

  for (x = start_x; x <= max_x - start_x; ++x)
    mvaddch(y, x, ' ');

  mvprintw(y, 2, "%s", msg);
}

void print_card(int y, int x, card_t card, bool_t opened) {
  char buf[255];
  int color_pair;
  bool_t open_flag;

  #ifdef OPENCARDS
    open_flag = TRUE;
  #else
    open_flag = FALSE;
  #endif

  if (!opened && !open_flag) {
    color_pair = CP_CARD_HIDDEN;
    PRINT_ATTR(A_BOLD | COLOR_PAIR(color_pair), y, x, "%s", "  ");
  } else {
    color_pair = ((card[SUIT] == 'd') || (card[SUIT] == 'h')) ?
      CP_CARD_DH :
      CP_CARD_CS;

    PRINT_ATTR(A_BOLD | COLOR_PAIR(color_pair),
        y, x, "%c%c", card[RANK], card[SUIT]);
  }
}

void print_obscene(int y, int x) {
  char buf[255];
  byte_t random_phrase = rand() % (sizeof(phrases) / sizeof(char*));
  const char* phrase = phrases[random_phrase];

  PRINT_ATTR(A_BOLD, y, x, "*%s*", phrase);
}

void print_player_name(int y, int x, player_t* player) {
  char *button, buf[255];
  int color_pair;
  bool_t is_current;

  button = (player->is_dealer) ? "(D)" : "";
  is_current = (game.current == player);

  color_pair = (is_current) ? 2 : 1;
  if (is_current) print_obscene(y - 1, x);

  PRINT_ATTR(COLOR_PAIR(color_pair), y, x, "%s%s", player->name, button);
}

void print_panel(int x, int y) {
  const char panel[] = "*----------------------*";
  int l = strlen(panel);

  mvprintw(y, x - l / 2, panel);
  mvprintw(y+3, x - l / 2, panel);
  mvprintw(y+1, x - l / 2, "|");
  mvprintw(y+1, x + l / 2 - 1, "|");
  mvprintw(y+2, x - l / 2, "|");
  mvprintw(y+2, x + l / 2 - 1, "|");
}

void ui_init() {

  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);

  start_color();
  INIT_COLOR_PAIRS;
}

void ui_destroy() {
  getch();
  endwin();
}

void ui_refresh() {
  char buf[255];  /* for PRINT macros */
  counter_t i;
  byte_t revealed_cards;
  byte_t cor;  /* center correction */
  bool_t cards_opened;

  const int players_x[] = PLAYERS_POSX;
  const int players_y[] = PLAYERS_POSY;
  int max_y, max_x;
  int y, x;

  erase();
  box(stdscr, 0, 0);
  attron(COLOR_PAIR(3));
  mvprintw(1, 1, "Dirty C90 Poker");
  attroff(COLOR_PAIR(3));
  /*PRINT_ATTR(COLOR_PAIR(3), 1, 1, "%s", "Dirty C90 Poker");*/

  getmaxyx(stdscr, max_y, max_x);
  for (i = 0; i < PLAYERS_COUNT; i++) {
    y = max_y * players_y[i] / 100;
    x = max_x * players_x[i] / 100;
    print_player_name(y, x, &(game.players[i]));
    PRINT(y+1, x, " [ %u ]", game.players[i].cash);
    cards_opened = game.players[i].is_cards_opened || !game.players[i].is_ai;
    if (game.players[i].is_in_game) {
      PRINT(y+2, x, "%s", "-------");
      print_card(y+3, x-1, game.players[i].pocket[0], cards_opened);
      print_card(y+3, x+2, game.players[i].pocket[1], cards_opened);
      if (game.players[i].bet)
        PRINT(y+4, x, "bet: %u", game.players[i].bet);
    }
  }

  y = max_y * TABLE_CENTER_Y / 100;
  x = max_x * TABLE_CENTER_X / 100;

  switch (game.round) {
    case INIT: revealed_cards = 0; break;
    case PREFLOP: revealed_cards = 0; break;
    case FLOP: revealed_cards = 3; break;
    case TURN: revealed_cards = 4; break;
    case RIVER: revealed_cards = 5; break;
    case END: revealed_cards = 0; break;
  }
  cor = 3 * revealed_cards / 2 - 1;
  for (i = 0; i < revealed_cards; i++)
    print_card(y+1, x-cor+3*i, game.table[i], TRUE);
  PRINT(y+2, x, "bank: %u", game.bank);

  print_panel(x, y);

  print_msg(game.msg[M1], M1);
  print_msg(game.msg[M2], M2);
}

decision_t ui_selector(player_t* player) {
  const byte_t CHOICES = 4;
  char* choices[CHOICES];
  byte_t c_count;
  int c, old_c, y;
  bool_t done = FALSE;

  game.ui_choice = 0;
  y = getmaxy(stdscr) - 8;

  c_count = fill_choices(choices, player, CHOICES);
  print_choices(y, 3, choices, CHOICES);
  while (!done) {
    c = getch();
    old_c = game.ui_choice;
    switch (c) {
      case KEY_DOWN:
        game.ui_choice = (old_c == c_count - 1) ? 0 : (old_c % c_count) + 1;
        break;
      case KEY_UP:
        game.ui_choice = (old_c == 0) ? c_count - 1 : (old_c % c_count) - 1;
        break;
      case RETURN:
        done = TRUE;
        break;
    }
    print_choices(y, 3, choices, CHOICES);
  }

  LOG("choice is %s\n", choices[game.ui_choice]);
  return decode_choice(choices[game.ui_choice]);
}

byte_t fill_choices(char** choices, player_t* player, size_t size) {
  counter_t i;

  for (i = 0; i < size; i++)
    choices[i] = NULL;

  i = 0;
  choices[i] = M_FOLD;
  i++;
  if (can_check(player)) {
    choices[i] = M_CHECK;
    i++;
  }
  if (can_call(player)) {
    choices[i] = M_CALL;
    i++;
  }
  if (can_bet(player)) {
    choices[i] = M_BET;
    i++;
  }
  if (can_raise(player)) {
    choices[i] = M_RAISE;
    i++;
  }

  return i;
}

void print_choices(int y, int x, char** choices, size_t size) {
  counter_t i;
  /*LOG("printing menu y = %d x = %d choice = %d\n", y, x, game.ui_choice);*/
  for(i = 0; i < size; i++) {
    if (choices[i]) {
      if (game.ui_choice == i) {
        attron(A_REVERSE);
        mvprintw(y, x, "%s", choices[i]);
        attroff(A_REVERSE);
      } else
        mvprintw(y, x, "%s", choices[i]);
      y++;
    }
  }
}

decision_t decode_choice(char* choice) {
  /*LOG("%s|%s|%s|%s|%s\n", choice, M_CHECK, M_CALL, M_BET, M_RAISE);*/
  if (!strcmp(choice, M_CHECK)) return CHECK;
  if (!strcmp(choice, M_CALL)) return CALL;
  if (!strcmp(choice, M_BET)) return BET;
  if (!strcmp(choice, M_RAISE)) return RAISE;
  return FOLD;
}

void ui_sleep(unsigned int seconds) {
  refresh();
  sleep(seconds);
}

void ui_info_window(char* header, char* first_line, byte_t n_lines) {
  WINDOW* w_info;
  int max_y, max_x;
  counter_t i;
  char buf[255];
  char *line;

  LOG("%s\n", "Printing info window");
  getmaxyx(stdscr, max_y, max_x);

  w_info = newwin(W_INFO_HEIGHT,
                  W_INFO_WIDTH,
                  max_y / 2 -  W_INFO_HEIGHT / 2,
                  max_x / 2 -  W_INFO_WIDTH / 2);

  wbkgd(w_info, COLOR_PAIR(CP_PCURRENT));
  box(w_info, 0, 0);
  /*wborder(w_info, '|', '|', '-', '-', '+', '+', '+', '+');*/

  WPRINT(w_info, 0, W_INFO_WIDTH / 2, "%s", header);

  line = first_line;
  for (i = 0; i < n_lines; i++) {
    mvwprintw(w_info, 2 + i, 2, line);
    line += W_INFO_WIDTH - 2;
  }

  WPRINT(w_info, W_INFO_HEIGHT - 2, W_INFO_WIDTH / 2, "%s",
    "Press any key to continue.");

  wrefresh(w_info);

  wgetch(w_info);
  delwin(w_info);
  refresh();
}

void ui_wait_any_key() {
  getch();
}
