#ifndef __VECTOR_H__
#define __VECTOR_H__

/* This structure holds the actual x, y, z components of the acceleration read from an IMU */
struct direcType {
	double x, y, z;
};

void integrateVector(struct direcType vector[], struct direcType newVect[]);
void circVects(struct direcType vector[]);

extern const double pi;

#endif	
