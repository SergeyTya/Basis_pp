#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "M204D08AA.h"
#include "task_panel.h"
#include "mbsupport.h"

static void Page_Logo(void *arg);
static void Page_Ac1Indi(void *arg);
static void Page_Ac2Indi(void *arg);
static void PageDc1Indi(void *arg);
static void PageDc2Indi(void *arg);
static void Page_Ac1Setup(void *arg);
static void Page_Dc1Setup(void *arg);
static void Page_Ac2Setup(void *arg);
static void Page_Dc2Setup(void *arg);
static void Page_Ac1AdvancedSetup(void *arg);
static void Page_Ac2AdvancedSetup(void *arg);
static void Page_Dc1AdvancedSetup(void *arg);
static void Page_Dc2AdvancedSetup(void *arg);
static void EndPoint(void *arg);
static void flash_cursor(char *pntr, size_t pos);

static inline void Page_AcIndiTemplate(uint16_t *(*foo)(uint8_t adr), int acnum, void *arg);
static inline void Page_DcIndiTemplate(uint16_t *(*foo)(uint8_t adr), int dcnum, void *arg);
static inline void Page_AcSetupTemplate(TypeDef_MB_Holding *(*foo)(uint8_t adr), int acnum, void *arg);
static inline void Page_DcSetupTemplate(TypeDef_MB_Holding *(*foo)(uint8_t adr), int dcnum, void *arg);
static inline void Page_AdvancedSetupTemplate(TypeDef_MB_Holding *(*foo)(uint8_t adr), void *arg);
static inline void onError(void * pntr);

static char displayMemory[80] = {0};
static char shadowDisplayMemory[80] = {0};
static bool displayUpdateHarBit = false;

static void DisplayUpdater();
static void (*pages[])(void *arg) = {
    Page_Logo,
    Page_Ac1Indi,
    PageDc1Indi,
    PageDc2Indi,
    Page_Ac1Setup,
    Page_Ac2Setup,
    Page_Dc1Setup,
    Page_Dc2Setup,
    Page_Ac1AdvancedSetup,
    EndPoint};

volatile TypedefEnum_Position current_position = PAGE9;
volatile TypedefEnum_ButtonStates buttonStates = KEY_NO;

xSemaphoreHandle xDisplayUpdaterSemaphore;

volatile size_t menu_cur_pos = 0;

#define LOAD_DISPLAY_DATA_ASYNC(displayMemoryShadow)             \
    {                                                            \
        xSemaphoreTake(xDisplayUpdaterSemaphore, portMAX_DELAY); \
        memcpy(displayMemory, displayMemoryShadow, 80);          \
        xSemaphoreGive(xDisplayUpdaterSemaphore);                \
    }

void vTask_Panel(__attribute__((unused)) void *argument)
{

    vTaskDelay(1000);
    M204D08AA_DisplayInit();
    memset(displayMemory, 80, 0);
    vSemaphoreCreateBinary(xDisplayUpdaterSemaphore);

    xTaskCreate(DisplayUpdater, "DisplayUpdater", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);

    while (1)
    {
        pages[current_position](shadowDisplayMemory);

        // main buttons handler
        switch (buttonStates)
        {
        default:
            break;
        }

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

        xSemaphoreTake(xDisplayUpdaterSemaphore, portMAX_DELAY);
        displayUpdateHarBit = !displayUpdateHarBit;
        M204D08AA_UpdateDisplayFromBuffer(displayMemory);
        xSemaphoreGive(xDisplayUpdaterSemaphore);
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        vTaskDelay(5);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        vTaskDelay(100);
    }
}

static void Page_Ac1Indi(void *arg)
{
    Page_AcIndiTemplate(GetHoldingPntrByAdrFromAC1, 1, arg);
}

static void Page_Ac2Indi(void *arg)
{
    Page_AcIndiTemplate(GetHoldingPntrByAdrFromAC2, 2, arg);
}

static void PageDc1Indi(void *arg)
{
    Page_DcIndiTemplate(GetHoldingPntrByAdrFromDC1, 1, arg);
}

static void PageDc2Indi(void *arg)
{
    Page_DcIndiTemplate(GetHoldingPntrByAdrFromDC2, 2, arg);
}

static void Page_Ac1Setup(void *arg)
{
    Page_AcSetupTemplate(GetHoldingByAdrFromAC1, 1, arg);
}

static void Page_Ac2Setup(void *arg)
{
    Page_AcSetupTemplate(GetHoldingByAdrFromAC2, 2, arg);
}

