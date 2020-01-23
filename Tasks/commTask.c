#include <stdio.h>
#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include "wireless/comm_lib.h"
#include "../Constants.h"
#include "../Prototypes.h"

#define STACKSIZE 1024
char commTaskStack[STACKSIZE];

int sendMsg = 0;

void commTaskFxn(UArg arg0, UArg arg1) {
   char payload[16];
   uint16_t senderAddr = IEEE80154_MY_ADDR;
   int32_t result = StartReceive6LoWPAN();
   if(result != true) {
      System_abort("Wireless receive start failed");
   }
   while (true) {
            
        if (GetTXFlag() == false && sendMsg == 1) { // lahettaa viestin kun ele havaittu
            strcpy(payload, "Gesture detect!");
            Send6LoWPAN(IEEE80154_SERVER_ADDR, (uint8_t*)payload, 16);
            StartReceive6LoWPAN(); // siirtyy vastaanotto tilaan
            sendMsg = 0;
        }
        
        if (GetRXFlag()) {
           // Receive network message
           memset(payload,0,16);
           Receive6LoWPAN(&senderAddr, payload, 16);
           displayMessage(payload);
           System_printf(payload);
           System_flush();
        }
    }
}

void startCommTask() {
    Task_Params params;
    Task_Handle handle;
    Task_Params_init(&params);
    params.stackSize = STACKSIZE;
    params.stack = &commTaskStack; 
    params.priority = 1;
    handle = Task_create((Task_FuncPtr)commTaskFxn, &params, NULL);
    if (handle == NULL) {
        System_abort("Message task create failed");
    }
}

