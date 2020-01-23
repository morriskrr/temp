#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "../Constants.h"
#include "../Prototypes.h"

#define MAX_OPTIONS 6
#define MAX_CALLBACKS 4

typedef void (*MenuOptionFxn)(void);         // function pointer for actions
typedef void (*DrawMenuFxn)(tContext*);      // function pointer for drawing 

typedef struct NextMenu *pMenu;              // pointer (gives warning)

// options struct 
typedef struct {                            
	pMenu next;                             // pointer for next Menu 
	MenuOptionFxn actions[MAX_CALLBACKS];   // calls these fuctions when option selected
	const char *text;                       // menu option's name
} MenuOption;


// main struct 
typedef struct {                            
	const unsigned char numOptions;
	unsigned char selectedOption;           
	DrawMenuFxn drawFxn;                    // draw graphics for selected menu option
	const MenuOption options[MAX_OPTIONS];  // options struct
} Menu;



// prototypes
int isLastOption(const MenuOption opt);
Menu *GetFirstMenu();



#endif // DISPLAY_H_
