#include <stdio.h>
#include <math.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include "Board.h"
#include "Prototypes.h"
#include "Constants.h"



void initializeData(Data *real, Data *trn) {
  int i;
  for (i = 0; i < SENSOR_DATA_SIZE; i++) {
        real->accDataX[i] = 0.0f;
        real->accDataY[i] = 0.0f;
        real->accDataZ[i] = 0.0f;
        real->gyroDataX[i] = 0.0f;
        real->gyroDataY[i] = 0.0f;
        real->gyroDataZ[i] = 0.0f;

		trn->accDataX[i] = 0.0f;
        trn->accDataY[i] = 0.0f;
        trn->accDataZ[i] = 0.0f;
        trn->gyroDataX[i] = 0.0f;
        trn->gyroDataY[i] = 0.0f;
        trn->gyroDataZ[i] = 0.0f;

  }
}

int calculateGesture(Data *real, Data *trn) {
    float accSimilarity = 0.0f;
    float accDataMagnitude = 0.0f;
    float accTrnDataMagnitude = 0.0f;
    float gyroSimilarity = 0.0f;
    float gyroDataMagnitude = 0.0f;
    float gyroTrnDataMagnitude = 0.0f;
    float amx = 0.0f;  float amy = 0.0f;  float amz = 0.0f;
    float atmx = 0.0f; float atmy = 0.0f; float atmz = 0.0f;
    float gmx = 0.0f;  float gmy = 0.0f;  float gmz = 0.0f;
    float gtmx = 0.0f; float gtmy = 0.0f; float gtmz = 0.0f;

    // keskiarvo
    int i; for (i = 0; i < SENSOR_DATA_SIZE; i++) {
  
        amx += real->accDataX[i]; amy += real->accDataY[i]; amz += real->accDataZ[i];
        atmx += trn->accDataX[i]; atmy += trn->accDataY[i]; atmz += trn->accDataZ[i];
        gmx += real->gyroDataX[i]; gmy += real->gyroDataY[i]; gmz += real->gyroDataZ[i];
        gtmx += trn->gyroDataX[i]; gtmy += trn->gyroDataY[i]; gtmz += trn->gyroDataZ[i];
    }
    amx /= SENSOR_DATA_SIZE; amy /= SENSOR_DATA_SIZE; amz /= SENSOR_DATA_SIZE;
    atmx /= SENSOR_DATA_SIZE; atmy /= SENSOR_DATA_SIZE; atmz /= SENSOR_DATA_SIZE;
    gmx /= SENSOR_DATA_SIZE; gmy /= SENSOR_DATA_SIZE; gmz /= SENSOR_DATA_SIZE;
    gtmx /= SENSOR_DATA_SIZE; gtmy /= SENSOR_DATA_SIZE; gtmz /= SENSOR_DATA_SIZE;


   // correlation
    for (i = 0; i < SENSOR_DATA_SIZE; i++) {
        accSimilarity += (real->accDataX[i] - amx) * (trn->accDataX[i] - atmx) + (real->accDataY[i] - amy) * (trn->accDataY[i] - atmy)  + (real->accDataZ[i] - amz) * (trn->accDataZ[i] - atmz);
        accDataMagnitude += pow((real->accDataX[i] - amx), 2) + pow((real->accDataY[i] - amy), 2) + pow((real->accDataZ[i] - amz), 2);
        accTrnDataMagnitude += pow((trn->accDataX[i] - atmx), 2) + pow((trn->accDataY[i] - atmy), 2) + pow((trn->accDataZ[i] - atmz), 2);

        gyroSimilarity += (real->gyroDataX[i] - gmx) * (trn->gyroDataX[i] - gtmx) + (real->gyroDataY[i] - gmy) * (trn->gyroDataY[i] - gtmy)  + (real->gyroDataZ[i] - gmz) * (trn->gyroDataZ[i] - gtmz);
        gyroDataMagnitude += pow((real->gyroDataX[i] - gmx), 2) + pow((real->gyroDataY[i] - gmy), 2) + pow((real->gyroDataZ[i] - gmz), 2);
        gyroTrnDataMagnitude += pow((trn->gyroDataX[i] - gtmx), 2) + pow((trn->gyroDataY[i] - gtmy), 2) + pow((trn->gyroDataZ[i] - gtmz), 2);

    }
   

    accSimilarity /= sqrt(accDataMagnitude * accTrnDataMagnitude);
    gyroSimilarity /= sqrt(gyroDataMagnitude * gyroTrnDataMagnitude);

    //printf("%f", accSimilarity);
    //printf("%f", gyroSimilarity);
   
	if (sqrt(accSimilarity * accSimilarity + gyroSimilarity * gyroSimilarity) > GESTURE_THRESHOLD) {
        return true;
    } 
    return false;

}




void recordData(Data *d, float ax, float ay, float az, float gx, float gy, float gz, float timestamp) {
    int i; for (i = SENSOR_DATA_SIZE - 1; i > 0; i--) {        
        // shift values
        d->accDataX[i] = d->accDataX[i-1];
        d->accDataY[i] = d->accDataY[i-1];
        d->accDataZ[i] = d->accDataZ[i-1];
        d->gyroDataX[i] = d->gyroDataX[i-1];
        d->gyroDataY[i] = d->gyroDataY[i-1];
        d->gyroDataZ[i] = d->gyroDataZ[i-1];
        d->timestampData[i] = d->timestampData[i-1];
    }
    // push 
    int smooth = 4;
    d->accDataX[0] = ax / smooth;
    d->accDataY[0] = ay / smooth;
    d->accDataZ[0] = az / smooth;
    d->gyroDataX[0] = gx / smooth;
    d->gyroDataY[0] = gy / smooth;
    d->gyroDataZ[0] = gz / smooth;
    for (i = 1; i < smooth; i++) {
        d->accDataX[0] += d->accDataX[i] / smooth;
        d->accDataY[0] += d->accDataY[i] / smooth;
        d->accDataZ[0] += d->accDataZ[i] / smooth;
        d->gyroDataX[0] += d->gyroDataX[i] / smooth;
        d->gyroDataY[0] += d->gyroDataY[i] / smooth;
        d->gyroDataZ[0] += d->gyroDataZ[i] / smooth;
    }
    d->timestampData[0] = timestamp;
}

