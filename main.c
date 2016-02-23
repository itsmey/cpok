#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "defs.h"
#include "game.h"
#include "card.h"
#include "tests.h"
#include "ui.h"

FILE* log_file;

int main() {

  srand(time(NULL));

  #ifdef TESTS
    test();
  #else
    game_start();
  #endif

  return 0;
}
