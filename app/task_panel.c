#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "M204D08AA.h"
#include "task_panel.h"
#include "mbsupport.h"


static void Page_Logo(void * arg);
static void Page_Ac1Indi(void * arg);
static void Page_Ac2Indi(void * arg);
static void PageDc1Indi(void * arg);
static void PageDc2Indi(void * arg);
static void Page_Setup(void * arg);
static void Page_Ac1Setup(void * arg);
static void Page_Dc1Setup(void * arg);
static void Page_Ac2Setup(void * arg);
static void Page_Dc2Setup(void * arg);
static void EndPoint(void * arg);
static void flash_cursor(char * pntr, size_t pos);

static char displayMemory[80] = {0};
static char shadowDisplayMemory[80]={0};
static bool displayUpdateHarBit = false;

static void DisplayUpdater();
static void CounterUpdater(__attribute__((unused)) void *argument);
static void(*pages[])(void * arg) = {
    Page_Logo, 
    Page_Ac1Indi, 
    PageDc1Indi, 
    PageDc2Indi,  
    Page_Ac1Setup,
    Page_Dc1Setup,
    Page_Setup, 
    EndPoint};

enum postion{
    PAGE1,
    PAGE2,
    PAGE3,
    PAGE4,
    PAGE5,
    PAGE6,
    PAGE7,
    ENDPOINT
};

enum postion current_position = PAGE1;

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
        pages[current_position](shadowDisplayMemory);
        LOAD_DISPLAY_DATA_ASYNC(shadowDisplayMemory);        
        vTaskDelay(10);
    }
}

// void CounterUpdater(__attribute__((unused)) void *argument){
//     while (1)
//     {
//         current_position ++ ;
//         vTaskDelay(2000);
//     }
// }

void DisplayUpdater(__attribute__((unused)) void *argument)
{
    M204D08AA_DisplayInit();

    while (1)
    {
        xSemaphoreTake( xDisplayUpdaterSemaphore, portMAX_DELAY );
        displayUpdateHarBit = !displayUpdateHarBit;
        M204D08AA_UpdateDisplayFromBuffer(displayMemory);
        xSemaphoreGive( xDisplayUpdaterSemaphore);
        vTaskDelay(150);
    }
}


static void Page_Logo(void * arg){
    // clear
    char * pntr = (char *) arg;
    memset(pntr, 0, 80);
    // set static
    snprintf(&pntr[0], 20, LG_NAME);
}

void flash_cursor(char * pntr, size_t pos){
    
    if(displayUpdateHarBit) {
        pntr[pos] = 0;
    }else{
       if( pntr[pos] == 0x20 ) 
       pntr[pos] = '_';
    }
}



volatile size_t menu_cur_pos = 0;
static void Page_Ac1Indi(void * arg){

    char * pntr = (char *) arg;
    
    memset(pntr, 0, 80);
    snprintf(
        pntr, 
        60, 
        "U,B   %3d  %3d  %3d I,A   %3d  %3d  %3d F,Hz  %3d P,kBA %3d",
        GET_HOLDING_VALUE_BY_ADR_FROM_AC1(240),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC1(241),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC1(242),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC1(243),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC1(244),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC1(245),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC1(101),
        90
    );
}

static void Page_Ac2Indi(void * arg){

    char * pntr = (char *) arg;
    
    memset(pntr, 0, 80);
    snprintf(
        pntr, 
        60, 
        "U,B   %3d  %3d  %3d I,A   %3d  %3d  %3d F,Hz  %3d P,kBA %3d",
        GET_HOLDING_VALUE_BY_ADR_FROM_AC2(240),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC2(241),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC2(242),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC2(243),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC2(244),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC2(245),
        GET_HOLDING_VALUE_BY_ADR_FROM_AC2(101),
        90
    );
}


static void PageDc1Indi(void * arg){
   
    char * pntr = (char *) arg;
    memset(pntr, 0, 80);
    uint16_t volt = GET_HOLDING_VALUE_BY_ADR_FROM_DC1(211);
    uint16_t volt_d = volt/10;
    uint16_t volt_p = volt - volt_d * 10;

    snprintf(pntr, 80, 
        "          %s DC1                 U,B  %2d,%1d  P,kBT 90 I,A  %4d",
        LG_NAME,
        volt_d, volt_p,
        GET_HOLDING_VALUE_BY_ADR_FROM_DC1(210)
    ); 
}

static void PageDc2Indi(void * arg){
   
    char * pntr = (char *) arg;
    memset(pntr, 0, 80);
    uint16_t volt = GET_HOLDING_VALUE_BY_ADR_FROM_DC2(211);
    uint16_t volt_d = volt/10;
    uint16_t volt_p = volt - volt_d * 10;

    snprintf(pntr, 80, 
        "          %s DC2                 U,B  %2d,%1d  P,kBT 90 I,A  %4d",
        LG_NAME,
        volt_d, volt_p,
        GET_HOLDING_VALUE_BY_ADR_FROM_DC2(210)
    ); 
}

static void Page_Ac1Setup(void * arg){
    uint8_t cursor_pos[] = {35,33,32,31, 55,53,52,51, 75,73,72,71,};

    char * pntr = (char *) arg;
    memset(pntr, 0, 80);

    uint16_t Uref = GET_HOLDING_VALUE_BY_ADR_FROM_AC1(102);
    uint16_t Fref = GET_HOLDING_VALUE_BY_ADR_FROM_AC1(101);
    uint16_t Iref = GET_HOLDING_VALUE_BY_ADR_FROM_AC1(121);

    snprintf(pntr, 80, 
        " AC1      %sS  U,B     %3d,%1d    E  F,Hz    %3d,%1d    T  LimI,A  %3d,%1d   ", LG_NAME,
        Uref/10,Uref-(Uref/10)*10, 
        Fref/10,Fref-(Fref/10)*10, 
        Iref/10,Iref-(Iref/10)*10
    );

    if(menu_cur_pos >= sizeof(cursor_pos)) menu_cur_pos = 0;
    flash_cursor(pntr, cursor_pos[menu_cur_pos]);
}

static void Page_Dc1Setup(void * arg){
    char * pntr = (char *) arg;
    memset(pntr, 0, 80);
    snprintf(pntr, 80, 
        " DC1      XXXXX XXXXS                   E   U,B     %3d,%2d  T   LimI,A  %3d,%2d  ",
        123,33, 123,33
    );
}

static void Page_Setup(void * arg){
    uint8_t cursor_pos[] = {26,27,28, 31,32,33, 36,37,38, 46,47,48, 56,57,58};
    char * pntr = (char *) arg;
    memset(pntr, 0, 80);
    snprintf(pntr, 80, 
        "�������  %1d %5d,%3d�������� %1d ��������������               ����������",
        0, 12345, 333, 1
    );
    if(menu_cur_pos >= sizeof(cursor_pos)) menu_cur_pos = 0;
    flash_cursor(pntr, cursor_pos[menu_cur_pos]);
}

static void EndPoint(void * arg){
    current_position=PAGE1;
}



//https://radioaktiv.ru/custom_character_generator_for_hd44780.html