/***********************************************************************
 *
 * This file contains all the functions to support the game's back-end.
 * Your graphical front-end should work with these functions.
 *
 * The main shared data structures between the front and back end are:
 * 1) CARD *inplay[NUM_INPLAY] -- an array of pointers to those
 *    cards currently chosen to be in play.
 * 2) pc_user_set[NUM_SETCARDS] -- a simple queue holding those cards that
 *    the user currently has selected.
 * 
 * The actual cards are stored in deck[NUM_CARDS] -- once created,
 * this is never modified.
 *
 * The card is represented as a struct with its properties as the struct's
 * fields.  Each property may take one of three values, all encoded as
 * an integer between 0 and 2.  This encoding is chosen so that it would
 * be easy to calculate where the card lives on the deck and to check
 * whether three cards form a set (by doing Base 3 arithmetic).
 *
 ***********************************************************************/

#include "main.h"
#include "sets.h"

/***********************************************************************
  These functions initialize the game:
  * set_gen_deck(): creates the deck
  * set_shuffle(): stores the randomized pointers to the deck in global variable shuffled[]
  * set_next_game(): sets up all the shared data structures for a new game.
     If it's in "demo mode" in which it was supplied a file containing pre-specified games,
     it will read the next game.  If the file reaches the end, it will be rewound to the 
     beginning.  
     If the program is in "play mode" it will get a new game from the shuffled cards.
     if too few cards remain, it will reshuffle.
 ***********************************************************************/
void set_gen_deck() {
  int i, c,s,n,p;

  i=0;
  for (c=0;c<NUM_SETCARDS;c++) 
    for (s=0;s<NUM_SETCARDS;s++) 
      for (n=0;n<NUM_SETCARDS;n++) 
	for (p=0;p<NUM_SETCARDS;p++) {
	  deck[i].id = i;
	  deck[i].color = c;    
	  deck[i].shape = s;
	  deck[i].num = n;
	  deck[i++].pattern = p;
	}
}
void set_shuffle() {
  int i,j, remains;
  CARD *tmp;
  
  /* initialize to serial order */
  for (i=0; i<NUM_CARDS; i++) 
    shuffled[i] = &(deck[i]);

  /* perform an in-place random permutation of all 
     elements of the shuffled array.  The algorithm
     is both known as the Knuth Shuffle and the
     Fisher and Yates Shuffle (Wikipedia has a reasonable
     blurb about it: en.wikipedia.org/wiki/Shuffle) */
  for (i=0, remains = NUM_CARDS; i<NUM_CARDS-1; i++, remains --) {
    j = (rand()%remains) + i;
    tmp = shuffled[j];
    shuffled[j] = shuffled[i];
    shuffled[i] = tmp;
  }
  iDeck_top = 0;
}


void set_next_game() {
  int i;

  if (bPlayMode) { /* get new game from deck */
    /* near end of deck; reshuffle */
    if ((iDeck_top + NUM_INPLAY) > NUM_CARDS) 
      set_shuffle();
    
    for (i=0; i<NUM_INPLAY; i++, iDeck_top++) 
      inplay[i] = shuffled[iDeck_top];
  }
  else {  /* get new game from file */
    set_read_next_game();
  }
  
  set_reset_queue(pc_user_set);
  
  set_find_all();
  if (iSets_top == 0)
    set_next_game();  /* move on to the next game if there's no set to find */
}

/***********************************************************************
   These functions update the queue holding user selected cards (pc_user_set[]).
   * set_reset_queue(pc_queue): resets the queue.
   * set_full_queue(pc_queue): whether the queue is full
   * set_in_queue(pc, pc_queue): returns whether pc points to a card in queue.
   * set_proc_one_input(int iSel): the input iSel is the card ID (numbered
     from 1 to 12, corresponding to the cards on the table)
     * if the selected card is in queue, remove it (user deselected the card)
     * if it's not in the queue, add it (user just selected the card)
       but if queue is full -- print a warning message and return error code
************************************************************************/
void set_reset_queue(CARD *pc_set[]) {
  int i;

  printf ("resetting set queue\n");
  for (i=0; i<NUM_SETCARDS; i++)
    pc_set[i] = NULL;
}

int set_full_queue(CARD *pc_set[]) {
  return (pc_set[NUM_SETCARDS-1]!=NULL);
}

int set_in_queue (CARD *pc, CARD *pc_set[]) {
  int ret = iter_queue(pc, pc_set);
  return (ret != NUM_SETCARDS) && (pc_set[ret] != NULL);
}

/* helper function that actually iterates through the queue.
   If the input card is found, the function returns the index
   to where the card is found.
   Otherwise, it returns the index to the first empty slot */
  
