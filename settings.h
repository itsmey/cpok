#ifndef SETTINGS_H
#define SETTINGS_H

#include "defs.h"

#define W_SETTINGS_WIDTH 40
#define W_SETTINGS_HEIGHT 16

#define F_SPEED 0
#define F_AI_DIFF 1
#define F_CASH 2
#define F_BLIND 3
#define F_BET 4

#define FIELDS_COUNT 5

#define FOREACH_FIELD for(field = 0; field < FIELDS_COUNT; field++)

typedef enum {DUMB, SMARTER} ai_diff_t;
typedef enum {SLOW, FASTER, FAST} game_speed_t;
typedef byte_t field_t;

typedef struct settings_t {
  unsigned short cash;
  unsigned short blind;
  unsigned short max_bet;
  game_speed_t speed;
  ai_diff_t ai_diff;
  field_t selected;
} settings_t;

extern settings_t settings;

void settings_init(settings_t *s);
void settings_show();
void field_label(field_t f, char* buf);
void field_value(field_t f, char* buf);
void field_update(field_t f, int sign);


#endif
