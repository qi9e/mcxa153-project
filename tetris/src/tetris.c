#include "tetronimo.h"
#include "settings.h"
#include <stdbool.h>

#include "gfx.h"

#include <stdlib.h>
#include <stdio.h>


#define WIDTH 10
#define HEIGHT 20

u8 board[HEIGHT][WIDTH] = {0};
u8 falling_type = 0;
u8 falling_rotation = 0;
i8 falling_x = 0;
i8 falling_y = 0;


// 128 x 160
#define RECT_HEIGHT 10
#define RECT_WIDTH 10

static bool
out_of_bounds(i8 x, i8 y) {
  return x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT;
}

static void
tetronimo_spawn(u8 x, u8 y, u8 type, u8 rotation) {
  falling_type = type;
  falling_x = x;
  falling_y = y;
  falling_rotation = rotation;
}
static int
tetris_remove_full_row(int y) {
  for (int x = 0; x < WIDTH; x++) {
    if (board[y][x] == 8) {
      return 0;
    }
  }

  for (int _y = y; _y > 0; _y--) {
    for (int x = 0; x < WIDTH; x++) {
      board[_y][x] = board[_y-1][x];
    }          
  }

  for (int x = 0; x < WIDTH; x++) {
    board[0][x] = 8;
  }
  return 1;
}


static void
tetris_draw() {
  int x = (0 * RECT_WIDTH);
  int y = (0 * RECT_HEIGHT);
  int w = (RECT_WIDTH);
  int h = (RECT_HEIGHT);
  gfx_color_t c = 15;
  GFX_DrawRect(x, y, w, h, c);
  
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (board[y][x] != 8) {
        c = board[y][x];
        x =  (x * RECT_WIDTH);
        y =  (y * RECT_HEIGHT);
        w =  (RECT_WIDTH);
        h =  (RECT_HEIGHT);
        GFX_DrawRect(x, y, w, h, c);
      }
    }    
  }

  for (int y = 0; y < TSIZE; y++) {
    for (int x = 0; x < TSIZE; x++) {
      gfx_color_t c = pieces[falling_type][falling_rotation][y][x];
      if (c > 0) {
        int posy = falling_y + y;
        int posx = falling_x + x;
        if (!out_of_bounds(posx, posy)) {
          x = (posx * RECT_WIDTH);
          y = (posy * RECT_HEIGHT);
          w = (RECT_WIDTH);
          h = (RECT_HEIGHT);
          GFX_DrawRect(x, y, w, h, c);
        }
      }
    }    
  }
}

static bool
check_collision() {
  for (int y = 0; y < TSIZE; y++) {
    for (int x = 0; x < TSIZE; x++) {
      if (pieces[falling_type][falling_rotation][y][x] != 0) {
        int real_x = falling_x + x;
        int real_y = falling_y + y;
        if (out_of_bounds(real_x, real_y)) {
          return true;
        } else {
          u8 v = board[real_y][real_x]; 
          if (v != 8) {
            return true;
          }                  
        }
      }
    }    
  }
  return false;
}

static void
falling_rotate() {
  u8 old = falling_rotation;
  falling_rotation = (falling_rotation + 1) % MAX_ROT;
  if (check_collision()) {
    falling_rotation = old;
  }
}

static int
falling_place() {
  for (int y = TSIZE - 1; y >= 0; y--) {
    int real_y = falling_y + y;
    for (int x = 0; x < TSIZE; x++) {
      u8 v = pieces[falling_type][falling_rotation][y][x];
      if (v != 0) {
        int real_y = falling_y + y;
        int real_x = falling_x + x;
        board[real_y][real_x] = v;
      }
    }
  }

  int rows_removed = 0;
  for (int y = 0; y < TSIZE; y++) {
    int real_y = falling_y + y;
    if (real_y < HEIGHT) {
      int row_removed = tetris_remove_full_row(real_y);
      rows_removed += row_removed;
    }
  }
  return rows_removed;
}


static int
tetris_tick() {
  int rows_removed = 0;
  falling_y += 1;
  if (check_collision()) {
    falling_y -= 1;
    rows_removed = falling_place();
    int rotation = rand() % MAX_ROT;
    int type = rand() % 7;
    tetronimo_spawn(0,0,type,rotation);
  }
  int points = 0;
  if (rows_removed == 1) {
    points = 100;
  } else if (rows_removed == 2) {
    points = 300;
  } else if (rows_removed == 3) {
    points = 500;
  } else if (rows_removed == 4) {
    points = 800;
  }
  return points;
}

static void
falling_move(int dir) {
  falling_x += dir;
  if (check_collision()) {
    falling_x -= dir;    
  }
}

static void
piece_rotate() {
  int last_rotation = falling_rotation;
  falling_rotation = (falling_rotation + 1) % MAX_ROT;
  if (check_collision()) {
    falling_rotation = last_rotation;
  }
}



static void
board_init() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      board[y][x] = 8;        
    }
  }
}

void
tetris_init()
{
  board_init();
  tetronimo_spawn(0, 0, 0, 0);
}


bool running = true;
unsigned int nframes = 0;
unsigned int last_time = 0;
unsigned int current_time = 0;
int points = 0;
int level = 1;

bool
tetris_input(char input)
{
  if (input == '1') {
    falling_rotate();
  }
  if (input == '2') {
    last_time = current_time;
    points += tetris_tick() * level;
  }
  if (input == '3') {
    falling_move(-1);
  }
  if (input == '4') {
    falling_move(1);
  }  
}
const int INPUT_DEBOUNCE = 10;

bool
tetris_update(char input)
{
  tetris_draw();
  current_time = nframes;
  if (current_time - last_time > 10) {
    last_time = current_time;
    points += tetris_tick() * level;      
  }
  nframes++;
}  


