#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "defs.h"
#include "card.h"
#include "tests.h"
#include "ui.h"
#include "game.h"

FILE* log_file;

int main() {

  srand(time(NULL));

  #ifdef TESTS

  test();
  return 0;

  #else

  game_start();

  #endif

  return 0;
}
