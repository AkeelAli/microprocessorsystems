
#include "stm32f10x_conf.h"
#include "atan_LUT.h"
#include <math.h>

#define TO_DEG_FACTOR 57.29577951 
#define LUT_SIZE 100

void init_lut(double lut_1[],double lut_2[]){
	int i;
	double value;	

	for(i=0;i<LUT_SIZE;i++){
		value=(i+1)/100.;
		lut_1[i]=atan(value)*TO_DEG_FACTOR;				
	}
	
	for(i=0;i<LUT_SIZE;i++){
		value=(i+1)/10. + 1;
		lut_2[i]=atan(value)*TO_DEG_FACTOR;				
	}	
}

float atan_table(float value){
	static int initialized=0;
	//2 lookup tables 
	static double lut_1[LUT_SIZE];
	static double lut_2[LUT_SIZE];

	int sign=1;
	float ans=0.0;
	int index=0;

	if (!initialized){
		init_lut(lut_1,lut_2);
		initialized=1;
	}

	if (value < 0){
		sign=-1;
		value*=-1;
	}

	if(value <= 1){
		index=(int) (value*100 - 1);
		ans=lut_1[index];
	}
	else if(value <= 11){
		index=(int) ((value - 1)*10 - 1);
		ans=lut_2[index];
	}
	else if (value <= 11.430)
	    ans=85.0;
	else if (value <= 14.301)
	    ans=86.0;
	else if (value <= 19.081)
	    ans=87.0;
	else if (value <= 28.636)
	    ans=88.0;
	else if (value <= 57.290)
	    ans=89.0;
	else
	    ans=90.0;

	return ans*sign;
}

