#include "maze.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "svg.h"

#include "stacks.h"

#define SCALE 8

struct vec4i {
    int x1;
    int y1;
    int x2;
    int y2;
};
typedef struct vec4i vec4i;

ARKSTACK_DEF(vec4i);

struct _tree {
    struct _maze* maze;

    int x1; int y1;
    int x2; int y2;

    int ex; int ey;
    int dir;

    struct _tree* left;
    struct _tree* right;

    struct _tree* exitRoom;
};

enum wallDirections {
    WALL_DOWN = 1,
    WALL_RIGHT = 2,
    WALL_UP = 4,
    WALL_LEFT = 8
};

struct _maze {
    int height; int width;
    int exit;

    int** cells;

    tree* solveTree;
};

maze* maze_init(int width, int height)
{
    maze* r = calloc(1, sizeof(maze));
    r->height = height;
    r->width = width;
    r->cells = calloc(height, sizeof(int*));
    for(int i = 0; i < height; i++)
        r->cells[i] = calloc(width, sizeof(int));
    
    for(int i = 0; i < height; i++)
        r->cells[i][width-1] |= WALL_RIGHT;
    for(int i = 0; i < width; i++)
        r->cells[height-1][i] |= WALL_DOWN;
    
    r->exit = rand()%((width*2 + height*2) - 4);

    return r;
}

static void get_maze_exit_xy(maze* maze, int* x, int* y) {
    if(maze->exit < maze->width) {
        *x = maze->exit;
        *y = 0;
        return;
    }
    maze->exit -= maze->width+1;
    if(maze->exit < maze->height) {
        *x = maze->width-1;
        *y = maze->exit;
        return;
    }
    maze->exit -= maze->height+1;
    if(maze->exit < maze->width) {
        *x = maze->width-1 - maze->exit;
        *y = maze->height-1;
        return;
    }
    maze->exit -= maze->width+1;
    if(maze->exit < maze->height) {
        *x = 0;
        *y = maze->height-1 - maze->exit;
        return;
    }
    *x = -1;
    *y = -1;
}

tree* tree_create()
{
    tree* r = calloc(1, sizeof(tree));
    if(!r)
        return NULL;
    return r;
}

int rect_contains_pt(tree* t, int px, int py)
{
    return (px >= t->x1) && (px < t->x2) && (py >= t->y1) && (py < t->y2);
}

typedef struct {
    tree** crossRef;
    int ex, ey;
} reqElem;

struct _list {
    struct _list* next;
    reqElem elem;
};

void recurse_bintree(tree* tree)
{
    int x1 = tree->x1, x2 = tree->x2;
    int y1 = tree->y1, y2 = tree->y2;

    if(x2-x1 == 1 || y2-y1 == 1)
        return;
    
    struct _tree* t1 = tree_create();
    struct _tree* t2 = tree_create();

    if (x2-x1 >= y2-y1) { // X side is bigger
        int split_offset = rand()%(x2-x1-1);
        int door_offset = rand()%(y2-y1);

        int exl = x1+split_offset, exr = x1+split_offset+1;
        int ey = y1+door_offset;

        t1->x1 = x1;                t1->y1 = y1; t1->x2 = x1+split_offset+1;    t1->y2 = y2; t1->ex = exl; t1->ey = ey; t1->dir = WALL_RIGHT;
        t2->x1 = x1+split_offset+1; t2->y1 = y1; t2->x2 = x2;                   t2->y2 = y2; t2->ex = exr; t2->ey = ey; t2->dir = WALL_LEFT;
    }
    else {
        int split_offset = rand()%(y2-y1-1);
        int door_offset = rand()%(x2-x1);

        int eyu = y1+split_offset, eyd = y1+split_offset+1;
        int ex = x1+door_offset;

        t1->x1 = x1; t1->y1 = y1;                   t1->x2 = x2; t1->y2 = y1+split_offset+1;    t1->ex = ex; t1->ey = eyu; t1->dir = WALL_DOWN;
        t2->x1 = x1; t2->y1 = y1+split_offset+1;    t2->x2 = x2; t2->y2 = y2;                   t2->ex = ex; t2->ey = eyd; t2->dir = WALL_UP;
    }
    if(rect_contains_pt(t1, tree->ex, tree->ey)) {
        t1->ex = tree->ex;
        t1->ey = tree->ey;
        tree->right = t1;
        tree->left = t2;
    }
    else {
        t2->ex = tree->ex;
        t2->ey = tree->ey;
        tree->right = t2;
        tree->left = t1;
    }
    recurse_bintree(tree->left);
    // Process requests ?
    recurse_bintree(tree->right);
}