static void Page_Dc1Setup(void *arg)
{
    Page_DcSetupTemplate(GetHoldingByAdrFromDC1, 1, arg);
}

static void Page_Dc2Setup(void *arg)
{
    Page_DcSetupTemplate(GetHoldingByAdrFromDC2, 2, arg);
}

static void Page_Ac1AdvancedSetup(void *arg)
{
    Page_AdvancedSetupTemplate(GetHoldingByAdrFromAC1, arg);
}

static void Page_Ac2AdvancedSetup(void *arg)
{
    // Page_AdvancedSetupTemplate( GetHoldingPntrByAdrFromAC2 , arg);
}

static void Page_Dc1AdvancedSetup(void *arg)
{
    //  Page_AdvancedSetupTemplate( GetHoldingPntrByAdrFromDC1 , arg);
}

static void Page_Dc2AdvancedSetup(void *arg)
{
    // Page_AdvancedSetupTemplate( GetHoldingPntrByAdrFromDC2 , arg);
}

static void EndPoint(void *arg)
{
    current_position = PAGE1;
}

static void Page_Logo(void *arg)
{
    // clear
    char *pntr = (char *)arg;
    memset(pntr, 0, 80);
    vTaskDelay(1);
    // set static
    snprintf(&pntr[0], sizeof(LG_NAME), LG_NAME);
}

void flash_cursor(char *pntr, size_t pos)
{

    if (displayUpdateHarBit)
    {
        pntr[pos] = 0;
    }
    else
    {
        if (pntr[pos] == 0x20)
            pntr[pos] = '_';
    }
}

volatile bool upkey;
volatile bool dwnkey;
volatile bool enterkey;

volatile bool leftkey;
volatile bool rightkey;

static inline void Page_AcIndiTemplate(uint16_t *(*foo)(uint8_t adr), int acnum, void *arg)
{
    char *pntr = (char *)arg;

    memset(pntr, 0, 80);
    snprintf(
        pntr,
        80,
        " AC%1d      %sU,B   %3d  %3d  %3d I,A   %3d  %3d  %3d F,Hz  %3d P,kBA %3d", acnum, LG_NAME,
        *foo(240),
        *foo(241),
        *foo(242),
        *foo(243),
        *foo(244),
        *foo(245),
        *foo(101),
        90);
}

static inline void Page_DcIndiTemplate(uint16_t *(*foo)(uint8_t adr), int dcnum, void *arg)
{
    char *pntr = (char *)arg;
    memset(pntr, 0, 80);
    uint16_t volt = *foo(211);
    uint16_t volt_d = volt / 10;
    uint16_t volt_p = volt - volt_d * 10;

    snprintf(pntr, 80,
             " DC%1d      %s                    U,B  %2d,%1d  P,kBT 90 I,A  %4d", dcnum, LG_NAME,
             volt_d, volt_p,
             *foo(210));
}

