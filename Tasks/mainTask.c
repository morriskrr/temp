#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include "../Constants.h"
#include "../Prototypes.h"
#include "wireless/address.h"
#include "wireless/comm_lib.h"


#define STACKSIZE 2048
char mainTaskStack[STACKSIZE];

// Handles



void SwitchStateTo_IDLE() {
    mystate = IDLE;
}

void SwitchStateTo_READ_SENSOR() {
    mystate = READ_SENSOR;
}

void SwitchStateTo_RECORD_SENSOR() {
    mystate = RECORD_SENSOR;
}

void SwitchStateTo_MSG_WAITING() {
    mystate = MSG_WAITING;
}

void SwitchStateTo_TEMPERATURE() {
    mystate = TEMPERATURE;
}



void mainTaskFxn(UArg arg0, UArg arg1) {
    int gesture;
    

    while(1) {
        switch (mystate)
        {
            case IDLE:

                //Event_pend(globalEvents, MESSAGE_RECEIVED, Event_id_none, BIOS_WAIT_FOREVER);   // (handle, andMask, orMask, Timeout)
                //state = MSG_WAITING;
                break;
        
            case READ_SENSOR:
   
                gesture = calculateGesture(&read, &train);
                if(gesture) {
                    mystate = DETECTED;
                }
                break;

            case RECORD_SENSOR:   
                
                break;
                
            case DETECTED:
                buzz(SWEEP_UP);
                Task_sleep(1000000 / Clock_tickPeriod); // show gesture for 2 sec 
                mystate = READ_SENSOR;
                Event_post(globalEvents, UPDATE_SCREEN); // korjaa sen ku teksti jumittuu
    
                break;
                
            case TEMPERATURE:

                break;

            case MSG_WAITING:

                break;
        }
        Task_sleep(100000 / Clock_tickPeriod); // 100 ms
    }
}


void startMainTask() {
    
    Task_Params params;
    Task_Handle handle;
    Task_Params_init(&params);
    params.stackSize = STACKSIZE;
    params.stack = &mainTaskStack; 
    params.priority = 2;
    handle = Task_create((Task_FuncPtr)mainTaskFxn, &params, NULL);
    if (handle == NULL) {
        System_abort("Main task create failed");
    }
}

