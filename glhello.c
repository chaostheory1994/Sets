#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sets.h"

#define TRUE  1
#define FALSE 0

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#define my_assert(X,Y) ((X)?(void) 0:(printf("error:%s in %s at %d", Y, __FILE__, __LINE__), myabort()))



#define DELTA_TIME 10  /* defined to be 10 msec */

void myabort(void);
void glut_setup(void) ;
void gl_setup(void) ;
void my_setup(int argc, char **argv);
void my_display(void) ;
void my_reshape(int w, int h) ;
void my_keyboard(unsigned char key, int x, int y);
void my_mouse_drag(int x, int y);
void my_mouse(int button, int state, int mousex, int mousey);
void my_idle(void);
void play_game(void);
void init_game(void);
void draw_card(CARD*);

// Global Variables
int width;
int height;
int margin_width;
int margin_height;
int needs_init;
int guess;
int found;
int total;
double ratio;



void myabort(void) {
	abort();
	exit(1); /* exit so g++ knows we don't return. */
}



int main(int argc, char **argv) {
    
	/* General initialization for GLUT and OpenGL
     Must be called first */
	glutInit( &argc, argv ) ;

	/* we define these setup procedures */
	glut_setup() ;
	gl_setup() ;
	my_setup(argc, argv);

	/* go into the main event loop */
	glutMainLoop() ;

	return(0) ;
}

/* This function sets up the windowing related glut calls */
void glut_setup(void) {

	/* specify display mode -- here we ask for a double buffer and RGB coloring */
	glutInitDisplayMode (GLUT_DOUBLE |GLUT_RGB);

	/* make a 888,640 window with the title of "GLUT Skeleton" placed at the top left corner */
	glutInitWindowSize(888,640);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Sets");
	width = 888;
	height = 640;

	/*initialize callback functions */
	glutReshapeFunc( my_reshape );
	glutMouseFunc( my_mouse);
	glutMotionFunc( my_mouse_drag);
	glutKeyboardFunc (my_keyboard);
	glutIdleFunc( play_game );


	return ;
}

/* This function sets up the initial states of OpenGL related enivornment */
void gl_setup(void) {

	/* specifies a background color: black in this case */
	glClearColor(0,0,0,0) ;

	/* setup for simple 2d projection */
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	/* set the viewable portion  */
	// Since the original window size is this size
	// We will draw with respect to that.
	gluOrtho2D(0, 880.0, 0, 640.0);
	return ;
}

void my_setup(int argc, char **argv) {
    // Copied from main.c
    /* if there's an input file, use it to get
     predefined game configurations */
    needs_init = 1;
    guess = 0;
    ratio = 880.0/640.0;
  if (argc >= 2) {
    bPlayMode = FALSE;
    set_load_game_file(argv[1]);
  }
  else {
    bPlayMode = TRUE;
  }
    glLineWidth(2.0);
	return;
}

void my_reshape(int w, int h) {
	/* define viewport -- x, y, (origin is at lower left corner) width, height */
	// Now we much change the viewpoint to keep the same aspect ratio as 888/640.
	double curr_ratio = (double)w/(double)h;
	printf("%1.2f, %1.2f\n", curr_ratio, ratio);
	if(curr_ratio < ratio){
        // This means that the widths will line up.
        int view_height = h * ((double)w/880.0);
        margin_width = 0;
        margin_height = (h - view_height)/2;
        glViewport (0, margin_height, w, view_height);
	}
	else if(curr_ratio > ratio){
        // This is where the heights match and the width gets margins.
        int view_width = w * ((double)h/640.0);
        margin_width = (w - view_width)/2;
        margin_height = 0;
        glViewport(margin_width, 0, view_width, h);
	}
	else {
        //This is where the ratios are the same.
        margin_width = 0;
        margin_height = 0;
        glViewport(0,0,w,h);
	}

	width = w;
	height = h;
}

/* Sample keyboard callback function.
 This allows the user to quit the program
 by typing the letter q.
 */
