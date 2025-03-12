#ifndef TASK_PANEL_H_
#define TASK_PANEL_H_



#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#define LG_NAME "12345 1234"

//#define LG_NAME "œ–»¬≈“ À»Àﬂ"

void vTask_Panel(__attribute__((unused)) void *argument);


typedef struct {
    int first_line_pos;
    int cursor_pos;
    int items_size;
    char * disp;
    char * const items [18];
    
}TypeDef_ListWithCursor;


#endif