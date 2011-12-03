#include "service_functions.h"


uint8_t findMode(uint8_t byteA[]) {
    uint8_t size = 32;
	//uint8_t size = sizeof(byteA)/sizeof(byteA[0]);
    uint8_t countA[256];
   
    uint8_t mode;
    int8_t maxCount = -1;

    //initialize counts to 0
    uint16_t i = 0;

	for(i = 0; i < 256; i++) {
        countA[i]=0;
    }
    
   
    //place counts
    for(i = 0; i < size; i++) {
        countA[byteA[i]]++;
    }

    //find mode
    for(i = 0; i < 256; i++) {
        if (countA[i] > maxCount) {
            maxCount = countA[i];
            mode = i;
        }
    }

    return mode;

}