void generate_bintree(maze* maze)
{
    tree* root = tree_create();
    root->x1 = 0; root->y1 = 0;
    root->x2 = maze->width; root->y2 = maze->height;
    maze->solveTree = root;
    recurse_bintree(root);
}

void generate_pure_random(maze* maze)
{
    for(int i = 0; i < maze->height; i++)
    for(int j = 0; j < maze->width; j++)
        maze->cells[i][j] |= rand()%4;
}

void generate_treeless(maze* maze, int x1, int y1, int x2, int y2)
{
    if(x2-x1 == 1 || y2-y1 == 1)
        return;
    if (x2-x1 >= y2-y1) { // X side is bigger
        int split_offset = rand()%(x2-x1-1);
        int door_offset = rand()%(y2-y1);
        for(int i = 0; i < (y2-y1); i++)
            if(i != door_offset)
                maze->cells[y1+i][x1+split_offset] |= WALL_RIGHT;
        generate_treeless(maze, x1, y1, x1+split_offset+1, y2);
        generate_treeless(maze, x1+split_offset+1, y1, x2, y2);
    }
    else {
        int split_offset = rand()%(y2-y1-1);
        int door_offset = rand()%(x2-x1);
        for(int i = 0; i < (x2-x1); i++)
            if(i != door_offset)
                maze->cells[y1+split_offset][x1+i] |= WALL_DOWN;
        generate_treeless(maze, x1, y1, x2, y1+split_offset+1);
        generate_treeless(maze, x1, y1+split_offset+1, x2, y2);
    }
}

void generate_treeless_iterative(maze* maze)
{
    arkstack(vec4i) stack = arkstack_create(vec4i);
    vec4i tmpVec = (vec4i){0, 0, maze->width, maze->height};
    arkstack_push(stack, tmpVec);
    while(arkstack_len(stack) > 0) {
        vec4i cur = arkstack_pop(stack);
        if(cur.x2-cur.x1 == 1 || cur.y2-cur.y1 == 1)
            continue;
        if (cur.x2-cur.x1 >= cur.y2-cur.y1) { // X side is bigger
            int split_offset = rand()%(cur.x2-cur.x1-1);
            int door_offset = rand()%(cur.y2-cur.y1);
            for(int i = 0; i < (cur.y2-cur.y1); i++)
                if(i != door_offset)
                    maze->cells[cur.y1+i][cur.x1+split_offset] |= WALL_RIGHT;
            tmpVec = (vec4i){cur.x1, cur.y1, cur.x1+split_offset+1, cur.y2};
            arkstack_push(stack, tmpVec);
            tmpVec = (vec4i){cur.x1+split_offset+1, cur.y1, cur.x2, cur.y2};
            arkstack_push(stack, tmpVec);
        }
        else {
            int split_offset = rand()%(cur.y2-cur.y1-1);
            int door_offset = rand()%(cur.x2-cur.x1);
            for(int i = 0; i < (cur.x2-cur.x1); i++)
                if(i != door_offset)
                    maze->cells[cur.y1+split_offset][cur.x1+i] |= WALL_DOWN;
            tmpVec = (vec4i){cur.x1, cur.y1, cur.x2, cur.y1+split_offset+1};
            arkstack_push(stack, tmpVec);
            tmpVec = (vec4i){cur.x1, cur.y1+split_offset+1, cur.x2, cur.y2};
            arkstack_push(stack, tmpVec);
        }
    }
}

