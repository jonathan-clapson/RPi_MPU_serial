/*
* This code was created by Jeff Molofee '99 
* (ported to Linux/SDL by Ti Leggett '01)
*
* If you've found this code useful, please let me know.
*
* Visit Jeff at http://nehe.gamedev.net/
* 
* or for port-specific comments, questions, bugreports etc. 
* email to leggett@eecs.tulane.edu
*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include "SDL.h"

#include "error.h"
#include "rpi_mpu_io.h"

#include "graphics.h"

/* screen width, height, and bit depth */
#define SCREEN_WIDTH  1024	//viewing screen dimensions 
#define SCREEN_HEIGHT 768	// 480 gl units??
#define SCREEN_BPP     16	//bits per pixel resolution

/* Define our booleans */
#define TRUE  1
#define FALSE 0

/* This is our SDL surface */
SDL_Surface *surface; 

/* Font stuff */
GLuint  base; /* Base Display List For The Font Set */
GLfloat cnt1; /* 1st Counter Used To Move Text & For Coloring */
GLfloat cnt2; /* 2nd Counter Used To Move Text & For Coloring */

/* function to recover memory form our list of characters */
GLvoid KillFont( )
{
    glDeleteLists( base, 96 );

    return;
}

/* function to release/destroy our resources and restoring the old desktop */
void Quit( int returnCode )
{	
	KillFont( );
	
	/* clean up the window */
	SDL_Quit( );

	/* and exit appropriately */
	exit( returnCode );
}

/* function to build our font list */
GLvoid buildFont( )
{
    Display *dpy;          /* Our current X display */
    XFontStruct *fontInfo; /* Our font info */

    /* Sotrage for 96 characters */
    base = glGenLists( 96 );

    /* Get our current display long enough to get the fonts */
    dpy = XOpenDisplay( NULL );

    /* Get the font information */
    fontInfo = XLoadQueryFont( dpy, "-adobe-helvetica-medium-r-normal--18-*-*-*-p-*-iso8859-1" );

    /* If the above font didn't exist try one that should */
    if ( fontInfo == NULL )
	{
	    fontInfo = XLoadQueryFont( dpy, "fixed" );
	    /* If that font doesn't exist, something is wrong */
	    if ( fontInfo == NULL )
		{
		    fprintf( stderr, "no X font available?\n" );
		    Quit( 1 );
		}
	}

    /* generate the list */
    glXUseXFont( fontInfo->fid, 32, 96, base );

    /* Recover some memory */
    XFreeFont( dpy, fontInfo );

    /* close the display now that we're done with it */
    XCloseDisplay( dpy );

    return;
}

/* Print our GL text to the screen */
GLvoid glPrint( const char *fmt, ... )
{
    char text[256]; /* Holds our string */
    va_list ap;     /* Pointer to our list of elements */

    /* If there's no text, do nothing */
    if ( fmt == NULL )
	return;

    /* Parses The String For Variables */
    va_start( ap, fmt );
      /* Converts Symbols To Actual Numbers */
      vsprintf( text, fmt, ap );
    va_end( ap );

    /* Pushes the Display List Bits */
    glPushAttrib( GL_LIST_BIT );

    /* Sets base character to 32 */
    glListBase( base - 32 );

    /* Draws the text */
    glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );

    /* Pops the Display List Bits */
    glPopAttrib( );
}

/* function to reset our viewport after a window resize */
int resizeWindow( int width, int height )
{
	/* Height / width ratio */
	GLfloat ratio;		//initialise a ratio variable

	/* Protect against a divide by zero */
	if ( height == 0 )
		height = 1;

	//converting the int types to GLfloat type before assigning to ratio
	ratio = ( GLfloat )width / ( GLfloat )height;

	/* Setup our viewport. Or the zoom on the object within the window, how much 
	* we can see of the area etc... lower left corner and top right corner */
	glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );


	/* change to the projection matrix and set our viewing volume. */
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	/* Set our perspective - sets the angle at which your view sees and the ratio
	* and close/far proximity that the display ignores the objects view*/
	glOrtho(-512, 512, -768/2, 768/2, -768/2, 768/2);

	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode( GL_MODELVIEW );

	/* Reset The View */
	glLoadIdentity( );

	return( TRUE );
}

