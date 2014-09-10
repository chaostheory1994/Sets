#include <stdio.h>
#include <stdlib.h>

#define TRUE  1
#define FALSE 0

#define my_assert(X,Y) ((X)?(void) 0:(printf("error:%s in %s at %d", Y, __FILE__, __LINE__), myabort()))

/* function decl */
void myabort(void);
void my_setup();
void print_board() ;
void play_game() ;
int get_set();
