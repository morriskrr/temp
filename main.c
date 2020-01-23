#include <stdio.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>

/* my files */
#include "Board.h"
#include "Prototypes.h"
#include "Constants.h"
#include "wireless/comm_lib.h"
#include "eventdefs.h"

// Handles
Display_Handle hDisplay;
Event_Handle globalEvents;



static PIN_Handle hButtonMenu;  
static PIN_State bStateMenu;
PIN_Config buttonMenu[] = {
    Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,  
    PIN_TERMINATE
};

static PIN_Handle hButtonShut;
static PIN_State bStateShut;
PIN_Config buttonShut[] = { 
    Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

PIN_Config buttonWake[] = { 
    Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PINCC26XX_WAKEUP_NEGEDGE,
    PIN_TERMINATE 
};
static PIN_Handle ledHandle;
static PIN_State ledState;
PIN_Config ledConfig[] = { 
    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE 
};


void SetPowerOff() {
    Display_clear(hDisplay);
    Display_close(hDisplay);
    Task_sleep(100000 / Clock_tickPeriod);
	PIN_close(hButtonShut);
    PINCC26XX_setWakeup(buttonWake);
	Power_shutdown(NULL, 0);
}


int main(void) {
    
    
    // Alustetaan laite
    Board_initGeneral();
    Board_initI2C();
    
    hButtonMenu = PIN_open(&bStateMenu, buttonMenu);
    if (!hButtonMenu) {
        System_abort("Error initializing button pins\n");
    }
    
    ledHandle = PIN_open(&ledState, ledConfig);
    if (!ledHandle) {
        System_abort("Error initializing LED pins\n");
    }
    
    if (PIN_registerIntCb(hButtonMenu, &buttonMenuFxn) != 0) {
        System_abort("Error registering button callback function");
    }
    
    hButtonShut = PIN_open(&bStateShut, buttonShut);
    if (!hButtonShut) {
        System_abort("Error initializing button shut pins\n");
    }
    if (PIN_registerIntCb(hButtonShut, &buttonShutFxn) != 0) { 
    System_abort("Error registering button callback function");
    }    
    
    globalEvents = Event_create(NULL, NULL);
    if (globalEvents == NULL) {
    	System_abort("Failed to create event.\n");
    }

    
    Init6LoWPAN(); // call this before networking
    
    //Start all tasks
    startDisplayTask();
    startSensorTask();
    startMainTask();
    startBuzzerTask();
    
    
    
    System_printf("Starting BIOS...\n");
    System_flush();
    BIOS_start();
    return (0);
}


