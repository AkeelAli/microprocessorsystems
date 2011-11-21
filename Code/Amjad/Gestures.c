#include "Gestures.h"

int updateGesture(s16 x, s16 y, s16 z, s16 /*NEED to check if this is the correct datatype for gyro*/ roll)
	{
		int result; 
		s16 delta_x, delta_y, delta_z, delta_roll;
		s16 temp;

		//Find Maximum Magnitude:
		delta_x = abs( (x - prev_rawAcc[0]) );
		delta_y = abs( (y - prev_rawAcc[1]) );
		delta_z = abs( (z - prev_rawAcc[2]) );
		
		//We might need to apply a scaling factor to delta roll in order to ensure that we can compare it with the other linear deltas
		delta_roll = abs( (roll - prev_rawGyr[0/*Make sure that this represents roll*/]) );

		//Find which has the largest delta
		temp = delta_x;
		if(temp < delta_y) temp = delta_y;
		if(temp < delta_z) temp = delta_z;
		if(temp < delta_roll) temp = delta_roll;

		if( (temp >= ACC_X_MAG) || (temp >= ACC_Y_MAG) || (temp >= ACC_Z_MAG) || (temp >= ROLL_MAG))
		{
			switch(temp)
			{
				case(delta_x):
					if(x - prevrawAcc[0] >= 0)
						intelligent_push(pos_x);
					else
						intelligent_push(neg_x);
					break;
				
				case(delta_y):
					if(y - prevrawAcc[1] >= 0)
						intelligent_push(pos_y);
					else
						intelligent_push(neg_y);
					break;			
			
				case(delta_z):
					if(z - prevrawAcc[2] >= 0)
						intelligent_push(pos_z);
					else
						intelligent_push(neg_z);
					break;		
					
				case(delta_roll):
					if(roll - prevrawGyr[0] >= 0)
						intelligent_push(pos_roll);
					else
						intelligent_push(neg_roll);
					break;
				default:
					
			}//End Switch	
		}//End if
		
		//Update Prev_values:
		prev_rawAcc[0] = x;
		prev_rawAcc[1] = y;
		prev_rawAcc[2] = z;
		prev_rawGyr[0] = roll;

		//Calling Process_symbol
		result = process_symbol();
		return result;

	}