void my_keyboard( unsigned char key, int x, int y ) {

	switch( key ) {
		case 'q':
		case 'Q':
			exit(0) ;
			break ;
		default: break;
	}
	return ;
}

/* currently doesn't do anything */
void my_mouse_drag(int x, int y) {
	return;
}

/* stubbed but doesn't do anything either
 (mousex, mousey) are in screen coordinates
 that is, (0,0) is at the upper left corner
 */
void my_mouse(int button, int state, int mousex, int mousey) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        // Converts mouse coordinates to orthos coordinates.
        int x = ((double)mousex/(double)width)*880;
        // Also will invert y to start from bottom left corner
        int y = 640 - ((double)mousey/(double)height)*640;

        // Now we will figure out which card the player just clicked on.
        // This will be a similar calculation to where the cards were placed.
        // (i%4)*200+54), ((i/4)*160 + 90)
        int cardX = ((x-54)/200);
        int cardY = ((y-90)/160);
        int cardnum = cardX + (cardY*4);
        // Now with the x and y card value, we can easily check to see if the mouse was exactly in the bounds.
        // Did the mouse click somewhere in the border?
        if(x >= 54 && y >= 90 && x <= 834 && y <= 550){
            // Did the mouse actually click on the card or in between 2 cards?
            if(x <= (cardX*200)+(180+54) && y <= (cardY*160)+(140+90)) set_proc_one_input(cardnum+1);
        }
    }

}

void my_display(void) {
	/* clear the buffer */
	glClear(GL_COLOR_BUFFER_BIT) ;
    if(guess == 0){
        glColor3f(0, 0, 0);
    }
    else if(guess == 1){
        glColor3f(0, 1, 0);
        guess = 0;
    }
    else if(guess == 2){
        glColor3f(1, 0, 0);
        guess = 0;
    }

    /* draw something */
    glBegin(GL_POLYGON);
    glVertex2f(0.0, 0.0);
    glVertex2f(0.0, 640.0);
    glVertex2f(888.0, 640.0);
    glVertex2f(888.0, 0.0);
    glEnd();

    // This is where we draw the cards.
    for(int i = 0; i < NUM_INPLAY; i++){
        //First thing to do is draw the background.
        glColor3f(1, 1, 1);
        glPushMatrix();
        // Lets move the origin to where the card will be drawn.
        glTranslatef(((i%4)*200+54), ((i/4)*160 + 90), 0.0);
        glBegin(GL_POLYGON);
        glVertex2f(0, 140);
        glVertex2f(0, 0);
        glVertex2f(180, 0);
        glVertex2f(180, 140);
        glEnd();
        
        draw_card(inplay[i]);
        // Now lets draw a border around the card IF it is selected.
        if(set_in_queue(inplay[i], pc_user_set)){
            glLineWidth(3.0);
            glColor3f(1, 0, 0.5);
            glBegin(GL_LINE_LOOP);
            glVertex2f(0, 140);
            glVertex2f(0, 0);
            glVertex2f(180, 0);
            glVertex2f(180, 140);
            glEnd();
            glLineWidth(2.0);
        }
        glPopMatrix();
    }

    // This is the progress squares.
    for(int i = 0; i < found; i++){

    }
    for(int i = found; i < total; i++){

    }

	/* buffer is ready */
	glutSwapBuffers();

	return ;
}

// This is the main game loop.
void play_game(){
    int temp;
    // Lets check to see if the game needs initialized.
    if (needs_init == 1) init_game();
    // First we check for set mechanics.
    // We should check if the queue is full.
    temp = set_check_user_set();
    if(temp == SET){
        // The use had found a set!
        guess = 1;
        found++;
        // Did the user find all the solutions?
        //if(found == total) needs_init = 1;
    }
    else if(temp == NOSET){
        guess = 2;
    }

    // Now we draw our screen
    my_display();
}

// This will initialize the game.
void init_game(){
    set_gen_deck();
    set_shuffle();
    set_next_game();
    total = set_find_all();
    needs_init = 0;
    found = 0;
}



