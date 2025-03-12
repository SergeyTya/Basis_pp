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
static void Page_Ac1Setup(void * arg);
static void Page_Dc1Setup(void * arg);
static void Page_Ac2Setup(void * arg);
static void Page_Dc2Setup(void * arg);
static void Page_Ac1AdvancedSetup(void * arg);
static void Page_Ac2AdvancedSetup(void * arg);
static void Page_Dc1AdvancedSetup(void * arg);
static void Page_Dc2AdvancedSetup(void * arg);
static void EndPoint(void * arg);
static void flash_cursor(char * pntr, size_t pos);


static inline void Page_AcIndiTemplate( uint16_t * (*foo) (uint8_t adr) ,int acnum, void * arg );
static inline void Page_DcIndiTemplate( uint16_t * (*foo) (uint8_t adr) ,int dcnum, void * arg );
static inline void Page_AcSetupTemplate( uint16_t * (*foo) (uint8_t adr) ,int acnum, void * arg );
static inline void Page_DcSetupTemplate( uint16_t * (*foo) (uint8_t adr) ,int dcnum, void * arg );
static inline void Page_AdvancedSetupTemplate(uint16_t * (*foo) (uint8_t adr), void * arg);

static char displayMemory[80] = {0};
static char shadowDisplayMemory[80]={0};
static bool displayUpdateHarBit = false;

static void DisplayUpdater();
static void(*pages[])(void * arg) = {
    Page_Logo, 
    Page_Ac1Indi, 
    PageDc1Indi, 
    PageDc2Indi,  
    Page_Ac1Setup,
    Page_Ac2Setup,
    Page_Dc1Setup,
    Page_Ac1AdvancedSetup, 
    EndPoint};

enum postion{
    PAGE1,
    PAGE2,
    PAGE3,
    PAGE4,
    PAGE5,
    PAGE6,
    PAGE7,
    PAGE8,
    ENDPOINT
};

enum postion current_position = PAGE5;

xSemaphoreHandle xDisplayUpdaterSemaphore;

volatile size_t menu_cur_pos = 0;

#define LOAD_DISPLAY_DATA_ASYNC(displayMemoryShadow) { \
    xSemaphoreTake( xDisplayUpdaterSemaphore, portMAX_DELAY ); \
    memcpy(displayMemory, displayMemoryShadow, 80); \
    xSemaphoreGive( xDisplayUpdaterSemaphore); \
}

