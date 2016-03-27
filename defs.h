#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>

#define RETURN 10

#define TRUE 1
#define FALSE 0

#define W_INFO_HEIGHT 10
#define W_INFO_WIDTH 40

#define RANKS "23456789TJQKA"
#define SUITS "hdcs"

#define RANK 0
#define SUIT 1

#define PLAYERS_COUNT 4
#define PLAYERS_NAMES {"Shurik", "Coward", "Fool", "Pro"}
#define PLAYERS_AI {FALSE, FALSE, FALSE, FALSE}
#define PLAYERS_AI_PARS /* aggr, rnds, bluf, tact */\
 {{0,  20, 20, 70},  /* Shurik */ \
  {20, 50, 30, 50},  /* Coward */ \
  {80, 40, 70, 20},  /* Fool */   \
  {30, 0,  15, 90}}  /* Pro */
#define PLAYERS_POSX {15, 50, 85, 50}
#define PLAYERS_POSY {40, 10, 40, 70}

#define TABLE_CENTER_X 50
#define TABLE_CENTER_Y 40

#define HAND_SIZE 2
#define TABLE_SIZE 5
#define POOL_SIZE (HAND_SIZE + TABLE_SIZE)
#define GLOBAL_POOL_SIZE (TABLE_SIZE + HAND_SIZE * PLAYERS_COUNT)

#define INITIAL_CASH_DEF 1000
#define BIG_BLIND_DEF 10
#define MAX_RAISE_DEF 160
#define AI_DIFF_DEF DUMB
#define SPEED_DEF SLOW

#define INITIAL_CASH settings.cash
#define BIG_BLIND settings.blind
#define SMALL_BLIND (BIG_BLIND / 2)
#define MAX_RAISE  settings.max_bet
#define AI_DIFF  settings.ai_diff

typedef enum {CHECK, CALL, BET, RAISE, FOLD} decision_t;

#define M_FOLD   "  Fold "
#define M_CHECK  " Check "
#define M_CALL   "  Call "
#define M_BET    "  Bet  "
#define M_RAISE  " Raise "

#define M1 0
#define M2 1

#define CP_PNAME    1
#define CP_PCURRENT 2
#define CP_LABEL    3
#define CP_CARD_CS  4
#define CP_CARD_DH  5

#define INIT_COLOR_PAIRS {\
      init_pair(CP_PNAME,    COLOR_WHITE, COLOR_YELLOW); \
      init_pair(CP_PCURRENT, COLOR_WHITE, COLOR_MAGENTA); \
      init_pair(CP_LABEL,    COLOR_BLACK, COLOR_WHITE);\
      init_pair(CP_CARD_CS,  COLOR_WHITE, COLOR_GREEN);\
      init_pair(CP_CARD_DH,  COLOR_WHITE, COLOR_RED);} \

#define FOR_EACH_PLAYER(i) for (i = 0; i < PLAYERS_COUNT; i++)

extern FILE* log_file;

#ifdef LOGGING
#define LOG(F, ...) {\
      log_file = fopen("log", "a");\
      fprintf(log_file, F, __VA_ARGS__);\
      fclose(log_file);}
#else
#define LOG(F, ...)
#endif

#define PRINT(y, x, F, ...) {\
      sprintf(buf, F, __VA_ARGS__);\
      mvwprintw(stdscr, y, x - strlen(buf) / 2, buf);}

#define WPRINT(w, y, x, F, ...) {\
      sprintf(buf, F, __VA_ARGS__);\
      mvwprintw(w, y, x - strlen(buf) / 2, buf);}

#define PRINT_ATTR(Attr, y, x, F, ...) {\
      attron(Attr);\
      sprintf(buf, F, __VA_ARGS__);\
      mvwprintw(stdscr, y, x - strlen(buf) / 2, buf);\
      attroff(Attr);}

typedef unsigned char bool_t;
typedef unsigned char byte_t;
typedef unsigned short counter_t;

#endif
