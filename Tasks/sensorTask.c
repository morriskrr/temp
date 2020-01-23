#include <stdio.h>
#include <stdbool.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include "../Board.h"
#include "../Constants.h"
#include "../Prototypes.h"
#include "sensors/bmp280.h"
#include "sensors/mpu9250.h"
#include <inttypes.h>

#define STACKSIZE 2048
char SensorTaskStack[STACKSIZE];


enum I2CMODE {
	I2CMODE_NONE = 0,
	I2CMODE_NORMAL,
	I2CMODE_MPU9250
};
enum I2CMODE i2cMode = I2CMODE_NONE;


//MPU GLOBAL VARIABLES
static PIN_Handle hMpuPin;
static PIN_State MpuPinState;
static PIN_Config MpuPinConfig[] = { Board_MPU_POWER  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX, PIN_TERMINATE };

// MPU9250 uses its own I2C interface
static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
    .pinSDA = Board_I2C0_SDA1,
    .pinSCL = Board_I2C0_SCL1
};

// I2C global params
I2C_Handle i2cMPU; // INTERFACE FOR MPU9250 SENSOR
I2C_Params i2cMPUParams;
I2C_Handle i2c; // INTERFACE FOR OTHER SENSORS
I2C_Params i2cParams;



static void SetI2CParams() {


	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_400kHz;

	I2C_Params_init(&i2cMPUParams);
	i2cMPUParams.bitRate = I2C_400kHz;
    i2cMPUParams.custom = (uintptr_t)&i2cMPUCfg;
}

void Sensors_SwitchToMPU9250I2C() {
	if (i2cMode == I2CMODE_MPU9250) {
		return;
    }

    else if (i2cMode == I2CMODE_NORMAL) {
        I2C_close(i2c);
    }
    
    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    if (i2cMPU == NULL) {
        System_abort("Error Initializing I2CMPU\n");
    }
	i2cMode = I2CMODE_MPU9250;
}

void Sensors_SwitchToNormalI2C() {
	if (i2cMode == I2CMODE_NORMAL){
		return;
    }

    else if (i2cMode == I2CMODE_MPU9250) {
        I2C_close(i2cMPU);
    }
    
	i2c = I2C_open(Board_I2C0, &i2cParams);
    if (i2c == NULL) {
        System_abort("Error Initializing I2C\n");
    }
	            
	i2cMode = I2CMODE_NORMAL;
}



void SensorTaskFxn(UArg arg0, UArg arg1) {
    float ax, ay, az, gx, gy, gz;
    double pressure, temperature;
    
    //initialize sensor
    SetI2CParams();
    
    
    Sensors_SwitchToNormalI2C();
    
    //BMP280 SENSOR SETUP 
    bmp280_setup(&i2c);
    
    Sensors_SwitchToMPU9250I2C();
    
    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (hMpuPin == NULL) {
        System_abort("MPU pin open failed!");
    }
    
    //MPU PWER ON
    PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_ON);
    
    //WAIT 100MS FOR THE SENSOR TO POWER UP
	Task_sleep(100000 / Clock_tickPeriod); 
    System_printf("MPU9250: Power ON\n");
    System_flush();
    
    //MPU9250 SETUP AND CALIBRATION
    System_printf("MPU9250: Setup and calibration...\n");
    System_flush();
    
    mpu9250_setup(&i2cMPU);
    
    System_printf("MPU9250: Setup and calibration OK\n");
    System_flush();
	

    Task_sleep(100000/Clock_tickPeriod); 
    
    
    //read
	while (1) {
	    
        //if (read_sensor_values() == true)
        Sensors_SwitchToMPU9250I2C();
		mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
		
		uint32_t timestamp = Clock_tickPeriod * Clock_getTicks(); 
		if (mystate == READ_SENSOR || mystate == DETECTED)
		{
		    recordData(&read, ax, ay, az, gx, gy, gz, timestamp / 1000000);// timestamp in seconds
		}
        else if (mystate == RECORD_SENSOR) 
        {
            recordData(&train, ax, ay, az, gx, gy, gz, timestamp / 1000000);// timestamp in seconds
        }
        else if (mystate == TEMPERATURE) 
        {
   
            Sensors_SwitchToNormalI2C();
            bmp280_get_data(&i2c, &pressure, &temperature);
            setTempnPress(temperature, pressure);
        }
        
        setAccelerationTxt(ax, ay, az);
        setGyroTxt(gx, gy, gz);
    	Task_sleep(SENSOR_SAMPLING_DELAY / Clock_tickPeriod);
    	//Event_pend(globalEvents, READ_SERNSOR_VALUES, Event_Id_NONE, BIOS_WAIT_FOREVER);
    	
    
	}
	// power off
    // PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_OFF); 
}

void startSensorTask() {
    
   initializeData(&read, &train);
   Task_Params params;
   Task_Handle handle;
   Task_Params_init(&params);
   params.stackSize = STACKSIZE;
   params.stack = &SensorTaskStack; 
   params.priority = 2;
   handle = Task_create((Task_FuncPtr)SensorTaskFxn, &params, NULL);
   if (handle == NULL) {
       System_abort("Acceleration sensor task create failed");
   }
}

