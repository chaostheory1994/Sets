#define NUM_CARDS   81
#define NUM_INPLAY  12  
#define NUM_SETCARDS 3  

#define MAX_SET_BUFFER 30 /* just a guess */

/* use constants for card properties */
#define RED   0
#define GREEN 1
#define BLUE  2

#define DIAMOND   0
#define RECTANGLE 1
#define PEANUT    2

#define ONE     0   /* the card shows one object -- internally, the representation */
#define TWO     1   /* of the properties is still encoded from 0 to 2 like all the */
#define THREE   2   /* other properties though.  */

#define OPEN    0
#define STRIPE  1
#define SOLID   2

typedef struct _CARD {
  int  id;           /* this is not used, but might help with debugging */
  int  color;
  int  shape;
  int  num;
  int  pattern;
}CARD;

/* user feedback error code */
#define NORMAL 0
#define USRERR 1
#define SET    2
#define NOSET  3

/********************************************
 *
 * global variables -- shared data structure
 *
 ********************************************/
FILE *fp_puzzles;                    /* file pointer to input */

CARD deck[NUM_CARDS];
CARD *shuffled[NUM_CARDS];
int  iDeck_top;                      /* index of first unturned card in shuffled deck */
CARD *inplay[NUM_INPLAY];            /* ptrs to cards on table */
CARD *pc_user_set[NUM_SETCARDS];     /* ptrs to cards selected from player */

CARD *rgpc_sets[MAX_SET_BUFFER][NUM_SETCARDS];
CARD *rgpc_sets_found[MAX_SET_BUFFER][NUM_SETCARDS];
int  iSets_top, iSetsFound_top;
int  bPlayMode;
/********************************************
 *
 * declare functions
 *
 ********************************************/
void set_gen_deck();
void set_shuffle();
void set_next_game();

void set_reset_queue(CARD *pc_set[]) ;
int  set_full_queue(CARD *pc_set[]);
int  set_in_queue(CARD *pc, CARD*pc_set[]);
int  iter_queue(CARD *pc, CARD *pc_set[]); 
int  set_proc_one_input(int iSel) ;

int set_check_user_set() ;
int set_check(CARD *pc_set[], int bVerbose) ;
int set_check_attribute (int rgv[]) ;
int set_match_set();

void set_print_card(CARD *pc) ;
void set_defined_game(char *fname);
void set_read_next_game();
void set_load_game_file(char *fname) ;

int set_find_all();