static inline void Page_AcSetupTemplate(TypeDef_MB_Holding *(*foo)(uint8_t adr), int acnum, void *arg)
{

    static const uint8_t pageAcTemplateCursorPos[] = {35, 33, 32, 31, 55, 53, 52, 51, 75, 73, 72, 71};
    static const uint16_t pageAcTemplateDlt[] = {1, 10, 100, 1000, 1, 10, 100, 1000, 1, 10, 100, 1000};
    static TypeDef_MB_Holding *pageAcTemplateAdr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    char *pntr = (char *)arg;

    memset(pntr, 0, 80);

    TypeDef_MB_Holding *Uref = foo(102);
    for (size_t i = 0; i < 4; i++)
    {
        pageAcTemplateAdr[i] = Uref;
    }
    TypeDef_MB_Holding *Fref = foo(101);
    for (size_t i = 4; i < 8; i++)
    {
        pageAcTemplateAdr[i] = Fref;
    }
    TypeDef_MB_Holding *Iref = foo(121);
    for (size_t i = 8; i < 12; i++)
    {
        pageAcTemplateAdr[i] = Iref;
    }

    snprintf(pntr, 80,
             " AC%1d      %sS  U,B     %3d,%1d    E  F,Hz    %3d,%1d    T  LimI,A  %3d,%1d   ", acnum, LG_NAME,
             *Uref->pntr / 10, *Uref->pntr - (*Uref->pntr / 10) * 10,
             *Fref->pntr / 10, *Fref->pntr - (*Fref->pntr / 10) * 10,
             *Iref->pntr / 10, *Iref->pntr - (*Iref->pntr / 10) * 10);

    if (menu_cur_pos >= sizeof(pageAcTemplateCursorPos))
        menu_cur_pos = 0;
    flash_cursor(pntr, pageAcTemplateCursorPos[menu_cur_pos]);

    TypeDef_MB_Holding *selected = pageAcTemplateAdr[menu_cur_pos];

    switch (buttonStates)
    {
    case KEY_UP:
        if (*selected->pntr < 9999 - pageAcTemplateDlt[menu_cur_pos])
        {
            selected->lock = true;
            *selected->pntr += pageAcTemplateDlt[menu_cur_pos];
        }
        buttonStates = KEY_NO;
        break;
    case KEY_DOWN:
        if (*selected->pntr >= 0 + pageAcTemplateDlt[menu_cur_pos])
        {
            selected->lock = true;
            *selected->pntr -= pageAcTemplateDlt[menu_cur_pos];
        }
        buttonStates = KEY_NO;
        break;
    case KEY_LONGENTER:
        if (Uref->lock)
            Uref->change_req = true;
        if (Iref->lock)
            Iref->change_req = true;
        if (Fref->lock)
            Fref->change_req = true;
        buttonStates = KEY_NO;
        break;
    case KEY_NO:
        break;

    default:
        if (!Uref->change_req)
            Uref->lock = false;
        if (!Iref->change_req)
            Iref->lock = false;
        if (!Fref->change_req)
            Fref->lock = false;
        break;
    }
}

static inline void Page_DcSetupTemplate(TypeDef_MB_Holding *(*foo)(uint8_t adr), int dcnum, void *arg)
{
    char *pntr = (char *)arg;
    static const uint8_t pageDcTemplateCursorPos[] = {56, 54, 53, 52, 76, 74, 73, 72}; // TODO select correct positions
    static const uint16_t pageDcTemplateDlt[] = {1, 10, 100, 1000, 1, 10, 100, 1000};
    static TypeDef_MB_Holding *pageDcTemplateAdr[] = {0, 0, 0, 0, 0, 0, 0, 0};

    memset(pntr, 0, 80);

    TypeDef_MB_Holding *Uref = foo(102);
    for (size_t i = 0; i < 4; i++)
    {
        pageDcTemplateAdr[i] = Uref;
    }
    TypeDef_MB_Holding *Iref = foo(101);
    for (size_t i = 4; i < 8; i++)
    {
        pageDcTemplateAdr[i] = Iref;
    }

    snprintf(pntr, 80,
             " DC%1d      %sS                   E   U,B     %3d,%1d   T   LimI,A  %3d,%1d   ", dcnum, LG_NAME,
             *Uref->pntr / 10, *Uref->pntr - (*Uref->pntr / 10) * 10,
             *Iref->pntr_base / 10, *Iref->pntr - (*Iref->pntr / 10) * 10);

    if (menu_cur_pos >= sizeof(pageDcTemplateCursorPos))
        menu_cur_pos = 0;
    flash_cursor(pntr, pageDcTemplateCursorPos[menu_cur_pos]);

    TypeDef_MB_Holding *selected = pageDcTemplateAdr[menu_cur_pos];

    switch (buttonStates)
    {
    case KEY_UP:
        if (*selected->pntr < 9999 - pageDcTemplateDlt[menu_cur_pos])
        {
            selected->lock = true;
            *selected->pntr += pageDcTemplateDlt[menu_cur_pos];
        }
        buttonStates = KEY_NO;
        break;
    case KEY_DOWN:
        if (*selected->pntr >= 0 + pageDcTemplateDlt[menu_cur_pos])
        {
            selected->lock = true;
            *selected->pntr -= pageDcTemplateDlt[menu_cur_pos];
        }
        buttonStates = KEY_NO;
        break;
    case KEY_LONGENTER:
        if (Uref->lock)
            Uref->change_req = true;
        if (Iref->lock)
            Iref->change_req = true;
        buttonStates = KEY_NO;
        break;
    case KEY_NO:
        break;

    default:
        if (!Uref->change_req)
            Uref->lock = false;
        if (!Iref->change_req)
            Iref->lock = false;
        break;
    }
}

