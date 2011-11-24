#include <stdio.h>
#include <stdlib.h>
#include "fdacoefs.h"

typedef struct ResultSet {

  unsigned int *n0;
  unsigned int *n1;
  unsigned int *n2;
  unsigned int *n3;
  unsigned int *n4;
  unsigned int *n5;
  unsigned int *n6;
  unsigned int *n7;
  unsigned int *n8;
  unsigned int *n9;
  unsigned int *n10;
  unsigned int *n11;
  float *avg;
  unsigned int *temp;

} ResultSet;

void rotate (ResultSet  *n);
void add_to_set (ResultSet  *n, unsigned int a);

void main (int argc,char *argv[]) {


    ResultSet a;


    unsigned int n0 = 0;
    unsigned int n1 = 0;
    unsigned int n2 = 0;
    unsigned int n3 = 0;
    unsigned int n4 = 0;
    unsigned int n5 = 0;
    unsigned int n6 = 0;
    unsigned int n7 = 0;
    unsigned int n8 = 0;
    unsigned int n9 = 0;
    unsigned int n10 = 0;
    unsigned int n11 = 0;
    unsigned int temp = 0;
    float avg = 0;

    a.temp =  &temp;
    a.n11 = &n11;
    a.n10 = &n10;
    a.n9 = &n9;
    a.n8 = &n8;
    a.n7 = &n7;
    a.n6 = &n6;
    a.n5 = &n5;
    a.n4 = &n4;
    a.n3 = &n3;
    a.n2 = &n2;
    a.n1 = &n1;
    a.n0 = &n0;
    a.avg = &avg;


    unsigned int i = 0;
    while (i < 10) {
        add_to_set (&a, 5);
        i++;
    }
    add_to_set (&a, 100);
    while (i > 0) {
        add_to_set (&a, 5);
        i--;
    }
   // ResultSet a = {1,2,3,4,5,6,7,8};
   // printf ("\n1: %i\n2: %i\n3: %i\n4: %i\n\navg is %f\n",*a.n0, *a.n1, *a.n2, *a.n3, *a.avg);

    //rotate (&a);

    //printf ("\n1: %i\n2: %i\n3: %i\n4: %i\n\navg is %f\n",*a.n0, *a.n1, *a.n2, *a.n3, *a.avg);
}

// initial value of result needs to be 1, otherwise result is invalid for 1!
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

void add_to_set (ResultSet  *n, unsigned int a) {
    rotate (n);

    *(*n).n0 = a;

    *(*n).avg =
        B[0] * *(*n).n0 +
        B[1] * *(*n).n1 +
        B[2] * *(*n).n2 +
        B[3] * *(*n).n3 +
        B[4] * *(*n).n4 +
        B[5] * *(*n).n5 +
        B[6] * *(*n).n6 +
        B[7] * *(*n).n7 +
        B[8] * *(*n).n8 +
        B[9] * *(*n).n9 +
        B[10] * *(*n).n10 +
        B[11] * *(*n).n11 ;

    printf ("\navg is %f\n",*(*n).avg);
}