// This method will assume that the matrix has been translated to create the card.
// This will then be called before the pop.
// It is responsible for drawing the card's features through various switches.
void draw_card(CARD* c){
    float temp;
    // First we set the color
    switch(c->color){
        case RED: glColor3f(1,0,0); break;
        case GREEN: glColor3f(0,1,0); break;
        case BLUE: glColor3f(0,0,1); break;
        default: glColor3f(0,0,0); break;
    }
    // Lets calculate where the first object will be placed on the card.
    // The middle of the card - half of the space the pattern will take.
    temp = (180.0/2.0) - ((54*c->num + 36)/2);
    // Next we draw the basic shape, the right number of times.
    // You will also see a check during the rendering of these shapes to check if
    // c->pattern is OPEN and in which case will change
    // glBegin to GL_LINE_LOOP or GL_LINE_STRIP
    // Thickness is set in my_setup();)
    switch(c->shape){
        case DIAMOND:
            // DIAMONDS!!!!
            glPushMatrix();
            glTranslatef(temp, 14.0, 0);
            for(int i = 0; i <= c->num; i++){
                // The offsets to the points will just be for multiple of the shape.
                if(c->pattern == OPEN) glBegin(GL_LINE_LOOP);
                else glBegin(GL_POLYGON);
                glVertex2f(18+(i*54), 112);
                glVertex2f((i*54), 56);
                glVertex2f(18+(i*54), 0);
                glVertex2f(36+(i*54), 56);
                glEnd();
            }
            glPopMatrix();
            break;
        case RECTANGLE:
            // Boring Rectangle.
            // Very Similar code as above but refined for a rectangle.
            glPushMatrix();
            glTranslatef(temp, 14.0, 0);
            for(int i = 0; i <= c->num; i++){
                if(c->pattern == OPEN) glBegin(GL_LINE_LOOP);
                else glBegin(GL_POLYGON);
                glVertex2f((i*54), 112);
                glVertex2f((i*54), 0);
                glVertex2f(36+(i*54), 0);
                glVertex2f(36+(i*54), 112);
                glEnd();
            }
            glPopMatrix();
            break;
        case PEANUT:
            // Finally dese nuts.
            glPushMatrix();
            glTranslatef(temp, 70, 0);
            for(int i = 0; i <= c->num; i++){
                // Top Half
                if(c->pattern == OPEN) glBegin(GL_LINE_STRIP);
                else glBegin(GL_POLYGON);
                glVertex2f(12+(i*54), 0);
                glVertex2f((i*54), 56 - (112/6)*2);
                glVertex2f((i*54), 56 - (112/6)*1);
                glVertex2f(12+(i*54), 56 - (112/6)*0);
                glVertex2f(24+(i*54), 56 - (112/6)*0);
                glVertex2f(36+(i*54), 56 - (112/6)*1);
                glVertex2f(36+(i*54), 56 - (112/6)*2);
                glVertex2f(24+(i*54), 0);
                glEnd();
                // Bottom Half
                glPushMatrix();
                glScalef(1,-1, 1);
                if(c->pattern == OPEN) glBegin(GL_LINE_STRIP);
                else glBegin(GL_POLYGON);
                glVertex2f(12+(i*54), 0);
                glVertex2f((i*54), 56 - (112/6)*2);
                glVertex2f((i*54), 56 - (112/6)*1);
                glVertex2f(12+(i*54), 56 - (112/6)*0);
                glVertex2f(24+(i*54), 56 - (112/6)*0);
                glVertex2f(36+(i*54), 56 - (112/6)*1);
                glVertex2f(36+(i*54), 56 - (112/6)*2);
                glVertex2f(24+(i*54), 0);
                glEnd();
                glPopMatrix();
            }
            glPopMatrix();
            break;
    }
    if(c->pattern == STRIPE){
        // If the object is striped, we just overlay the card with a bunch of lines.
        glColor3f(1, 1, 1);
        glBegin(GL_LINES);
        for(int i = 0; i < 140; i+=4){
            glVertex2f(0, i);
            glVertex2f(180, i);
        }
        glEnd();
    }
    
}
