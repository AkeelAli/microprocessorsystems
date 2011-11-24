#include "stm32f10x_conf.h"
#include "LSM303DLH.c"
#include "stm32f10x.h"
#include "fdacoefs.h"


typedef struct ResultSet {

  s16 *n0;
  s16 *n1;
  s16 *n2;
  s16 *n3;
  s16 *n4;
  s16 *n5;
  s16 *n6;
  s16 *n7;
  s16 *n8;
  s16 *n9;
  s16 *n10;
  s16 *n11;  
  s16 *temp;
  float *avg;

} ResultSet;

void rotate (ResultSet  *n);
void add_to_set (ResultSet  *n, s16 *a);


s16 output[3];
s16 filtered_output[3];
float angles[3];
u8 Power_Mode = 0x20;
u8 ODR = 0x08;
u8 Axes_Enable = 0x07;
u8 FS = 0;
u8 Data_Update = 0;
u8 Endianess = 0;

LSM_Acc_ConfigTypeDef config;


int main(void) {    


	ResultSet a;


    s16 n0[3];
    s16 n1[3];
    s16 n2[3];
    s16 n3[3];
    s16 n4[3];
    s16 n5[3];
    s16 n6[3];
    s16 n7[3];
    s16 n8[3];
    s16 n9[3];
    s16 n10[3];
    s16 n11[3];
    s16 temp[3];
    float avg[3];

// = {&n0,&n1,&n2,&n3,&n4,&n5,&n6,&n7,&n8,&n9,&n10,&n11,&temp, $avg};

    a.temp =  &temp[0];
    a.n11 = &n11[0];
    a.n10 = &n10[0];
    a.n9 = &n9[0];
    a.n8 = &n8[0];
    a.n7 = &n7[0];
    a.n6 = &n6[0];
    a.n5 = &n5[0];
    a.n4 = &n4[0];
    a.n3 = &n3[0];
    a.n2 = &n2[0];
    a.n1 = &n1[0];
    a.n0 = &n0[0];
    a.avg = &avg[0];




	LSM303DLH_I2C_Init();


	 
	config.Power_Mode = Power_Mode;
	config.ODR = ODR;
	config.Axes_Enable = Axes_Enable;
	config.FS = FS;
	config.Data_Update = Data_Update;
	config.Endianess = Endianess;




	LSM303DLH_Acc_Config(&config);
	while (1) {
		LSM303DLH_Acc_Read_Acc(output);
		add_to_set(&a,output);

	//	angles[0] = atan2(filtered_output[0],sqrt(filtered_output[1]*filtered_output[1] + filtered_output[2]*filtered_output[2])); 

	}

 	return 0;
}

void rotate (ResultSet *n) {
    (*n).temp =  (*n).n11;
    (*n).n11 = (*n).n10;
    (*n).n10 = (*n).n9;
    (*n).n9 = (*n).n8;
    (*n).n8 = (*n).n7;
    (*n).n7 = (*n).n6;
    (*n).n6 = (*n).n5;
    (*n).n5 = (*n).n4;
    (*n).n4 = (*n).n3;
    (*n).n3 = (*n).n2;
    (*n).n2 = (*n).n1;
    (*n).n1 = (*n).n0;
    (*n).n0 = (*n).temp;
}



void add_to_set (ResultSet  *n, s16 *a) {
    unsigned int i = 0;
	rotate (n);
    (*n).n0[0] = a[0];
    (*n).n0[1] = a[1];
    (*n).n0[2] = a[2];



    while (i < 3) {
    (*n).avg[i] =
        B[0] * (*n).n0[i] +
        B[1] * (*n).n1[i] +
        B[2] * (*n).n2[i] +
        B[3] * (*n).n3[i] +
        B[4] * (*n).n4[i] +
        B[5] * (*n).n5[i] +
        B[6] * (*n).n6[i] +
        B[7] * (*n).n7[i] +
        B[8] * (*n).n8[i] +
        B[9] * (*n).n9[i] +
        B[10] * (*n).n10[i] +
        B[11] * (*n).n11[i] ;
        
        i++;
    }

	filtered_output[0] = (*n).avg[0];
	filtered_output[1] = (*n).avg[1];
	filtered_output[2] = (*n).avg[2];

}
