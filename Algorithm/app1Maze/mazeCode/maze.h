typedef struct _tree tree;
typedef struct _maze maze;

maze *maze_random (int width, int height, int method);
void maze_svg (maze *maze, char *filename);

