/***********************************************************************
 *  
 *  An ascii version of a card game called Sets.
 *  For students of CS1566 [Homework #1]
 * 
 *  Although this is a completed stand-alone program, 
 *  this version is not user-friendly. The interface is
 *  difficult to use, and there is only a minimal amount 
 *  of user-friendly functionalities and
 *  user-input error checking/handling.  
 *
 *  You may use the parts of the program that handle the
 *  internal representations of the game, but you will 
 *  have to add a user-friendly graphical front-end interface.  
 *
 *  Rebecca Hwa
 ***********************************************************************/

#include "main.h"
#include "sets.h"

void myabort(void) {
    abort();
    exit(1); /* exit so g++ knows we don't return. */
} 

int main(int argc, char **argv) {
  setbuf(stdout, NULL);   /* for writing to stdout asap */
  my_setup(argc, argv);
  play_game();
}

void my_setup(int argc, char **argv) {

  /* if there's an input file, use it to get
     predefined game configurations */
  if (argc >= 2) {
    bPlayMode = FALSE;
    set_load_game_file(argv[1]);
  }
  else {
    bPlayMode = TRUE;
  }
  set_gen_deck();
  set_shuffle();
  set_next_game();

  return;
}

void play_game() {
  print_board();
  while (TRUE) {
    if (!get_set()) {
      printf ("Thanks for playing!\n");
      break;
    }
  }
}

void print_board() {
  int i;
  for (i=0; i<NUM_INPLAY; i++) {
    printf ("(%2d): ", i+1);
    set_print_card(inplay[i]);
    printf ("\n");
  }
  printf ("\n");
  
  printf ("Select three cards with attributes that are either all same or all different.\n");
  printf ("Pick cards one at a time (by typing in the number next to the card)\n");
  printf ("Once 3 cards have been chosen, type 0 to check whether you found a Set\n");
  printf ("Type -1 to quit\n");
}

/* asks for user input -- user must enter a number */
int get_set() {
  char buffer[100];
  int i;

    while (TRUE) {
        printf ("Card number?> ");
        scanf ("%s",buffer);
        if (sscanf (buffer, "%d",&i))
            break;
        else printf ("Bad input (not a number). Please re-select: ");
    }

  if (i==-1) 
    return FALSE;
  else if (i == 0) 
    set_check_user_set();
  else {
    /* do some basic error check */
    if ((i <= 0) || (i >NUM_INPLAY))
      printf ("Bad input (not a valid card number).  Please re-select: ");
    else set_proc_one_input(i);
  }
  return TRUE;
}
