#ifndef TASK_PANEL_H_
#define TASK_PANEL_H_

#include "stdbool.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

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
    PAGE1,
    PAGE2,
    PAGE3,
    PAGE4,
    PAGE5,
    PAGE6,
    PAGE7,
    PAGE8,
    PAGE9,
    ENDPOINT
}TypedefEnum_Position;

typedef enum {
    KEY_NO,
    KEY_UP,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_ENTER,
    KEY_LONGENTER,
    KEY_AC1,
    KEY_AC2,
    KEY_DC1,
    KEY_DC2
}TypedefEnum_ButtonStates;


typedef struct 
{

    uint16_t adr;
    bool isPartly;
    uint8_t bitNumber;
    uint8_t bitsCount;

}TypeDef_AdvancedMenuItem;

extern const uint8_t acAdvancedMenuSize;
extern const TypeDef_AdvancedMenuItem acAdvancedMenu[ 4][15];
extern const TypeDef_AdvancedMenuItem dcAdvancedMenu[14][15];


#endif