int iter_queue(CARD *pc, CARD *pc_set[]) {
  int i;

  for (i=0; i<NUM_SETCARDS; i++) {
    if (!pc_set[i])
      break;
    if (pc == pc_set[i])
      break;
  }
  return i;
}

/* check selected card against the queue of selected cards
   (global variable pc_user_set)
   if it's in the queue, delete it (user deselect)
   if it's not in the queue, add it.
   if queue already full -- do some error handling */
int set_proc_one_input(int iSel) {
  int i, j, iUserFeedbackCode;
  CARD *pc = inplay[iSel-1];

  iUserFeedbackCode = NORMAL;
  i = iter_queue(pc, pc_user_set);
  if (i >=NUM_SETCARDS ) {
    iUserFeedbackCode = USRERR;
    printf ("You've already picked three cards!\n");
  }
  else if (pc_user_set[i]) {
    /* delete it by moving everything down one slot */
    for (j=i; j<NUM_SETCARDS-1; j++) {
      pc_user_set[j] = pc_user_set[j+1];
    }
    pc_user_set[j] = NULL;
    printf ("deleted Card %d\n", iSel);
  }
  else  {    /* add it */
    pc_user_set[i] = pc;
    printf ("added Card %d\n", iSel);
  }
  return iUserFeedbackCode;
}

/*******************************************
 Functions for Set checking:
 * set_check_user_set(): interface with main when user thinks a set has been found.
   the return code can be one of the following:
      SET    (user found a set)
      NOSET  (user didn't find a set)
      USRERR (user didn't choose 3 cards)
 * set_check(pc_set, bVerbose): pc_set is a completed set, bVerbose tells
   the program how much messages to print out to the user.
 * set_check_attribute(rgv[]): function that checks whether the cards form a 
   set with respect to some particular attribute.
   It's a set if all values are same or all different.

   The values are encoded as consecutive numbers so set can be easily
   tested by checking whether there's any remainder when dividing the
   sum by 3 (assuming 3 cards make a set).
   
   all same: x*3 --> div by 3
   all diff: 0+1+2 = 3 --> div by 3

   two of one kind, one of another kind:
     x*2 + y, where x != y, and y can be at most 2 away from x:
     [if y is one away] 2x+(x+/-1) = 3x+/-1 --> not div by 3
     [if y is two away] 2x+(x+/-2) = 3x=/-2 --> not div by 3
***************/
int set_check_user_set() {
  int ret;

  if (set_full_queue(pc_user_set)) {
    ret = set_check(pc_user_set, TRUE);
    set_reset_queue(pc_user_set);
  }
  else {
    //printf ("You don't have enough cards to form a set\n");
    ret = USRERR;
  }
  return ret;
}

int set_check(CARD *pc_set[], int bVerbose){
  int i;
  int bSet = SET;
  int rgv[NUM_SETCARDS];
  
  for (i=0; i<NUM_SETCARDS;i++)
    rgv[i] = pc_set[i]->color;
  if (!set_check_attribute (rgv)) {
    if (bVerbose) 
      printf ("Not a set: Colors are not all same or all different\n");
    bSet = NOSET;
  }

  for (i=0; i<NUM_SETCARDS;i++)
    rgv[i] = pc_set[i]->shape;
  if (!set_check_attribute(rgv)) {
    if (bVerbose) 
      printf ("Not a set: Shapes are not all same or all different\n");
    bSet = NOSET;
  }

  for (i=0; i<NUM_SETCARDS;i++)
    rgv[i] = pc_set[i]->num;
  if (!set_check_attribute (rgv)) {
    if (bVerbose) 
      printf ("Not a set: Numbers are not all same or all different\n");
    bSet = NOSET;
  }

  for (i=0; i<NUM_SETCARDS;i++)
    rgv[i] = pc_set[i]->pattern;
  if (!set_check_attribute (rgv)) {
    if (bVerbose) 
      printf ("Not a set: Patterns are not all same or all different\n");
    bSet = NOSET;
  }
  
  if (bVerbose && (bSet==SET)) {
    /* make sure that this is a new set -- only do this when user is playing */
    bSet = set_match_set();
  }
  return bSet;
}

int set_check_attribute (int rgv[]) {
  int i;
  int sum = 0;
  for (i=0; i<NUM_SETCARDS;i++) {
    sum += rgv[i];
  }
  return (sum%NUM_SETCARDS == 0);
}


/* this function returns whether the set from the user 
 * matches any of the remaining solution sets
 */
