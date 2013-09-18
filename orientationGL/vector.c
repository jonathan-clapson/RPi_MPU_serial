#include "Vector.h"


const double pi = 4*atan(1);

/*infinityPath[1] is the same as *(infinityPath+1) where the * means 'pulling data from' and [] is making a pointer =)
   takes in both a vector you wish to integrate and 	a vector (one smalles than the first vector) to store the values of the integral*/
void integrateVector(int vectSize, direcType vector[], direcType newVect[]){	
	
	
	int temp;
	int i=0;
	int j=i+1;
	
	/* Simulates the integration of a vector of acceleration data passed into the function
	 * The fake sample rate for this code (N) is set to 1 sample taken every 125us = 0.000125*/
	int sampRate = 0.000125;
	
	/*calculates the area under two points on the vector plot and saves it into a new vector*/
	for(i=0; i<(vectSize-1; i++){
		
		if(vector[i].x < vector[j].x){
			temp = sampRate*(vector[j].x-vector[i].x)/2;
			temp = temp + sampRate*vector[i].x;			
		}else if(vector[j].x < vector[i].x){
			temp = sampRate*(vector[i].x-vector[j].x)/2;
			temp = temp + sampRate*vector[j].x;	
		}else{
			temp = temp + sampRate*vector[j].x;
		}
		newVect[i].x = temp;
		
		if(vector[i].y < vector[j].y){
			temp = sampRate*(vector[j].y-vector[i].y)/2;
			temp = temp + sampRate*vector[i].y;			
		}else if(vector[j].y < vector[i].y){
			temp = sampRate*(vector[i].y-vector[j].y)/2;
			temp = temp + sampRate*vector[j].y;	
		}else{
			temp = temp + sampRate*vector[j].y;
		}
		newVect[i].y = temp;

		if(vector[i].z < vector[j].z){
			temp = sampRate*(vector[j].z-vector[i].z)/2;
			temp = temp + sampRate*vector[i].z;			
		}else if(vector[j].z < vector[i].z){
			temp = sampRate*(vector[i].z-vector[j].z)/2;
			temp = temp + sampRate*vector[j].z;	
		}else{
			temp = temp + sampRate*vector[j].z;
		}
		newVect[i].z = temp;
	}
	
	return;
}


/*creates the vector components of a circular acceleration pattern*/
void circVects(int vectSize, direcType vector[]){
	
	int i=0;
	double thetaChange = 360/vectSize; /* number of degrees the constant in magnitude acceleration changes direction */
	double currentAngle = 0;
	
	for(i; i<vectSize; i++){
		vector[i].x = 10*sin(currentAngle*PI/180) + 10;
		vector[i].y = 10*cos(currentAngle*PI/180) + 10;
		currentAngle += thetaChange;
	}
	
	return;
}
