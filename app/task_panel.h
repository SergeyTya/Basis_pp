#ifndef TASK_PANEL_H_
#define TASK_PANEL_H_

#include "stdbool.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "panelConfig.h"

#define LG_NAME "12345 1234"

//#define LG_NAME "������ ����"



void vTask_Panel(__attribute__((unused)) void *argument);


typedef struct {
    int first_line_pos;
    int cursor_pos;
    int items_size;
    char * disp;
    char * const items [18];
    
}TypeDef_ListWithCursor;

typedef enum {
    KEY_NO    = 0,
    KEY_UP    = 8,
    KEY_DOWN  = 2,
    KEY_RIGHT = 6,
    KEY_LEFT  = 4,
    KEY_ENTER = 5,
    KEY_LONGENTER = 7,
    KEY_AC1 = 10,
    KEY_AC2 = 11,
    KEY_DC1 = 12,
    KEY_DC2 = 13
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
    bool enable;
    bool enableLim;
    bool modified;

    const uint32_t * options;
    const size_t options_len;
     
}TypeDef_ConfigMenuItem;

extern const uint8_t acAdvancedMenuSize;
extern const TypeDef_AdvancedMenuItem acAdvancedMenu[ 4][15];
extern const TypeDef_AdvancedMenuItem dcAdvancedMenu[14][15];
extern Typedef_PanelConfig panelConfig;
extern size_t configMenuSize;
extern TypeDef_ConfigMenuItem configMenu[];
extern const TypeDef_ConfigMenuItem nullMenuItem;

// label size 3
#define LABEL_AC1 "AC1"
#define LABEL_AC2 "AC2"
#define LABEL_DC1 "DC1"
#define LABEL_DC2 "DC2"

#endif