/* function to handle key press events */
void handleKeyPress( SDL_keysym *keysym )
{
	switch ( keysym->sym )
	{
	case SDLK_ESCAPE:
		/* ESC key was pressed */
		Quit( 0 );
		break;
	case SDLK_F1:
		/* F1 key was pressed this toggles fullscreen mode */
		SDL_WM_ToggleFullScreen( surface );
		break;
	default:
		break;
	}

	return;
}

/* general OpenGL initialization function */
int initGL( )
{

	/* Enable smooth shading */
	glShadeModel( GL_SMOOTH );

	/* Set the background black */
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	/* Depth buffer setup */
	glClearDepth( 1.0f );

	/* Enables Depth Testing */
	//glEnable( GL_DEPTH_TEST );
	glDisable( GL_DEPTH_TEST );

	/* The Type Of Depth Test To Do */
	glDepthFunc( GL_LEQUAL );

	/* Really Nice Perspective Calculations */
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	
	
	/* Set The Blending Function For Translucency */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);		
	
	buildFont( );	

	return( TRUE );
}

void glMoveCamera()
{
	glLoadIdentity();
	
	// move camera a distance r away from the center
	glTranslatef(0.0f, 0.0f, -15.0f);

	// rotate around y by 45 degrees 
	glRotatef(-45.0f, 0, 1, 0);
	// rotate around x by 45 degrees
	glRotatef(-45.0f, 1, 0, 0);

	// move to center of circle    
	//glTranslatef(-cx, -cy, -cz)
}

