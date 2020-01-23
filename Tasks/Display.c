#include "../Constants.h"
#include "../Prototypes.h"
#include "Display.h"

Menu mainMenu;          
Menu GestureMenuRead;

Menu GestureMenuTrain = {
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawGestureMenuTrain,
	.options = {
			{&GestureMenuRead, {SwitchStateTo_READ_SENSOR, NULL, NULL, NULL}, ""},
			{NULL, {NULL, NULL, NULL, NULL}, ""},
			{NULL, {NULL, NULL, NULL, NULL}, ""},
			{NULL, {NULL, NULL, NULL, NULL}, ""}
	}
};


Menu GestureMenuRead = {
	.numOptions = 2,
	.selectedOption = 0,
	.drawFxn = DrawGestureMenuRead,
	.options = {
			{&GestureMenuTrain, {SwitchStateTo_RECORD_SENSOR, NULL, NULL, NULL}, ""},
			{&mainMenu, {SwitchStateTo_IDLE, NULL, NULL, NULL}, ""},
			{NULL, {NULL, NULL, NULL, NULL}, ""},
			{NULL, {NULL, NULL, NULL, NULL}, ""}
	}
};


Menu TemperatureMenu = {
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawTemperatureMenu,
	.options = {
			{&mainMenu, {SwitchStateTo_IDLE, NULL, NULL, NULL}, "back"},
			{NULL, {NULL, NULL, NULL, NULL}, ""},
			{NULL, {NULL, NULL, NULL, NULL}, ""},
			{NULL, {NULL, NULL, NULL, NULL}, ""}
	}
};

Menu MessageMenu = {
	.numOptions = 1,
	.selectedOption = 0,
	.drawFxn = DrawMessageMenu,
	.options = {
			{&mainMenu, {SwitchStateTo_IDLE, NULL, NULL, NULL}, "back"},
			{NULL, {NULL, NULL, NULL, NULL}, ""},
			{NULL, {NULL, NULL, NULL, NULL}, ""},
			{NULL, {NULL, NULL, NULL, NULL}, ""}
	}
};


Menu mainMenu = {
	.numOptions = 4,
	.selectedOption = 0,
	.drawFxn = DrawMainMenu,
	.options = {
			{&GestureMenuRead, {SwitchStateTo_READ_SENSOR, NULL, NULL, NULL}, "GESTURE"},
			{&TemperatureMenu, {SwitchStateTo_TEMPERATURE, NULL, NULL, NULL}, "TEMPERATURE"},
			{&MessageMenu, {SwitchStateTo_MSG_WAITING, NULL, NULL, NULL}, "MESSAGES"},
			{NULL, {SetPowerOff, NULL, NULL, NULL}, "POWER OFF"},
			{NULL, {NULL, NULL, NULL, NULL}, ""}
	}
};


Menu *GetFirstMenu() {
    return &mainMenu;
}

int isLastOption(const MenuOption opt)
{
	return opt.actions[0] == NULL && opt.next == NULL && !strlen(opt.text);
}

