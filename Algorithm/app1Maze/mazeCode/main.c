#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "maze.h"


int main(int argc, char **argv) {
  srand(time(NULL));
  if (argc < 4) {
    printf("Usage: %s <width> <height> <method>\n", *argv);
    return 1;
  }

  /* You need to define this function in "maze.c" */
  maze *m = maze_random(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

  /* And also this one, using the SVG drawing function provided in "svg.h" */
  maze_svg(m, "maze.svg");

  return 0;
}
