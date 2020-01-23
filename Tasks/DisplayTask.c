#include <stdio.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <stdint.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/mw/grlib/grlib.h>
#include "Display.h"
#include "../Constants.h"
#include "../Prototypes.h"


#define STACKSIZE 2048
char displayTaskStack[STACKSIZE];

// Handles
Display_Handle hDisplay;
Clock_Handle buttonClock;


// Global variables
Menu *currentMenu = NULL;
int buttonHasReset = 1;
static int batteryPercentage = 0;

// MPU sensor values
char AccelerationXText[16] = "-";
char AccelerationYText[16] = "-";
char AccelerationZText[16] = "-";
char GyroXText[16] = "-";
char GyroYText[16] = "-";
char GyroZText[16] = "-";

// Temp and press
char TempText[16] = "-";
char PressText[16] = "-";

// messages
char MessageText[16] = ""; 
char MessageArray[8][16];
int MessageArrayCount = 0;


void ResetButtons(UArg arg0) {
	buttonHasReset = 1;
	Clock_stop(buttonClock);
}

void buttonMenuFxn(PIN_Handle handle, PIN_Id pinId) {
	Clock_start(buttonClock);
	
	if (buttonHasReset){
		unsigned char selectedOption = currentMenu->selectedOption;
		int fn = 0;
		while (currentMenu->options[selectedOption].actions[fn] != NULL) { // käy läpi kaikki "actions" ja suorittaa ne 
			currentMenu->options[selectedOption].actions[fn]();     
			++fn;
		}
		if (currentMenu->options[selectedOption].next != NULL) {    
			currentMenu = currentMenu->options[selectedOption].next;
		}
		Event_post(globalEvents, BUTTON_PRESSED);
		buttonHasReset = 0;
	}
}

void buttonShutFxn(PIN_Handle handle, PIN_Id pinId) {
	Clock_start(buttonClock);
	
	if (buttonHasReset) {
		unsigned char selectedOption = currentMenu->selectedOption;
		if (!isLastOption(currentMenu->options[selectedOption + 1])) {
			currentMenu->selectedOption++;
		} 
		else {
			currentMenu->selectedOption = 0;
		}
		Event_post(globalEvents, BUTTON_PRESSED);
		buttonHasReset = 0;
	}
}

void DrawMainMenu(tContext *pContext) {
	int i;
	
	Display_print1(hDisplay, 0, 0, "MENU         %i%%", batteryPercentage);

	for (i = 0; i < currentMenu->numOptions; ++i) {
        int x = 2;
		if (currentMenu->selectedOption == i) x++;  // ilmoittaa valitun option
		
		Display_print1(hDisplay, i+3, x, ">%s", currentMenu->options[i].text); //x liikuttaa oikealle i alaspäin
	}
	GrLineDraw(pContext,0,10,96,10);
	GrLineDraw(pContext,0,92,91,92);
	GrLineDraw(pContext,91,92,96,87);
}

void DrawGestureMenuRead(tContext *pContext) {

    Display_print1(hDisplay, 0, 0, "READING     %i%%", batteryPercentage);

	GrLineDraw(pContext,0,10,96,10);
	GrLineDraw(pContext,0,92,91,92);
	GrLineDraw(pContext,91,92,96,87); 

	Display_print0(hDisplay, 2, 0, "Acceleration:"); 
	Display_print0(hDisplay, 3, 0, AccelerationXText);
	Display_print0(hDisplay, 4, 0, AccelerationYText);
	Display_print0(hDisplay, 5, 0, AccelerationZText);
	Display_print0(hDisplay, 7, 0, "Gyroscope:"); 
	Display_print0(hDisplay, 8, 0, GyroXText); 
	Display_print0(hDisplay, 9, 0, GyroYText); 
	Display_print0(hDisplay, 10, 0, GyroZText); 

}

void DrawGestureMenuTrain(tContext *pContext) {
    
    Display_print1(hDisplay, 0, 0, "RECORDING   %i%%", batteryPercentage);

	GrLineDraw(pContext,0,10,96,10);
	GrLineDraw(pContext,0,92,91,92);
	GrLineDraw(pContext,91,92,96,87);

	Display_print0(hDisplay, 2, 0, "Acceleration:"); 
	Display_print0(hDisplay, 3, 0, AccelerationXText);
	Display_print0(hDisplay, 4, 0, AccelerationYText);
	Display_print0(hDisplay, 5, 0, AccelerationZText);
	Display_print0(hDisplay, 7, 0, "Gyroscope:"); 
	Display_print0(hDisplay, 8, 0, GyroXText); 
	Display_print0(hDisplay, 9, 0, GyroYText); 
	Display_print0(hDisplay, 10, 0, GyroZText);      
    
}

void DrawTemperatureMenu(tContext *pContext) {
    int i;
    
    Display_print1(hDisplay, 0, 0, "TEMPERATURE  %i%%", batteryPercentage);
	for (i = 0; i < currentMenu->numOptions; ++i) {
        int x = 10;
		if (currentMenu->selectedOption == i) x++;  // ilmoittaa valitun option
		
		Display_print1(hDisplay, i+10, x, ">%s", currentMenu->options[i].text); // x liikuttaa oikealle i alaspäin
	}
	GrLineDraw(pContext,0,10,96,10);
	GrLineDraw(pContext,0,92,91,92);
	GrLineDraw(pContext,91,92,96,87); 

	Display_print0(hDisplay, 2, 1, "Lampotila:");
	Display_print0(hDisplay, 4, 1, TempText);
	Display_print0(hDisplay, 6, 1, "Ilmanpaine:");
	Display_print0(hDisplay, 8, 1, PressText);	
}