int drawGLScene( struct reading_memory_type *readings )
{
	//1gl unit = 1cm
	
	/* These are to calculate our fps */
	static GLint T0     = 0;
	static GLint Frames = 0;

	/* Clear The Screen And The Depth Buffer */
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/* Set The Color To Blue One Time Only */
	//glColor3f( 0.5f, 0.5f, 1.0f);
	const int DEPTH_SHIFT = 10.0f;
	const float axes_scale = 120.0f;
	
	//draw cube
	glColor4f(0.9f, 0.9f, 0.0f, 0.3f);
	glLoadIdentity();
	
	//compute average
	double o_x = 0;
	double o_y = 0;
	double o_z = 0;
	for (int i=0; i<6; i++)
	{
		//skip the top coz its a little bit crazy
		if (i==4)
			continue;
		
		o_x += readings[i].o_x;
		o_y += readings[i].o_y;
		o_z += readings[i].o_z;		
	}
	o_x /= 5;
	o_y /= 5;
	o_z /= 5;
	
	glRotatef( o_x, 1.0f, 0.0f, 0.0f );
	glRotatef( o_y, 0.0f, 1.0f, 0.0f );
	glRotatef( o_z, 0.0f, 0.0f, 1.0f );	
	glTranslatef(-64.0f, -64.0f, -64.0f);	
	
	drawCuboid(128.0f, 128.0f, 128.0f);	

	//draw cube outline
	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawCuboid(128.0f, 128.0f, 128.0f);	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		
	
	//draw right axes	
	glLoadIdentity();
	glTranslatef(512.0f*2.0f/3.0f, 0.0f, DEPTH_SHIFT);
	glRotatef( readings[3].o_x, 1.0f, 0.0f, 0.0f );
	glRotatef( readings[3].o_y, 0.0f, 1.0f, 0.0f );
	glRotatef( readings[3].o_z, 0.0f, 0.0f, 1.0f );
	drawAxes(axes_scale);
	//draw text
	glColor4f(1.0f,0.0f,0.0f,1.0f);	
	glLoadIdentity();
	glRasterPos2f( 512.0f*2.0f/3.0f, 100.0f );
	glPrint("Right Face");
	
	//draw top axes	
	glLoadIdentity();
	glTranslatef(512.0f*2.0f/3.0f, 768.0f/3.0f, DEPTH_SHIFT);
	glRotatef( readings[4].o_x, 1.0f, 0.0f, 0.0f );
	glRotatef( readings[4].o_y, 0.0f, 1.0f, 0.0f );
	glRotatef( readings[4].o_z, 0.0f, 0.0f, 1.0f );
	drawAxes(axes_scale);
	//draw text
	glColor4f(1.0f,0.0f,0.0f,1.0f);	
	glLoadIdentity();
	glRasterPos2f( 512.0f*2.0f/3.0f, 768.0f/3.0f+100.0f );
	glPrint("Top Face");
	
	//draw front axes	
	glLoadIdentity();
	glTranslatef(512.0f*2.0f/3.0f, -768.0f/3.0f, DEPTH_SHIFT);
	glRotatef( readings[2].o_x, 1.0f, 0.0f, 0.0f );
	glRotatef( readings[2].o_y, 0.0f, 1.0f, 0.0f );
	glRotatef( readings[2].o_z, 0.0f, 0.0f, 1.0f );
	drawAxes(axes_scale);
	//draw text
	glColor4f(1.0f,0.0f,0.0f,1.0f);	
	glLoadIdentity();
	glRasterPos2f( 512.0f*2.0f/3.0f, -768.0f/3.0f+100.0f );
	glPrint("Front Face");
	
	//draw bottom axes
	glLoadIdentity();
	glTranslatef( -512.0f*2.0f/3.0f, -768.0f/3.0f,DEPTH_SHIFT);
	glRotatef( readings[0].o_x, 1.0f, 0.0f, 0.0f );
	glRotatef( readings[0].o_y, 0.0f, 1.0f, 0.0f );
	glRotatef( readings[0].o_z, 0.0f, 0.0f, 1.0f );
	drawAxes(axes_scale);
	//draw text
	glColor4f(1.0f,0.0f,0.0f,1.0f);	
	glLoadIdentity();
	glRasterPos2f( -512.0f*2.0f/3.0f, -768.0f/3.0f+100.0f );
	glPrint("Bottom Face");
	
	//draw left axes	
	glLoadIdentity();
	glTranslatef(-512.0f*2.0f/3.0f, 0.0f, DEPTH_SHIFT);
	glRotatef( readings[1].o_x, 1.0f, 0.0f, 0.0f );
	glRotatef( readings[1].o_y, 0.0f, 1.0f, 0.0f );
	glRotatef( readings[1].o_z, 0.0f, 0.0f, 1.0f );	
	drawAxes(axes_scale);
	//draw text
	glColor4f(1.0f,0.0f,0.0f,1.0f);	
	glLoadIdentity();
	glRasterPos2f( -512.0f*2.0f/3.0f, 100.0f );
	glPrint("Left Face");
	
	//draw back axes	
	glLoadIdentity();
	glTranslatef(-512.0f*2.0f/3.0f, 768.0f/3.0f, DEPTH_SHIFT);
	glRotatef( readings[5].o_x, 1.0f, 0.0f, 0.0f );
	glRotatef( readings[5].o_y, 0.0f, 1.0f, 0.0f );
	glRotatef( readings[5].o_z, 0.0f, 0.0f, 1.0f );
	drawAxes(axes_scale);
	//draw text
	glColor4f(1.0f,0.0f,0.0f,1.0f);	
	glLoadIdentity();
	glRasterPos2f( -512.0f*2.0f/3.0f, 768.0f/3.0f+100.0f );
	glPrint("Back Face");
	
	/* Draw it to the screen */
	SDL_GL_SwapBuffers( );

	/* Gather our frames per second */
	Frames++;
	GLint t = SDL_GetTicks();
	if (t - T0 >= 5000) {
		GLfloat seconds = (t - T0) / 1000.0;
		GLfloat fps = Frames / seconds;
		printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
		T0 = t;
		Frames = 0;
	}

	return( TRUE );
}