int set_match_set() {
  int i,j,k, found;
  for (i=0; i<iSets_top; i++) {
    // check in each known set
    found = 0;
    for (j=0;j<NUM_SETCARDS;j++) { // for each card in user chosen set
      for (k=0; k<NUM_SETCARDS; k++) { // for each card in current set
	if (pc_user_set[j] == rgpc_sets[i][k])
	  found++;
      }
    }
    if (found == NUM_SETCARDS)
      break;
  }
  if (i == iSets_top) { /* not in soln set */
    printf ("You've already found this set before\n");
    return NOSET;
  }
  /* remove from soln set, add into found set */
  for (j=0; j<NUM_SETCARDS; j++) 
    rgpc_sets_found[iSetsFound_top][j]=  rgpc_sets[i][j] ;
  iSetsFound_top++;

  for (k=i; k<iSets_top-1; k++) 
    for (j=0; j<NUM_SETCARDS; j++) 
      rgpc_sets[k][j]=  rgpc_sets[k+1][j] ;
  iSets_top--;
  return SET;
}

/* brute-force: just check all possible permutations */
int set_find_all() {
  int i,j,k;
  CARD *pc_set[NUM_SETCARDS];

  iSets_top = 0;
  iSetsFound_top = 0;

  for (i=0; i<NUM_INPLAY; i++) {
    pc_set[0] = inplay[i];
    for (j=i+1; j<NUM_INPLAY; j++) {
      pc_set[1] = inplay[j];
      for (k=j+1; k<NUM_INPLAY; k++) {
	pc_set[2] = inplay[k];
	if (set_check(pc_set, FALSE) == SET) {
	  if (iSets_top >= MAX_SET_BUFFER) {
	    printf ("Warning: there are more than %d Sets", MAX_SET_BUFFER);
	    break;
	  }
	  /* add to record */
	  rgpc_sets[iSets_top][0] = pc_set[0];
	  rgpc_sets[iSets_top][1] = pc_set[1];
	  rgpc_sets[iSets_top][2] = pc_set[2];
	  iSets_top++;
	}
      }
    }
  }
  printf ("AIM: Find %d Sets\n", iSets_top);
  return iSets_top;
}

/***********************************************************************
 Some utility functions -- these are probably the least flexible part of
 this implementation.  The properties and the values they can take have
 been hard-coded in.

 * set_print_card(CARD *pc) prints the info on card pointed by pc
 * set_load_game_file(char *fname) sets up the file pointer to the pre-defined
   games if we started the program in "demo" mode.

 ***********************************************************************/
void set_print_card(CARD *pc) {
  char ch;

  switch (pc->color) {
  case RED: ch = 'R'; break;
  case GREEN: ch = 'G'; break;
  case BLUE: ch = 'B'; break;
  default: my_assert(TRUE,"unexpected color value\n");
  }
  printf ("%c",ch);

  switch (pc->shape) {
  case DIAMOND: ch = 'D'; break;
  case RECTANGLE: ch = 'T'; break;
  case PEANUT: ch = 'P'; break;
  default: my_assert(TRUE,"unexpected shape value\n");
  }
  printf ("%c",ch);

  printf ("%d", pc->num+1);

  switch (pc->pattern) {
  case SOLID: ch = 'S'; break;
  case STRIPE: ch = 'L'; break;
  case OPEN: ch = 'O'; break;
  default: my_assert(TRUE,"unexpected pattern value\n");
  }
  printf ("%c", ch);
}


void set_load_game_file(char *fname) {
  fp_puzzles = fopen (fname,"r");
  my_assert(fp_puzzles, "can't open puzzle file");
}

/*********************************
 * This implementation of file IO is
 * a bit sloppy, so it's up to 
 * the user to do quality assurance 
 * on the input puzzle file
 ************************************/
int get_card_id(char *buffer) {
  int c,s,n,p;

  switch(buffer[0]) {
  case 'R': c = RED; break;
  case 'G': c = GREEN; break;
  case 'B': c = BLUE; break;
  default: my_assert(1,"bad color in file");
  }

  switch(buffer[2]) {
  case 'D': s = DIAMOND; break;
  case 'T': s = RECTANGLE; break;
  case 'P': s = PEANUT; break;
  default: my_assert(1,"bad shape in file");
  }

  switch(buffer[4]) {
  case '1': n = ONE; break;
  case '2': n = TWO; break;
  case '3': n = THREE; break;
  default: my_assert(1,"bad num in file");
  }

  switch(buffer[6]) {
  case 'S': p = SOLID; break;
  case 'L': p = STRIPE; break;
  case 'O': p = OPEN; break;
  default: my_assert(1,"bad pattern in file");
  }
  return p + n*3 + s*9 + c*27;  
}

void set_read_next_game() {
  char buffer[100];
  int iCard;
  int i = 0;
  
  while (fgets(buffer, 100, fp_puzzles)) {
    if (buffer[0] == '#')
      continue;
    iCard = get_card_id(buffer);
    inplay[i++] = &(deck[iCard]);
    if (i >= NUM_INPLAY)
      break;
  }
  if (feof(fp_puzzles)) { 
    /* go back to the top */
    rewind(fp_puzzles);
  }
}

