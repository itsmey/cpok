#ifndef SETTINGS_H
#define SETTINGS_H

#define W_SETTINGS_WIDTH 40
#define W_SETTINGS_HEIGHT 16

typedef enum {DUMB, SMARTER} ai_diff_t;
typedef enum {SLOW, FASTER, FAST} game_speed_t;

typedef struct settings_t {
  unsigned short cash;
  unsigned short blind;
  unsigned short max_bet;
  game_speed_t speed;
  ai_diff_t ai_diff;
} settings_t;

extern settings_t settings;

void settings_init(settings_t s);
void settings_show();

#endif