int main( int argc, char **argv )
{
	/* Flags to pass to SDL_SetVideoMode */
	int videoFlags;
	/* main loop variable */
	int done = FALSE;
	/* used to collect events */
	SDL_Event event;
	/* this holds some info about our display */
	const SDL_VideoInfo *videoInfo;

	/* initialize SDL */
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		fprintf( stderr, "Video initialization failed: %s\n",
		SDL_GetError( ) );
		Quit( 1 );
	}

	/* Fetch the video info */
	videoInfo = SDL_GetVideoInfo( );

	if ( !videoInfo )
	{
		fprintf( stderr, "Video query failed: %s\n",
		SDL_GetError( ) );
		Quit( 1 );
	}

	/* the flags to pass to SDL_SetVideoMode */
	videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
	videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
	videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
	videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

	/* This checks to see if surfaces can be stored in memory */
	if ( videoInfo->hw_available )
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	/* This checks if hardware blits can be done */
	if ( videoInfo->blit_hw )
		videoFlags |= SDL_HWACCEL;

	/* Sets up OpenGL double buffering */
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	/* get a SDL surface */
	surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, videoFlags );

	/* Verify there is a surface */
	if ( !surface )
	{
		fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
		Quit( 1 );
	}

	/* initialize OpenGL */
	initGL( );

	/* resize the initial window */
	resizeWindow( SCREEN_WIDTH, SCREEN_HEIGHT );
	
		
	/* initialise connection to RPi */
	#if defined (_WIN32) || defined( _WIN64)
		#define DEVICE_PORT "COM1"			// COM1 for windows
	#endif
	#ifdef __linux__
		#define DEVICE_PORT "/dev/ttyUSB0"	// ttyS0 for linux
	#endif
	
	rpi_mpu_io* rpi_mpu_dev = new rpi_mpu_io((char *)DEVICE_PORT, 2000000);
	// Open serial port device
    if (!rpi_mpu_dev->device_valid()) 
    {
		delete rpi_mpu_dev;
		Quit( 1 );
	}
    printf ("Serial port opened successfully!\n");
    	

	/* wait for events */ 
	while ( !done )
	{
		/* handle the events in the queue */
		while ( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
			case SDL_ACTIVEEVENT:
				/* Something's happend with our focus
				* If we lost focus or we are iconified, we
				* shouldn't draw the screen
				*/
				//if ( event.active.gain == 0 )
				
				break;			    
			case SDL_VIDEORESIZE:
			/* handle resize event */
				surface = SDL_SetVideoMode( event.resize.w, event.resize.h, 16, videoFlags );
				if ( !surface )
				{
					fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
					Quit( 1 );
				}
				resizeWindow( event.resize.w, event.resize.h );
				break;
			case SDL_KEYDOWN:
				/* handle key presses */
				handleKeyPress( &event.key.keysym );
				break;
			case SDL_QUIT:
				/* handle quit requests */
				done = TRUE;
				break;
			default:
				break;
			}
		}
		
		/* grab mpu data */
		struct reading_memory_type readings[6];

		int ret = rpi_mpu_dev->getReading(readings);
		
		if (ret == ERR_GOTREADING) 
		{
			/*for (int i=0; i<6; i++) 
			{
				printf("RPi_Dev%d: ts %ld x %6.0f y %6.0f z %6.0f vx %6.0f vy %6.0f vz %6.0f ax %6.0f ay %6.0f az %6.0f temp %6.0f ox %6.0f oy %6.0f oz %6.0f wx %6.0f wy %6.0f wz %6.0f\n",
					i,
					readings[i].timestamp,
					readings[i].x, readings[i].y, readings[i].z, 
					readings[i].v_x, readings[i].v_y, readings[i].v_z,
					readings[i].a_x, readings[i].a_y, readings[i].a_z,
					readings[i].temp,
					readings[i].o_x, readings[i].o_y, readings[i].o_z,
					readings[i].w_x, readings[i].w_y, readings[i].w_z);
			}*/
						
		}
		else
		{
			/* if we don't have a reading, try to set mode */
			rpi_mpu_dev->writeString("0\n");			
			printf("could not get reading\n");		
		}
		
		drawGLScene( readings );
	}

	/* clean ourselves up and exit */
	Quit( 0 );

	/* Should never get here */
	return( 0 );
}
