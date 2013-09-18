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

/* function to release/destroy our resources and restoring the old desktop */
void Quit( int returnCode )
{
	/* clean up the window */
	SDL_Quit( );

	/* and exit appropriately */
	exit( returnCode );
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

int drawGLScene( )
{
	//1gl unit = 1cm
	
	/* rotational vars for the triangle and quad, respectively */
	static GLfloat rquad;
	
	/* These are to calculate our fps */
	static GLint T0     = 0;
	static GLint Frames = 0;

	/* Clear The Screen And The Depth Buffer */
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/* Set The Color To Blue One Time Only */
	//glColor3f( 0.5f, 0.5f, 1.0f);
	const int DEPTH_SHIFT = 10.0f;
	
	//draw cube
	glColor4f(0.9f, 0.9f, 0.0f, 0.3f);
	glLoadIdentity();
	glTranslatef(-64.0f, -64.0f, -64.0f);
	drawCuboid(128.0f, 128.0f, 128.0f);	

	//draw cube outline
	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawCuboid(128.0f, 128.0f, 128.0f);	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		
	
	//draw top axes
	glLoadIdentity();
	glTranslatef (512.0f*2.0f/3.0f, 768.0f/3.0f, DEPTH_SHIFT);
	glRotatef( rquad, 1.0f, 0.0f, 0.0f );
	drawAxes(128.0f);
	
	//draw right axes	
	glLoadIdentity();
	glTranslatef(512.0f*2.0f/3.0f, 0.0f, DEPTH_SHIFT);
	glRotatef(rquad, 1.0f, 0.0f, 0.0f);	
	drawAxes(128.0f);
	
	//draw front axes	
	glLoadIdentity();
	glTranslatef(512.0f*2.0f/3.0f, -768.0f/3.0f, DEPTH_SHIFT+4.9f);
	glRotatef(rquad, 1.0f, 0.0f, 0.0f);	
	drawAxes(128.0f);
	
	//draw bottom axes
	glLoadIdentity();
	glTranslatef( -512.0f*2.0f/3.0f, -768.0f/3.0f,DEPTH_SHIFT);
	glRotatef(rquad, 1.0f, 0.0f, 0.0f);
	drawAxes(128.0f);
	
	//draw left axes	
	glLoadIdentity();
	glTranslatef(-512.0f*2.0f/3.0f, 0.0f, DEPTH_SHIFT);
	glRotatef(rquad, 1.0f, 0.0f, 0.0f);	
	drawAxes(128.0f);
	
	//draw back axes	
	glLoadIdentity();
	glTranslatef(-512.0f*2.0f/3.0f, 768.0f/3.0f, DEPTH_SHIFT-4.9f);
	glRotatef(rquad, 1.0f, 0.0f, 0.0f);	
	drawAxes(128.0f);
	

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
	
	/* rotate axes */
	rquad -=0.15f;

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
			for (int i=0; i<6; i++) 
			{
				//std::cout << "RPi_Dev" << i << ": x " << std::setprecision(2) << std::setw(8) << readings[i].x << " y " << readings[i].y << " z " << readings[i].z << 
				printf("RPi_Dev%d: ts %ld x %6.0f y %6.0f z %6.0f vx %6.0f vy %6.0f vz %6.0f ax %6.0f ay %6.0f az %6.0f temp %6.0f ox %6.0f oy %6.0f oz %6.0f wx %6.0f wy %6.0f wz %6.0f\n",
					i,
					readings[i].timestamp,
					readings[i].x, readings[i].y, readings[i].z, 
					readings[i].v_x, readings[i].v_y, readings[i].v_z,
					readings[i].a_x, readings[i].a_y, readings[i].a_z,
					readings[i].temp,
					readings[i].o_x, readings[i].o_y, readings[i].o_z,
					readings[i].w_x, readings[i].w_y, readings[i].w_z);
			}			
		}			
		
		drawGLScene( );
	}

	/* clean ourselves up and exit */
	Quit( 0 );

	/* Should never get here */
	return( 0 );
}