void vTask_Panel(__attribute__((unused)) void *argument){

    vTaskDelay(1000); 
    M204D08AA_DisplayInit();
    memset(displayMemory, 80, 0); 
    vSemaphoreCreateBinary( xDisplayUpdaterSemaphore ); 
   
    xTaskCreate(DisplayUpdater, "DisplayUpdater"     , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    
    while (1)
    {
        pages[current_position](shadowDisplayMemory);
        LOAD_DISPLAY_DATA_ASYNC(shadowDisplayMemory);        
        vTaskDelay(10);
    }
}

#include "stm32f4xx.h"

void DisplayUpdater(__attribute__((unused)) void *argument)
{
    vTaskDelay(1000); 
    while (1)
    {
        
        xSemaphoreTake( xDisplayUpdaterSemaphore, portMAX_DELAY );
        displayUpdateHarBit = !displayUpdateHarBit;
        M204D08AA_UpdateDisplayFromBuffer(displayMemory);
        xSemaphoreGive( xDisplayUpdaterSemaphore);
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        vTaskDelay(5);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        vTaskDelay(100);
        
    }
}

static void Page_Ac1Indi(void * arg){
    Page_AcIndiTemplate( GetHoldingPntrByAdrFromAC1 , 1,arg);
}

static void Page_Ac2Indi(void * arg){
    Page_AcIndiTemplate( GetHoldingPntrByAdrFromAC2 , 2,arg);
}

static void PageDc1Indi(void * arg){
    Page_DcIndiTemplate( GetHoldingPntrByAdrFromDC1 , 1,arg);
}

static void PageDc2Indi(void * arg){
    Page_DcIndiTemplate( GetHoldingPntrByAdrFromDC2 , 2,arg);
}

static void Page_Ac1Setup(void * arg){
   Page_AcSetupTemplate( GetHoldingPntrByAdrFromAC1 , 1,arg);
}

static void Page_Ac2Setup(void * arg){
   Page_AcSetupTemplate( GetHoldingPntrByAdrFromAC2 , 2, arg);
}

static void Page_Dc1Setup(void * arg){
    Page_DcSetupTemplate( GetHoldingPntrByAdrFromDC1 , 1,arg);
}

static void Page_Dc2Setup(void * arg){
    Page_DcSetupTemplate( GetHoldingPntrByAdrFromDC2 , 2,arg);
}

static void Page_Ac1AdvancedSetup(void * arg){
    Page_AdvancedSetupTemplate( GetHoldingPntrByAdrFromAC1 , arg);
}

static void Page_Ac2AdvancedSetup(void * arg){
    Page_AdvancedSetupTemplate( GetHoldingPntrByAdrFromAC2 , arg);
}

static void Page_Dc1AdvancedSetup(void * arg){
    Page_AdvancedSetupTemplate( GetHoldingPntrByAdrFromDC1 , arg);
}

static void Page_Dc2AdvancedSetup(void * arg){
    Page_AdvancedSetupTemplate( GetHoldingPntrByAdrFromDC2 , arg);
}

static void EndPoint(void * arg){
    current_position=PAGE1;
}

static void Page_Logo(void * arg){
    // clear
    char * pntr = (char *) arg;
    memset(pntr, 0, 80);
    vTaskDelay(1);
    // set static
    snprintf(&pntr[0], sizeof(LG_NAME), LG_NAME);
}

void flash_cursor(char * pntr, size_t pos){
    
    if(displayUpdateHarBit) {
        pntr[pos] = 0;
    }
    else{
       if( pntr[pos] == 0x20 ) 
       pntr[pos] = '_';
    }
}

volatile bool  upkey;
volatile bool dwnkey;

static inline void Page_AcIndiTemplate(uint16_t * (*foo) (uint8_t adr) ,int acnum,  void * arg){
    char * pntr = (char *) arg;
    
    memset(pntr, 0, 80);
    snprintf(
        pntr, 
        80, 
        " AC%1d      %sU,B   %3d  %3d  %3d I,A   %3d  %3d  %3d F,Hz  %3d P,kBA %3d",  acnum, LG_NAME,
        *foo(240),
        *foo(241),
        *foo(242),
        *foo(243),
        *foo(244),
        *foo(245),
        *foo(101),
        90
    );
}

static inline void Page_DcIndiTemplate(uint16_t * (*foo) (uint8_t adr) ,int dcnum,  void * arg){
    char * pntr = (char *) arg;
    memset(pntr, 0, 80);
    uint16_t volt = *foo(211);
    uint16_t volt_d = volt/10;
    uint16_t volt_p = volt - volt_d * 10;

    snprintf(pntr, 80, 
        " DC%1d      %s                    U,B  %2d,%1d  P,kBT 90 I,A  %4d" ,dcnum, LG_NAME,
        volt_d, volt_p,
        *foo(210)
    ); 
}

static inline void Page_AcSetupTemplate( uint16_t * (*foo) (uint8_t adr) ,int acnum,  void * arg ){

    static const uint8_t  pageAcTemplateCursorPos[] = {35,33,32,31, 55,53,52,51, 75,73,72,71};
    static const uint16_t pageAcTemplateDlt[] = {1,10,100,1000, 1,10,100,1000, 1,10,100,1000};
    static uint16_t * pageAcTemplateAdr[] = {0,0,0,0, 0,0,0,0, 0,0,0,0};
 
     char * pntr = (char *) arg;
     
     memset(pntr, 0, 80);
 
     uint16_t * Uref = foo(102); 
     for (size_t i = 0; i < 4; i++)
     {
        pageAcTemplateAdr[i] = Uref;
     }
     uint16_t * Fref = foo(101);
     for (size_t i = 4; i < 8; i++)
     {
        pageAcTemplateAdr[i] = Fref;
     }
     uint16_t * Iref = foo(121);
     for (size_t i = 8; i < 12; i++)
     {
        pageAcTemplateAdr[i] = Iref;
     }

     snprintf(pntr, 80, 
         " AC%1d      %sS  U,B     %3d,%1d    E  F,Hz    %3d,%1d    T  LimI,A  %3d,%1d   ",  acnum, LG_NAME,
         *Uref/10,*Uref-(*Uref/10)*10, 
         *Fref/10,*Fref-(*Fref/10)*10, 
         *Iref/10,*Iref-(*Iref/10)*10
     );

     if(menu_cur_pos >= sizeof(pageAcTemplateCursorPos)) menu_cur_pos = 0;
     flash_cursor(pntr, pageAcTemplateCursorPos[menu_cur_pos]);
 
     if(upkey){
        if(*pageAcTemplateAdr[menu_cur_pos] < 9999-pageAcTemplateDlt[menu_cur_pos]){
            *pageAcTemplateAdr[menu_cur_pos] += pageAcTemplateDlt[menu_cur_pos];    
        } 
     }
 
     if(dwnkey){
        if(*pageAcTemplateAdr[menu_cur_pos] >= 0 + pageAcTemplateDlt[menu_cur_pos] ){
            *pageAcTemplateAdr[menu_cur_pos] -= pageAcTemplateDlt[menu_cur_pos];
        } 
     }

    upkey = false;
    dwnkey = false;
}

static inline void Page_DcSetupTemplate(uint16_t * (*foo) (uint8_t adr) ,int dcnum, void * arg){
    char * pntr = (char *) arg;
    static const uint8_t  pageDcTemplateCursorPos[] = {56,54,53,52, 76,74,73,72}; // TODO select correct positions
    static const uint16_t pageDcTemplateDlt[] = {1,10,100,1000, 1,10,100,1000};
    static uint16_t * pageDcTemplateAdr[] = {0,0,0,0, 0,0,0,0};

    memset(pntr, 0, 80);

    uint16_t * Uref = foo(102); 
    for (size_t i = 0; i < 4; i++)
    {
       pageDcTemplateAdr[i] = Uref;
    }
    uint16_t * Iref = foo(101); 
    for (size_t i = 4; i < 8; i++)
    {
       pageDcTemplateAdr[i] = Iref;
    }

    snprintf(pntr, 80, 
        " DC%1d      %sS                   E   U,B     %3d,%1d   T   LimI,A  %3d,%1d   ", dcnum , LG_NAME,
        *Uref/10,*Uref-(*Uref/10)*10, 
        *Iref/10,*Iref-(*Iref/10)*10
    );

    if(menu_cur_pos >= sizeof(pageDcTemplateCursorPos)) menu_cur_pos = 0;
    flash_cursor(pntr, pageDcTemplateCursorPos[menu_cur_pos]);

    if(upkey){
       if(*pageDcTemplateAdr[menu_cur_pos] < 9999-pageDcTemplateDlt[menu_cur_pos]){
           *pageDcTemplateAdr[menu_cur_pos] += pageDcTemplateDlt[menu_cur_pos];    
       } 
    }

    if(dwnkey){
       if(*pageDcTemplateAdr[menu_cur_pos] >= 0 + pageDcTemplateDlt[menu_cur_pos] ){
           *pageDcTemplateAdr[menu_cur_pos] -= pageDcTemplateDlt[menu_cur_pos];
       } 
    }

   upkey = false;
   dwnkey = false;
}

static inline void Page_AdvancedSetupTemplate(uint16_t * (*foo) (uint8_t adr), void * arg){
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




//https://radioaktiv.ru/custom_character_generator_for_hd44780.html