static inline void Page_AdvancedSetupTemplate(TypeDef_MB_Holding *(*foo)(uint8_t adr), void *arg)
{
    static const uint8_t pageAdvancedSetupTemplateCursorPos[] = {9, 29, 31, 44, 69, 17, 16, 15, 14, 13}; // active display positions
    static const uint16_t pageAdvancedSetupTemplateDlt[] = {1, 1, 0, 0, 0, 10000, 1000, 100, 10, 1}; 

    char *pntr = (char *)arg; // display buffer pointer

    TypeDef_AdvancedMenuItem const (* menuStructure)[15] = acAdvancedMenu; //TODO as function parameter

    static uint16_t pageAdvancedSetupTemplateCat = 0; // use this value to navigate catalog
    static uint16_t pageAdvancedSetupTemplateParam = 0; // use this value to set parameters

    if(pageAdvancedSetupTemplateCat   >= acAdvancedMenuSize) pageAdvancedSetupTemplateCat = 0;
    // if(pageAdvancedSetupTemplateParam > sizeof(menuStructure)/sizeof(menuStructure[0])) pageAdvancedSetupTemplateParam = 0;
    
    // Get menuItem pointer
    TypeDef_AdvancedMenuItem  const *  menuItemSelected = &menuStructure[pageAdvancedSetupTemplateCat][pageAdvancedSetupTemplateParam];
    if(menuItemSelected->adr == 0) {pageAdvancedSetupTemplateParam = 0; return; };
    // Get holding pointer
    TypeDef_MB_Holding * holdingSelected = foo(menuItemSelected->adr);
    // Null check
    if(IS_HOLDING_NULL_POINTER(holdingSelected)) {
        onError(pntr);
        return;
    }

    uint16_t crntCursorPos  = pageAdvancedSetupTemplateCursorPos[menu_cur_pos]; 
    uint16_t crntDelta      = pageAdvancedSetupTemplateDlt[menu_cur_pos];
    uint16_t displayedValue = *holdingSelected->pntr;

    if(menuItemSelected->isPartly){
        uint8_t bit = menuItemSelected->bitNumber;
        displayedValue = (displayedValue & (1U << bit))>>bit;
    }

    memset(pntr, 0, 80);
    snprintf(pntr, 80,
             "KATALOG  %1d   %5d  PARAMETR %1d COXPAHiTbHAZAD               MONITORING",
             pageAdvancedSetupTemplateCat, displayedValue, pageAdvancedSetupTemplateParam);

    if (menu_cur_pos >= sizeof(pageAdvancedSetupTemplateCursorPos))
        menu_cur_pos = 0;
    flash_cursor(pntr, pageAdvancedSetupTemplateCursorPos[menu_cur_pos]);

    switch (buttonStates)
    {
    case KEY_NO:
        break;

    case KEY_ENTER:
        if (crntCursorPos == 31) 
        { // save button position
            if(holdingSelected->lock) holdingSelected->change_req = true; // mark holding to be sent to slave
        }
        if (crntCursorPos == 44)
        { // return button position
            if(!holdingSelected->change_req) holdingSelected->lock = false;
        }
        if (crntCursorPos == 69)
        { // exit button position
            if(!holdingSelected->change_req) holdingSelected->lock = false;
        }
        buttonStates = KEY_NO;
        break;
    case KEY_UP:
        if (crntDelta!= 0) // just check is it number, not button
        {
            if (crntCursorPos == 9)
            { // catalog counter
                if(!holdingSelected->change_req) holdingSelected->lock = false; // disable lock when change menu item, so value does not saved
                pageAdvancedSetupTemplateCat++;
            }else if (crntCursorPos == 29) 
            {// parameter counter
                if(!holdingSelected->change_req) holdingSelected->lock = false; // disable lock when change menu item, so value does not saved
                pageAdvancedSetupTemplateParam++;
            }else
            {// holding value
                holdingSelected->lock = true; // enable lock when change menu item, to prevent value changing during holdings update
                if(menuItemSelected->isPartly){ 
                    // bit modifying
                    *holdingSelected->pntr ^= ( 1U << menuItemSelected->bitNumber);
                }else{
                    *holdingSelected->pntr += pageAdvancedSetupTemplateDlt[menu_cur_pos];
                }
            }
        }
        buttonStates = KEY_NO;
        break;
    case KEY_DOWN:

        buttonStates = KEY_NO;
        break;

    default:
        break;
    }
}

static inline void onError(void * pntr){
    memset(pntr, 0, 80);
    snprintf(pntr, 80,
         "NULL POINTER ERROR"
    );
}

// https://radioaktiv.ru/custom_character_generator_for_hd44780.html