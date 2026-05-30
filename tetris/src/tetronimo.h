#ifndef TETRONIMO_H
#define TETRONIMO_H
#include <stdint.h>
typedef uint8_t u8;
typedef int8_t i8;
typedef uint8_t gfx_color_t;

#define TSIZE 4
#define MAX_ROT 4

// pieces[id][rotation][row][column]
extern u8 pieces[7][MAX_ROT][TSIZE][4];
#endif /* TETRONIMO_H */
