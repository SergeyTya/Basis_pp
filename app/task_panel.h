#ifndef TASK_PANEL_H_
#define TASK_PANEL_H_

#include "stdbool.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "panelConfig.h"

#include "locale.h"



void hwDriveHartBit_led1();
void hwDriveHartBit_led2();



void vTask_Panel(__attribute__((unused)) void *argument);


typedef struct {
    int first_line_pos;
    int cursor_pos;
    int items_size;
    char * disp;
    char * const items [18];
    
}TypeDef_ListWithCursor;

typedef enum {
    KEY_NO     = 0x7c33,
    KEY_LONGNO = ~0x7c33,
    KEY_UP    = 0x7433,
    KEY_DOWN  = 0x3c33,
    KEY_RIGHT = 0x7833,
    KEY_LEFT  = 0x5c33,
    KEY_ENTER = 0x6c33,
    KEY_LONGENTER = ~0x6c33,
    KEY_AC1 = 0x7c32,
    KEY_AC2 = 0x7c31,
    KEY_DC1 = 0x7c23,
    KEY_DC2 = 0x7c13,
    KEY_AC1LONG = ~0x7c32,
    KEY_AC2LONG = ~0x7c31,
    KEY_DC1LONG = ~0x7c23,
    KEY_DC2LONG = ~0x7c13,
    KEY_MENU = 0x7c20,
    KEY_LOGO = ~0x3433,// updown
}TypedefEnum_ButtonStates;


typedef struct 
{

    uint16_t adr; // modbus addres
    bool isBitfieldBit; // only one bit need to be displayed in this item
    uint8_t bitNumber;  // bit number
    uint8_t bitsCount;  // bit count (not used)

}TypeDef_AdvancedMenuItem;


typedef struct 
{

    const char label[10];
    uint32_t * val;
    int limHi;
    int limLo;
    int temVl;
    bool disabled;
    bool enableLim;
    bool modified;

    const uint32_t * options;
    const size_t options_len;

    void (* itemChangedEvent) ();
     
}TypeDef_ConfigMenuItem;

extern const uint8_t acAdvancedMenuSize;
extern const TypeDef_AdvancedMenuItem acAdvancedMenu[ 4][15];
extern const TypeDef_AdvancedMenuItem dcAdvancedMenu[14][15];
extern Typedef_PanelConfig panelConfig;
extern const size_t configMenuSize;
extern TypeDef_ConfigMenuItem configMenu[];
extern const TypeDef_ConfigMenuItem nullMenuItem;


#endif