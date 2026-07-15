#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "fifo_buffer/fifo_buffer.h"
#include "task_panel.h"
#include "task_logger.h"


#define PAGE_LOGMENU_SZ 100

extern volatile uint16_t log_fifo_rx_pointer;
extern Typedef_LoggerRecord rec_disp;
extern void (*current_page)(void* arg);
extern TypedefEnum_ButtonStates buttonState;

struct PageLog_menuItem{
    char label[10];
    int record_num; // это номер в фифо
};

static struct PageLog_menuItem pageLog_menu[3]; 
static                     int pageRecordListCursorVer = 0;
static                     int pageRecordListFirstLine = 0;

void PageLog_RecordList(void* arg);
void PageLog_RecordDisp(void* arg);


static inline void PanelLogCheckLimits(int* cursorVPos, int* firstLinePos, size_t menuSize, int vCnt)
{
    int vInd = vCnt - 1;

    if (*cursorVPos < 1)
    {
        *cursorVPos = 1;
        *firstLinePos = *firstLinePos - 1;
        if (*firstLinePos < 0)
            *firstLinePos = 0;
    }

    if (*cursorVPos > vInd)
    {
        *cursorVPos = vInd;
        *firstLinePos = *firstLinePos + 1;
        if (*firstLinePos > (int)menuSize - vCnt)
            *firstLinePos = (int)menuSize - vCnt;
    }
}


static inline void PanelLogDrawCursor( int* cursorVPos, char* p)
{
        p[0 +  20 * (*cursorVPos)] = '[';
        p[19 + 20 * (*cursorVPos)] = ']';

}


void PageLog_UpdateRecordList(){
    
    size_t fsz = vTask_logger_get_count_AC1();
    if(fsz>PAGE_LOGMENU_SZ) fsz=PAGE_LOGMENU_SZ;

    for (size_t i = 0; i < 3; i++)
    {
        Typedef_LoggerRecord tmp;
        int index =  (fsz-1) - (int)i - pageRecordListFirstLine ;
        int index_to_read = (PAGE_LOGMENU_SZ-1) - index;
        vTask_logger_read_AC1(&tmp, index_to_read);

        snprintf(pageLog_menu[i].label, 9, "%02d.%02d.%02d" ,
              tmp.timeStart.timestamp_day,tmp.timeStart.timestamp_month,tmp.timeStop.timestamp_minute
        );

        pageLog_menu[i].record_num = index;
    }

    current_page = PageLog_RecordList;
}




void PageLog_RecordList(void* arg){
    char* pntr = (char*)arg;

    PanelLogCheckLimits(&pageRecordListCursorVer, &pageRecordListFirstLine, PAGE_LOGMENU_SZ+1 , 4);

   
    snprintf(pntr, 81,
        "ARHIV SObITIY DC1    %03d     %10s  %03d     %10s  %03d     %10s",
        pageLog_menu[0].record_num,pageLog_menu[0].label,
        pageLog_menu[1].record_num,pageLog_menu[1].label,
        pageLog_menu[2].record_num,pageLog_menu[2].label
    );

    PanelLogDrawCursor(&pageRecordListCursorVer, pntr);

    switch (buttonState)
    {
    case KEY_UP:
        pageRecordListCursorVer--;
        if(pageRecordListCursorVer==0) PageLog_UpdateRecordList();
        break;

    case KEY_DOWN:
        pageRecordListCursorVer++;
        if(pageRecordListCursorVer>3) PageLog_UpdateRecordList();
        break;
    case KEY_ENTER:
         current_page = PageLog_RecordDisp;
        break;
    }

}

void PageLog_RecordDisp(void* arg){

    char* pntr = (char*)arg;

    Typedef_LoggerRecord rec;
    Typedef_LoggerRecord * r = &rec;

    int index_to_read = (PAGE_LOGMENU_SZ-1) - pageLog_menu[pageRecordListCursorVer-1].record_num;
        
    vTask_logger_read_AC1(r, index_to_read);


    snprintf(&pntr[ 0], 20, "%03d             %s", pageLog_menu[pageRecordListCursorVer-1].record_num, "DC1");
    snprintf(&pntr[20], 20, "        DD.MM.HH.SS");
    snprintf(&pntr[40], 20, "START   %02d.%02d.%02d.%02d",r->timeStart.timestamp_day, r->timeStart.timestamp_hour, r->timeStart.timestamp_minute, r->timeStart.timestamp_second);
    snprintf(&pntr[60], 20, "STOP    %02d.%02d.%02d.%02d",r->timeStop.timestamp_day,  r->timeStop.timestamp_hour,  r->timeStop.timestamp_minute,  r->timeStop.timestamp_second);


     switch (buttonState)
    {
    case KEY_UP:
        break;

    case KEY_DOWN:
        break;
    case KEY_ENTER:
        PageLog_UpdateRecordList();
        break;
    }
}