void maze_from_bintree(maze* m, tree* tree)
{
    int x1 = tree->x1, x2 = tree->x2;
    int y1 = tree->y1, y2 = tree->y2;

    if(x2-x1 == 1 || y2-y1 == 1) { // leaf
        if (x2-x1 >= y2-y1) // X side is bigger
            for(int i = x1; i < x2-1; i++)
                m->cells[y1][i] = m->cells[y1][i] & (~WALL_RIGHT);
        else
            for(int i = y1; i < y2-1; i++)
                m->cells[i][x1] = m->cells[i][x1] & (~WALL_DOWN);
        return;
    }

    if (tree->left) {
        struct _tree* exitIndicator;
        struct _tree* entranceIndic;
        int dx, dy;
        if(tree->left->ex == tree->ex && tree->left->ey == tree->ey) {
            exitIndicator = tree->right;
            entranceIndic = tree->left;   
        }
        else {
            exitIndicator = tree->left;
            entranceIndic = tree->right;
        }
        dx = exitIndicator->ex;
        dy = exitIndicator->ey;
        switch(exitIndicator->dir) {
        case WALL_UP:
            dy--;
        case WALL_DOWN:
            m->cells[dy][dx] = m->cells[dy][dx] & (~WALL_DOWN);
            break;
        case WALL_LEFT:
            dx--;
        case WALL_RIGHT:
            m->cells[dy][dx] = m->cells[dy][dx] & (~WALL_RIGHT);
            break;
        default:
            break;
        }

        maze_from_bintree(m, tree->left);
        maze_from_bintree(m, tree->right);
    }
}

maze* maze_random (int width, int height, int method)
{
    maze* r = maze_init(width, height);
    switch(method)
    {
    case 0:
        generate_treeless(r, 0, 0, width, height);
        break;
    case 1:
        generate_treeless_iterative(r);
        break;
    case 2:
        generate_bintree(r);
        for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
            r->cells[i][j] = 3;
        maze_from_bintree(r, r->solveTree);
        break;
    default:
        generate_pure_random(r);
        break;
    }
    
    return r;
}

void maze_svg (maze *maze, char *filename)
{
    FILE* f = fopen(filename, "w");

    svg_header(f, (maze->width+1)*SCALE, (maze->height+1)*SCALE);

    svg_line(f, 0, 0, maze->width*SCALE, 0);
    //svg_line(f, maze->width*SCALE, 0, maze->width*SCALE, maze->height*SCALE);
    //svg_line(f, maze->width*SCALE, maze->height*SCALE, 0, maze->height*SCALE);
    svg_line(f, 0, maze->height*SCALE, 0, 0);
    set_svg_strokewidth("1.2");
    //svg_rect(f, 1, 1, maze->width*SCALE-1, maze->height*SCALE-1);

    int x, y;
    set_svg_color("red");
    get_maze_exit_xy(maze, &x, &y);
    svg_rect(f, x*SCALE, y*SCALE, (x+1)*SCALE, (y+1)*SCALE);

    // Down printing (x lines)
    int begin = -1;
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
            if (maze->cells[y][x] & WALL_DOWN) {
                if  (begin == -1) {
                    begin = x;
                }
            }
            else if (begin != -1) {
                svg_line(f, begin*SCALE, (y+1)*SCALE, (x)*SCALE, (y+1)*SCALE);
                begin = -1;
            }
        }
        if (begin != -1) {
            svg_line(f, begin*SCALE, (y+1)*SCALE, (maze->width)*SCALE, (y+1)*SCALE);
            begin = -1;
        }
    }
    // Right printing (y lines)
    for (int x = 0; x < maze->width; x++) {
        for (int y = 0; y < maze->height; y++) {
            if (maze->cells[y][x] & WALL_RIGHT) {
                if  (begin == -1) {
                    begin = y;
                }
            }
            else if (begin != -1) {
                svg_line(f, (x+1)*SCALE, begin*SCALE, (x+1)*SCALE, (y)*SCALE);
                begin = -1;
            }
        }
        if (begin != -1) {
            svg_line(f, (x+1)*SCALE, begin*SCALE, (x+1)*SCALE, (maze->height)*SCALE);
            begin = -1;
        }
    }
    svg_footer(f);

    fclose(f);
}

