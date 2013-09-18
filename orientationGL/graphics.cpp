#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "graphics.h"
#include "vector.h"	

void rotateShape(int vectSize, int axis[]){
	
	if(vectSize != 4){ 
		
	}
}


/* need function that takes in the dimensions of the cuboid and then 
 * autogenerates the dimensions of the axis cuboids and their 
 * locations on the parent cuboid shape faces */

/*where width is the x axis of the front face, length is the z axis of the front face and y is the height of the front face of the cuboid*/

/* 
 * drawCuboid 
 * origin at bottom left corner 
 * width
 * height
 * depth 
 */
void drawCuboid(float width, float height, float depth){

	glBegin(GL_QUADS);
	    /* top of Polygon */
        glVertex3f(0, height, depth);
        glVertex3f(width, height, depth);
        glVertex3f(width, height, 0);
        glVertex3f(0, height, 0);        
        /* bottom of Polygon */
        glVertex3f(0, 0, depth);
        glVertex3f(width, 0, depth);
        glVertex3f(width, 0, 0);
        glVertex3f(0, 0, 0);  
        /* front of Polygon */
        glVertex3f(width, height, depth);
        glVertex3f(0, height, depth);
        glVertex3f(0, 0, depth);
        glVertex3f(width, 0, depth);
        /* back of Polygon */
        glVertex3f(0, height, 0);
        glVertex3f(width, height, 0);
        glVertex3f(width, 0, 0);
        glVertex3f(0, 0, 0);
        /* right side of Polygon */
        glVertex3f(width, height, 0);
        glVertex3f(width, height, depth);
        glVertex3f(width, 0, depth);
        glVertex3f(width, 0, 0);
        /* left side of Polygon */
        glVertex3f(0, height, depth);
        glVertex3f(0, height, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, depth);
    glEnd();
}

/* drawAxes 
 * - takes a scaling factor as an argument
 */

void drawAxes(float scale)
{
	/* rectangle in x direction */
	glColor3f(1.0f*scale,0.0f*scale,0.0f*scale);	
	drawCuboid(1.0f*scale, 0.1f*scale, 0.1f*scale);
	
	/* rectangle in y direction */
	glColor3f(0.0f*scale,1.0f*scale,0.0f*scale);	
	drawCuboid(0.1f*scale,1.0f*scale,0.1f*scale);
	
	/* rectangle in z direction */
	glColor3f(0.0f*scale,0.0f*scale,1.0f*scale);	
	drawCuboid(0.1f*scale,0.1f*scale,1.0f*scale);		
}