void DrawMessageMenu(tContext *pContext) {
    int i;
    
    Display_print1(hDisplay, 0, 0, "MESSAGES    %i%%", batteryPercentage);
	for (i = 0; i < currentMenu->numOptions; ++i) {
        int x = 10;
		if (currentMenu->selectedOption == i) x++;  // ilmoittaa valitun option
		
		Display_print1(hDisplay, i+10, x, ">%s", currentMenu->options[i].text); // x liikuttaa oikealle i alaspäin
	}
	GrLineDraw(pContext,0,10,96,10);
	GrLineDraw(pContext,0,92,91,92);
	GrLineDraw(pContext,91,92,96,87);     
    
}

void setAccelerationTxt(float ax, float ay, float az) {
    char sax[16]; char say[16]; char saz[16];
    
    sprintf(sax,"%.5f", ax);
    sprintf(say,"%.5f", ay);
    sprintf(saz,"%.5f", az);
    strcpy(AccelerationXText, sax);
    strcpy(AccelerationYText, say);
    strcpy(AccelerationZText, saz);
}

void setGyroTxt(float gx, float gy, float gz) {
    char sgx[16]; char sgy[16]; char sgz[16];
    
    sprintf(sgx,"%.5f", gx);
    sprintf(sgy,"%.5f", gy);
    sprintf(sgz,"%.5f", gz);
    strcpy(GyroXText, sgx);
    strcpy(GyroYText, sgy);
    strcpy(GyroZText, sgz);
}  

void setTempnPress(double temperature, double pressure) {
    char str1[16]; char str2[16];
    
    sprintf(str1,"%.1f C", temperature);
    strcpy(TempText, str1);
    sprintf(str2,"%.0f Pa", pressure);
    strcpy(PressText, str2);
}


void displayMessage(char* msg) {
    int i;
    //kopioi viestin array
    if (MessageArrayCount < 8) {
        strncpy(MessageArray[MessageArrayCount], msg, sizeof(MessageArray[MessageArrayCount]));
    }
    else {
        //shift
         for (i = 7; i > 0; i--) {
            strncpy(MessageArray[i], MessageArray[i-1], sizeof(MessageArray[i]));
        }
        strncpy(MessageArray[0], msg, sizeof(MessageArray[0]));
    }
    MessageArrayCount++;
}


void CheckBattery() {
	int i;
	uint32_t batteryReg = HWREG(AON_BATMON_BASE + AON_BATMON_O_BAT);
	float battery = (batteryReg & 0x00000700) >> 8;
	uint32_t frac = batteryReg & 0x000000FF;
	for (i = 7; i >= 0; --i) {
		battery += (frac >> i) & 1 ? pow2(-8 + i) : 0;
	}
	batteryPercentage = floor((battery / MAX_BAT_VOLTAGE) * 100);
}




void DisplayFxn(UArg arg0, UArg arg1) {
    
    Display_Params params;
    Clock_Params buttonClockParams;
    
    
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;
    hDisplay = Display_open(Display_Type_LCD, &params);
	if (hDisplay == NULL) {
		System_abort("Error initializing Display\n");
	}
    
    //BUTTON CLOCK
    Clock_Params_init(&buttonClockParams);
	buttonClockParams.period = 500000 / Clock_tickPeriod; // 0.5s
	buttonClock = Clock_create(ResetButtons, 0, &buttonClockParams, NULL);
	if (buttonClock == NULL) {
		System_abort("buttonClock create failed");
	}

    Display_clear(hDisplay);
    
    currentMenu = GetFirstMenu();
    
	while (1) {
		CheckBattery();
		tContext *context = DisplayExt_getGrlibContext(hDisplay);
		if (context) {
            if (mystate == READ_SENSOR || mystate == RECORD_SENSOR || mystate == TEMPERATURE) {   // printtaa sensorin arvoja ruutuun
                if (currentMenu != NULL) {
				    currentMenu->drawFxn(context);
                }
                GrFlush(context);
                Task_sleep(200000/Clock_tickPeriod);
            } 
            else {                                                                                                          
		        if (currentMenu != NULL) {
				    currentMenu->drawFxn(context);
		        }
		        GrFlush(context);
		        Event_pend(globalEvents, Event_Id_NONE, BUTTON_PRESSED | UPDATE_SCREEN, BIOS_WAIT_FOREVER);	// taski blockattuna täs kunnes nappia painetaan tai "| muut| "
		        Display_clear(hDisplay);
            }
	    }
	}
}

    
    
void startDisplayTask() {

    Task_Params params;
    Task_Handle handle;
    Task_Params_init(&params);
    params.stackSize = STACKSIZE;
    params.stack = &displayTaskStack; 
    params.priority = 2;
    handle = Task_create((Task_FuncPtr)DisplayFxn, &params, NULL);
    if (handle == NULL){
        System_abort("Display task create failed");
    } 
    System_flush();
}
