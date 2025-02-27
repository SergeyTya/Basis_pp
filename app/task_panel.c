
#include <string.h>
#include <stdio.h>
#include "M204D08AA.h"
#include "task_panel.h"

static void Page1(void * arg);
static void Page2(void * arg);
static void Page3(void * arg);
static void Page4(void * arg);
static void EndPoint(void * arg);

static char displayMemory[80] = {0};
static void DisplayUpdater();
static void CounterUpdater(__attribute__((unused)) void *argument);
static void ListWithCursor_Draw(TypeDef_ListWithCursor * list);
static uint16_t dcnt = 0;
static void(*pages[])(void * arg) = {Page1, Page2, Page3, Page4, EndPoint};

enum postion{
    PAGE1,
    PAGE2,
    PAGE3,
    PAGE4,
    ENDPOINT
};

volatile enum postion current_position = PAGE1;

xSemaphoreHandle xDisplayUpdaterSemaphore;

#define LOAD_DISPLAY_DATA_ASYNC(displayMemoryShadow) { \
    xSemaphoreTake( xDisplayUpdaterSemaphore, portMAX_DELAY ); \
    memcpy(displayMemory, displayMemoryShadow, 80); \
    xSemaphoreGive( xDisplayUpdaterSemaphore); \
}

void vTask_Panel(__attribute__((unused)) void *argument){
    
    memset(displayMemory, 80, 0); 
    vSemaphoreCreateBinary( xDisplayUpdaterSemaphore ); 
   
    xTaskCreate(DisplayUpdater, "DisplayUpdater"     , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
   // xTaskCreate(CounterUpdater, "CounterUpdater"     , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    
    while (1)
    {
        char tmp[80]={0};
        dcnt++;
        pages[current_position](tmp);
        LOAD_DISPLAY_DATA_ASYNC(tmp);        
        vTaskDelay(10);
    }
}

void CounterUpdater(__attribute__((unused)) void *argument){
    while (1)
    {
        current_position ++ ;
        vTaskDelay(2000);
    }
}

void DisplayUpdater(__attribute__((unused)) void *argument)
{
    M204D08AA_DisplayInit();

    while (1)
    {
        xSemaphoreTake( xDisplayUpdaterSemaphore, portMAX_DELAY );
        M204D08AA_UpdateDisplayFromBuffer(displayMemory);
        xSemaphoreGive( xDisplayUpdaterSemaphore);
        vTaskDelay(300);
    }
}

TypeDef_ListWithCursor Page1List = {
    .items =  {"ÏÀÐÀÌÅÒÐ #1","ÏÀÐÀÌÅÒÐ #2","ÏÀÐÀÌÅÒÐ #3","New line 1","New line 2","New line 3","New line 4","New line 5" },
    .items_size = 8
};

static void Page1(void * arg){

    char * pntr = (char *) arg;
    memset(pntr, 0, 80);
    Page1List.disp = arg;
    ListWithCursor_Draw(&Page1List);
    
}

static void Page2(void * arg){
    memset(arg, 0, 80);
    snprintf(&((char *)arg)[20], 19, "P2 cnrt=%d", dcnt);
}

static void Page3(void * arg){
    memset(arg, 0, 80);
    snprintf(&((char *)arg)[40], 19, "P3 cnrt=%d", dcnt);
}

static void Page4(void * arg){
    char * pntr = (char *) arg;
    memset(arg, 0, 80);

    snprintf(&(pntr)[60], 19, "4 cnrt=%d", dcnt);
}

static void EndPoint(void * arg){
    current_position=PAGE1;
}



void ListWithCursor_Draw(TypeDef_ListWithCursor * list){
    if(list->cursor_pos>3){
        list->cursor_pos=3; list->first_line_pos++;
        if(list->first_line_pos>list->items_size) list->first_line_pos = list->items_size;
    };
    if(list->cursor_pos<0){
        list->cursor_pos=0; list->first_line_pos--;
        if(list->first_line_pos<0) list->first_line_pos = 0;
    }
    
    for (int i = 0; i < 4; i++)
    {
        int item = i + list->first_line_pos;
        if(item >= list->items_size-(4-i)) item = list->items_size-(4-i);
        if(item < 0+i) item = i;
    
        size_t len = strlen(list->items[item]);
        if(len>18) len = 18;
        memcpy(&list->disp[i*20+1], list->items[item], len);
    
        if(i==list->cursor_pos){
            list->disp[i*20]= '[';  list->disp[i*20+19]= ']';
        }
    }
}

//https://radioaktiv.ru/custom_character_generator_for_hd44780.html