#ifndef PROTOTYPES_H_
#define PROTOTYPES_H_

#include <xdc/std.h>
#include <stdbool.h>
#include <string.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>
#include <xdc/runtime/System.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

#include "Constants.h"
#include "../util/mathh.h"
#include "../Tasks/Display.h"
#include "../eventdefs.h"

extern Event_Handle globalEvents;


/* --- DisplayTask.c --- */
void buttonMenuFxn(PIN_Handle handle, PIN_Id pinId);
void buttonShutFxn(PIN_Handle handle, PIN_Id pinId);
void ResetButtons(UArg arg0);
void DrawMainMenu(tContext *pContext);
void DrawGestureMenuRead(tContext *pContext);
void DrawGestureMenuTrain(tContext *pContext);
void DrawTemperatureMenu(tContext *pContext);
void DrawMessageMenu(tContext *pContext);
void displayMessage(char* msg);
void setAccelerationTxt(float ax, float ay, float az);
void setGyroTxt(float gx, float gy, float gz);
void setTempnPress(double temperature, double pressure);
void CheckBattery();

void startDisplayTask();



/* --- sensorTask.c --- */

static void SetI2CParams();
void Sensors_SwitchToMPU9250I2C();
void Sensors_SwitchToNormalI2C();
void startSensorTask();



/* --- commTask.c --- */
void startCommTask();



/* --- buzzerTask.c --- */
enum BuzzType { 
    SWEEP_UP,
    SWEEP_DOWN 
};

void buzz(int bType);
void startBuzzerTask();



/* --- cosSimilarity.c --- */
typedef struct {
    float timestampData[SENSOR_DATA_SIZE];
    float accDataX[SENSOR_DATA_SIZE];
    float accDataY[SENSOR_DATA_SIZE];
    float accDataZ[SENSOR_DATA_SIZE];
    float gyroDataX[SENSOR_DATA_SIZE];
    float gyroDataY[SENSOR_DATA_SIZE];
    float gyroDataZ[SENSOR_DATA_SIZE];
} Data;
Data train;
Data read;

void initializeData(Data *real, Data *trn);
int calculateGesture(Data *real, Data *trn);
void recordData(Data *d, float ax, float ay, float az, float gx, float gy, float gz, float timestamp);


/* --- mainTask.c --- */

void SwitchStateTo_IDLE();
void SwitchStateTo_READ_SENSOR();
void SwitchStateTo_RECORD_SENSOR();
void SwitchStateTo_MSG_WAITING();
void SwitchStateTo_TEMPERATURE();
void sensorTick(UArg arg0);

void startMainTask();

/* --- main.c --- */
void SetPowerOff();






#endif //PROTOTYPES_H_
