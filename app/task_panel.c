/*
 * Refer to manuals/blok-cxema.doc
 */
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "IDisplay.h"
#include "task_panel.h"
#include "mbsupport.h"
#include "panelConfig.h"
#include "task_master.h"


Typedef_PanelConfig panelConfig;
extern TypeDef_Master master;

static void Page_Logo(void* arg);
static void Page_Ac1Indi(void* arg);
static void Page_Ac2Indi(void* arg);
static void Page_Dc1Indi(void* arg);
static void Page_Dc2Indi(void* arg);
static void Page_Ac1Setup(void* arg);
static void Page_Dc1Setup(void* arg);
static void Page_Ac2Setup(void* arg);
static void Page_Dc2Setup(void* arg);
static void Page_Ac1AdvancedSetup(void* arg);
static void Page_Ac2AdvancedSetup(void* arg);
static void Page_Dc1AdvancedSetup(void* arg);
static void Page_Dc2AdvancedSetup(void* arg);
static void Page_Confirm(void* arg);
static void Page_SaveWarning(void* arg);
static void Page_Config(void* pntr);
static void Page_MenuItemEdit(void* arg);
static void Page_SlaveFault(char* pntr, const char* label, int code);
static void flash_cursor(char* pntr, size_t pos);
static void DisplayUpdater();

static inline void Page_AcIndiTemplate(uint16_t* (*foo)(uint8_t adr), int acnum, void* arg);
static inline void Page_DcIndiTemplate(uint16_t* (*foo)(uint8_t adr), int dcnum, void* arg);
static inline void Page_AcSetupTemplate(TypeDef_MB_Holding* (*foo)(uint8_t adr), int acnum, void* arg);
static inline void Page_DcSetupTemplate(TypeDef_MB_Holding* (*foo)(uint8_t adr), int dcnum, void* arg);
static inline void Page_AdvancedSetupTemplate(
    TypeDef_MB_Holding* (*foo)(uint8_t adr),                     // function for searching holding by addr
    void* arg,                                                   // display buffer pointer
    TypeDef_AdvancedMenuItem const (*menuStructureTemplate)[15], // menu item structure
    const char label[3],
    void (*backPointer)(void*),
    void (*indiPointer)(void*));

static inline void onError(void* pntr);

// pointer to current page to be displayed
static void (*current_page)(void* arg) = Page_Logo; // Page_Config; // Page_Logo;//Page_MenuItemEdit; // Page_Config; // Page_Logo;

static char displayMemory[80] = { 0 };       // use it for load data to display
static char shadowDisplayMemory[80] = { 0 }; // use it like buffer
static bool displayUpdateHarBit = false;   // this is for symbol blinking

volatile TypedefEnum_ButtonStates buttonState = KEY_NO; // put here button state
xSemaphoreHandle xDisplayUpdaterSemaphore;              // display update semaphore. Lock display buffer while transfer it to display
static size_t menu_cur_pos = 0;                         // Position of blinking cursor

void vTask_Panel(__attribute__((unused)) void* argument)
{


    DisplayInit();
    memset(displayMemory, 80, 0);
    vSemaphoreCreateBinary(xDisplayUpdaterSemaphore);

    xTaskCreate(DisplayUpdater, "DisplayUpdater", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    bool faultTrigger[5] = { false };

    while (1)
    {
        if (buttonState == KEY_LONGNO) buttonState = KEY_NO;
        // display page
        current_page(shadowDisplayMemory);

        // main buttons handler
        if (current_page != Page_Confirm && current_page != Page_SaveWarning)
        {
            switch (buttonState)
            {
            case KEY_AC1:
                if (panelConfig.enableAC1) {
                    if (current_page == Page_Ac1Indi) {
                        master.start_req_rdo[CONFIG_SLAVE_AC1] = true;
                    }
                    else {
                        current_page = Page_Ac1Indi;
                    }
                }
                break;
            case KEY_AC2:
                // if (panelConfig.enableAC2) {
                //     if (panelConfig.enableAC2) {
                //         if (current_page == Page_Ac2Indi) {
                //             master.start_req_rdo[CONFIG_SLAVE_AC2] = true;
                //         }
                //         else {
                //             current_page = Page_Ac2Indi;
                //         }
                //     }
                // }
                 master.start_req_rdo[CONFIG_SLAVE_AC2] = true;
                break;
            case KEY_DC1:
                if (panelConfig.enableDC1){
                    if (current_page == Page_Dc1Indi) {
                        master.start_req_rdo[CONFIG_SLAVE_DC1] = true;
                    }
                    else {
                        current_page = Page_Dc1Indi;
                    }
                }
                break;
            case KEY_DC2:
                if (panelConfig.enableDC2){ 
                    if (current_page == Page_Dc2Indi) {
                        master.start_req_rdo[CONFIG_SLAVE_DC2] = true;
                    }
                    else {
                        current_page = Page_Dc2Indi;
                    }
                }
                break;

            case KEY_AC1LONG:
                master.start_req_hw[CONFIG_SLAVE_AC1] = true;
                break;
            case KEY_AC2LONG:
                master.start_req_hw[CONFIG_SLAVE_AC2] = true;
            break;
            case KEY_DC1LONG:
                master.start_req_hw[CONFIG_SLAVE_DC1] = true;
                break;
            case KEY_DC2LONG:
                master.start_req_hw[CONFIG_SLAVE_DC2] = true;
            break;

            case KEY_MENU:
                //current_page = Page_Config;
                break;

            case KEY_LOGO:
                current_page = Page_Logo;
                break;

            default:
                break;
            }
        }

        // reset buttons
        buttonState = KEY_NO;

        // fault triger

        static void(*pages[])(void* arg) = { NULL,Page_Ac1Indi, Page_Ac2Indi, Page_Dc1Indi, Page_Dc2Indi };
        for (size_t i = 1; i < 5; i++)
        {
            if (master.fault_source[i] == true)
            {
                if (faultTrigger[i] == false)
                {
                    faultTrigger[i] = true;
                    current_page = pages[i];
                    break;
                }
            }
            else
            {
                faultTrigger[i] = false;
            }
        }

        // charge master wdg
        for (size_t i = 1; i < 5; i++)
        {
            master.master_wdg[5] = true;
        }

        // copy display data
        xSemaphoreTake(xDisplayUpdaterSemaphore, portMAX_DELAY);
        memcpy(displayMemory, shadowDisplayMemory, 80);
        xSemaphoreGive(xDisplayUpdaterSemaphore);
        vTaskDelay(30);

    }
}


/**
 *  @brief This RTOS task drives display
 */
void DisplayUpdater(__attribute__((unused)) void* argument)
{
    // can select parameter bug fixing 26/10/2025
    int update_time = 28;
    int counter_flash = 0; 
    int flash_time =300;
    int flash_counter_rst = flash_time/update_time;
    vTaskDelay(500);
    while (1)
    {
        xSemaphoreTake(xDisplayUpdaterSemaphore, portMAX_DELAY);
        if(counter_flash++>flash_counter_rst){
            counter_flash = 0;
            displayUpdateHarBit = !displayUpdateHarBit;

        };
        DisplayUpdateFromBuffer(displayMemory);
        xSemaphoreGive(xDisplayUpdaterSemaphore);
        vTaskDelay(update_time);
    }
}

static void Page_Ac1Indi(void* arg)
{
    Page_AcIndiTemplate(GetHoldingPntrByAdrFromAC1, 1, arg);
    panelConfig.active_slave = CONFIG_SLAVE_AC1;
}

static void Page_Ac2Indi(void* arg)
{
    Page_AcIndiTemplate(GetHoldingPntrByAdrFromAC2, 2, arg);
    panelConfig.active_slave = CONFIG_SLAVE_AC2;
}

static void Page_Dc1Indi(void* arg)
{
    Page_DcIndiTemplate(GetHoldingPntrByAdrFromDC1, 1, arg);
    panelConfig.active_slave = CONFIG_SLAVE_DC1;
}

static void Page_Dc2Indi(void* arg)
{
    Page_DcIndiTemplate(GetHoldingPntrByAdrFromDC2, 2, arg);
    panelConfig.active_slave = CONFIG_SLAVE_DC2;
}

static void Page_Ac1Setup(void* arg)
{
    Page_AcSetupTemplate(GetHoldingByAdrFromAC1, 1, arg);
    panelConfig.active_slave = CONFIG_SLAVE_AC1;
}

static void Page_Ac2Setup(void* arg)
{
    Page_AcSetupTemplate(GetHoldingByAdrFromAC2, 2, arg);
    panelConfig.active_slave = CONFIG_SLAVE_AC2;
}

static void Page_Dc1Setup(void* arg)
{
    Page_DcSetupTemplate(GetHoldingByAdrFromDC1, 1, arg);
    panelConfig.active_slave = CONFIG_SLAVE_DC1;
}

static void Page_Dc2Setup(void* arg)
{
    Page_DcSetupTemplate(GetHoldingByAdrFromDC2, 2, arg);
    panelConfig.active_slave = CONFIG_SLAVE_DC2;
}

static void Page_Ac1AdvancedSetup(void* arg)
{
    Page_AdvancedSetupTemplate(GetHoldingByAdrFromAC1, arg, acAdvancedMenu, LABEL_AC1, Page_Ac1Setup, Page_Ac1Indi);
    panelConfig.active_slave = CONFIG_SLAVE_AC1;
}

static void Page_Ac2AdvancedSetup(void* arg)
{
    Page_AdvancedSetupTemplate(GetHoldingByAdrFromAC2, arg, acAdvancedMenu, LABEL_AC2, Page_Ac2Setup, Page_Ac2Indi);
    panelConfig.active_slave = CONFIG_SLAVE_AC2;
}

static void Page_Dc1AdvancedSetup(void* arg)
{
    Page_AdvancedSetupTemplate(GetHoldingByAdrFromDC1, arg, dcAdvancedMenu, LABEL_DC1, Page_Dc1Setup, Page_Dc1Indi);
    panelConfig.active_slave = CONFIG_SLAVE_DC1;
}

static void Page_Dc2AdvancedSetup(void* arg)
{
    Page_AdvancedSetupTemplate(GetHoldingByAdrFromDC2, arg, dcAdvancedMenu, LABEL_DC2, Page_Dc2Setup, Page_Dc2Indi);
    panelConfig.active_slave = CONFIG_SLAVE_DC2;
}

extern float temp_ext;
static void Page_Logo(void* arg)
{
    // clear
    char* pntr = (char*)arg;
    memset(pntr, 0, 80);
    vTaskDelay(1);
    // set static
    int32_t t = ((int32_t)(temp_ext * 10.f)) / 10;
    snprintf(&pntr[0], sizeof(LG_NAME), LG_NAME);
   // snprintf(&pntr[70], 10, "Temp= %2d", t);

    if (buttonState == KEY_LONGENTER)
    {
        current_page = Page_Config;
    }
    panelConfig.active_slave = CONFIG_SLAVE_NONE;
}

void flash_cursor(char* pntr, size_t pos)
{
    if (displayUpdateHarBit)
    {
        if (pntr[pos] == 0x20 || pntr[pos] == 0)
        {
            pntr[pos] = '_';
        }
        else
        {
            pntr[pos] = ' ';
        }
    }
}

// Confirm message box
static void (*pageConfirmedRetPoint)(void* arg) = NULL; // point we will return after confirm finished
static bool pageConfirmedRetVal = false;                // trye if OK button selected
/**
 *  @brief Confirm message box function
 */
static void Page_Confirm(void* arg)
{
    static const uint8_t pageConfirmCursorPos[] = { 52, 45 };

    char* pntr = (char*)arg;
    memset(pntr, 0, 80);
    snprintf(pntr, 80,
        "                        %s?           %3s    %3s  ",
        LABEL_9_SAVE, LABEL_YES, LABEL_NO
    );

    if (menu_cur_pos > 1) // there is only 2 options
    {
        menu_cur_pos = 1;
    }
    flash_cursor(pntr, pageConfirmCursorPos[menu_cur_pos]);

    pageConfirmedRetVal = false; // reset return value every time
    switch (buttonState)
    {
    case KEY_LEFT: // navigate
        menu_cur_pos = 1;
        buttonState = KEY_NO;
        break;
    case KEY_RIGHT: // navigate
        menu_cur_pos = 0;
        buttonState = KEY_NO;
        break;
    case KEY_ENTER:
        pageConfirmedRetVal = menu_cur_pos == 1; // 0 - if OK
        current_page = pageConfirmedRetPoint;    // SET NEW POINTER
        buttonState = KEY_NO;
        break;
    default:
        break;
    }
}

static void (*pageSaveWarningRetPoint)(void* arg) = NULL; // point we will return when exit
/**
 *  @brief Warning message box
 */
static void Page_SaveWarning(void* arg)
{
    char* pntr = (char*)arg;

    memset(pntr, 0, 80);
    snprintf(pntr, 80, "|     %s      ||    %s    ||   %s   ||        OK        | ",
        LABEL_9_WARNING,   //9  
        LABEL_11_PARAMETERS,     //11  
        LABEL_13_NOTSAVED
    );

    flash_cursor(pntr, 69);

    switch (buttonState)
    {
    case KEY_ENTER:
        current_page = pageSaveWarningRetPoint;
        break;

    default:
        break;
    }
}

/**
 *  @brief Tmaplate for AC channel indicator (monitoring page)
 *
 *  @param  foo uint16_t *(*)(uint8_t) - Pointer to function which get holding pointer by modbus addres
 *  @param  acnum {int} - AC Channel number  (1 or 2)
 *  @param  arg {void*} - Display buffer pointer (char *)
 */
static inline void Page_AcIndiTemplate(uint16_t* (*foo)(uint8_t adr), int acnum, void* arg)
{
    char* pntr = (char*)arg;

    uint16_t U[3] = { *foo(240) / 10, *foo(241) / 10U, *foo(242) / 10 }; // 0.1V
    for (size_t i = 0; i < 3; i++)
    {
        if (U[i] > 999U)
            U[i] = 999U;
    }

    uint16_t I[3] = { *foo(243), *foo(244), *foo(245) }; // 0.1V
    for (size_t i = 0; i < 3; i++)
    {
        if (I[i] > 999U)
            I[i] = 999U;
    }

    uint16_t F = *foo(101) / 10;
    if (F > 999)
        F = 999U;

        // ADD Power estimation (BUG fixing 26/10/2025)
    uint16_t P = (U[0]*I[0]+U[1]*I[1]+U[2]*I[2])/1000;
    if(P>99)P=99;

    // process slave error code
    if (acnum == 1 && master.fault_source[1] == true)
    {
        Page_SlaveFault(pntr, LABEL_AC1, master.fault_code[1]);
    }
    else if (acnum == 2 && master.fault_source[2] == true)
    {
        Page_SlaveFault(pntr, LABEL_AC2, master.fault_code[2]);
    }
    // else if(master.master_wdg[acnum]==true){
    //     Page_SlaveFault(pntr, "CFL", 666);
    // }
    else
    {
        memset(pntr, 0, 80);
        snprintf(
            pntr, 80,
          // Remove source number (BUG fixing 26/10/2025)
          //  " AC%1d   %s  %sU,B   %3d  %3d  %3d I,A   %3d  %3d  %3d F,Hz  %3d P,kBA %3d",
          //  acnum,
            " AC   %s  %s U,B   %3d  %3d  %3d I,A   %3d  %3d  %3d F,Hz  %3d P,kBA %3d",
            master.master_wdg[acnum] == true ? "!" : " ",
            LG_NAME,
            U[0], U[1], U[2],
            I[0], I[1], I[2],
            F,
            P);
    }

    if (master.master_wdg[acnum] == true) {
        flash_cursor(pntr, 7);
    }

    menu_cur_pos = 0;

    if (master.master_wdg[acnum] == true) return;

    switch (buttonState)
    {
    case KEY_ENTER:
        if (acnum == 1)
        {
            current_page = Page_Ac1Setup;
        }
        if (acnum == 2)
        {
            current_page = Page_Ac2Setup;
        }
        break;

    default:
        break;
    }
}

/**
 *  @brief Tmaplate for DC channel indicator (monitoring page)
 *
 *  @param  foo uint16_t *(*)(uint8_t) - Pointer to function which get holding pointer by modbus addres
 *  @param  dcnum {int} - DC Channel number (1 or 2)
 *  @param  arg {void*} - Display buffer pointer (char *)
 */
static inline void Page_DcIndiTemplate(uint16_t* (*foo)(uint8_t adr), int dcnum, void* arg)
{
    char* pntr = (char*)arg;
    memset(pntr, 0, 80);
    uint16_t volt = *foo(211);
    uint16_t volt_d = volt / 10;
    if (volt_d > 99U)
        volt_d = 99U;
    uint16_t volt_p = volt - volt_d * 10;
    if (volt_p > 9)
        volt_p = 9;

    uint16_t I = *foo(210);
    if (I > 9000U)
        I = 9000U;

    menu_cur_pos = 0;

    if (dcnum == 1 && master.fault_source[3] == true)
    {
        Page_SlaveFault(pntr, LABEL_DC1, master.fault_code[3]);
    }
    else if (dcnum == 2 && master.fault_source[4] == true)
    {

        Page_SlaveFault(pntr, LABEL_DC2, master.fault_code[4]);
    }
    // else if(master.master_wdg[dcnum+2]==true){
    //     Page_SlaveFault(pntr, "CFL", 666);
    // }
    else
    {
        snprintf(pntr, 80,
            " DC%1d   %s  %s                    U,B  %2d,%1d  P,kBT 90 I,A  %4d",
            dcnum,
            master.master_wdg[dcnum + 2] ? "!" : " ",
            LG_NAME,
            volt_d, volt_p, I);
    }

    if (master.master_wdg[dcnum + 2] == true) {
        flash_cursor(pntr, 7);
    }

    if (master.master_wdg[dcnum + 2] == true) return;

    switch (buttonState)
    {
    case KEY_ENTER:
        if (dcnum == 1)
        {
            current_page = Page_Dc1Setup;
        }
        if (dcnum == 2)
        {
            current_page = Page_Dc2Setup;
        }
        break;

    default:
        break;
    }
}

/**
 *  @brief Template for AC channel setup page
 *
 *  @param  foo uint16_t *(*)(uint8_t) - Pointer to function which get holding pointer by modbus addres
 *  @param  dcnum {int} - AC Channel number (1 or 2)
 *  @param  arg {void*} - Display buffer pointer (char *)
 */
static inline void Page_AcSetupTemplate(TypeDef_MB_Holding* (*foo)(uint8_t adr), int acnum, void* arg)
{

    static const uint8_t pageAcTemplateCursorPos[] = { 31,   32,  33, 35,  51,   52,  53, 55,   71 , 72, 73  };
    static const uint16_t pageAcTemplateDlt[]      = { 1000, 100, 10, 1,   1000, 100, 10, 1 ,   100, 10, 1  };
    static TypeDef_MB_Holding* pageAcTemplateAdr[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    static bool pageAcSetupUnlocked;
    static bool pageAcTemplateIsOnConfirmWait;

    char* pntr = (char*)arg;

    memset(pntr, 0, 80);

    TypeDef_MB_Holding* Uref = foo(102);
    for (size_t i = 0; i < 4; i++)
    {
        pageAcTemplateAdr[i] = Uref;
    }
    TypeDef_MB_Holding* Fref = foo(101);
    for (size_t i = 4; i < 8; i++)
    {
        pageAcTemplateAdr[i] = Fref;
    }
    TypeDef_MB_Holding* Iref = foo(121);
    for (size_t i = 8; i < 12; i++)
    {
        pageAcTemplateAdr[i] = Iref;
    }

    // check if we are on save parameters confirm wait
    if (pageAcTemplateIsOnConfirmWait)
    {
        if (pageConfirmedRetVal)
        { // save
            if (Uref->lock)
                Uref->change_req = true;
            if (Iref->lock)
                Iref->change_req = true;
            if (Fref->lock)
                Fref->change_req = true;
        }
        else
        { // chancel
            Uref->change_req = false;
            Uref->lock = false;
            Iref->change_req = false;
            Iref->lock = false;
            Fref->change_req = false;
            Fref->lock = false;
        }
        pageAcTemplateIsOnConfirmWait = false; // acknowledge wait
    }

    uint16_t U[2] = { (*Uref->pntr / 10), *Uref->pntr - (*Uref->pntr / 10) * 10 };
    uint16_t F[2] = { (*Fref->pntr / 10), *Fref->pntr - (*Fref->pntr / 10) * 10 };
    // uint16_t I[2] = {(*Iref->pntr / 10), *Iref->pntr - (*Iref->pntr / 10) * 10};

    uint16_t I[2] = { *Iref->pntr , 0 };

    if (U[0] > 999U)
        U[0] = 999U;
    if (I[0] > 999U)
        I[0] = 999U;
    if (F[0] > 999U)
        F[0] = 999U;

    if (U[1] > 9U)
        U[1] = 9U;
    if (I[1] > 9U)
        I[1] = 9U;
    if (F[1] > 9U)
        F[1] = 9U;

    snprintf(pntr, 80,
      //  " AC%1d      %sS  U,B     %3d,%1d    E  F,Hz    %3d,%1d    T  LimI,A  %3d     ",
      //  acnum, 
      // Remove source number (BUG fixing 26/10/2025)
        " AC       %sS  U,B     %3d,%1d    E  F,Hz    %3d,%1d    T  LimI,A  %3d     ",
        LG_NAME,
        U[0], U[1],
        F[0], F[1],
        I[0]);

    if (menu_cur_pos >= sizeof(pageAcTemplateCursorPos))
        menu_cur_pos = 0;
    if (pageAcSetupUnlocked)
    {
        flash_cursor(pntr, pageAcTemplateCursorPos[menu_cur_pos]);
    }

    TypeDef_MB_Holding* selected = pageAcTemplateAdr[menu_cur_pos];

    switch (buttonState)
    {
    case KEY_UP:
        if (!pageAcSetupUnlocked)
            break;
        if (*selected->pntr < (9999 - pageAcTemplateDlt[menu_cur_pos]))
        {
            selected->lock = true; // lock parameter for changing
            *selected->pntr += pageAcTemplateDlt[menu_cur_pos];
        }
        break;
    case KEY_DOWN:
        if (!pageAcSetupUnlocked)
            break;
        if (*selected->pntr >= (0 + pageAcTemplateDlt[menu_cur_pos]))
        {
            selected->lock = true; // lock parameter for changing
            *selected->pntr -= pageAcTemplateDlt[menu_cur_pos];
        }
        break;
    case KEY_LONGENTER:
        // move to ac advanced param setup
        if (acnum == 1)
            current_page = Page_Ac1AdvancedSetup;
        if (acnum == 2)
            current_page = Page_Ac2AdvancedSetup;
        goto AC_PARAM_EXIT;
        break;

    case KEY_ENTER:
        if (Uref->lock || Iref->lock || Fref->lock)
        {
            // move to confirm page
            pageConfirmedRetPoint = current_page;
            pageConfirmedRetVal = false;
            pageAcTemplateIsOnConfirmWait = true;
            pageAcSetupUnlocked = false;
            current_page = Page_Confirm;
        }
        else
        {
            // move to ac indi page
            if (acnum == 1)
                current_page = Page_Ac1Indi;
            if (acnum == 2)
                current_page = Page_Ac2Indi;
            goto AC_PARAM_EXIT;
        }
        break;

    case KEY_NO:
        break;

    case KEY_LEFT:
        if (!pageAcSetupUnlocked)
            break;
        // move cursor
             if (menu_cur_pos == 0)
                {menu_cur_pos = 7;}
            else if (menu_cur_pos == 4)
                {menu_cur_pos = 10;}
            else if (menu_cur_pos == 8)
                {menu_cur_pos = 3;}
            else{ menu_cur_pos--;}
        break;

    case KEY_RIGHT:
        // move cursor
        if (!pageAcSetupUnlocked)
        {
            pageAcSetupUnlocked = true;
            menu_cur_pos = 3; // move to last position
            break;
        }
        if (menu_cur_pos == 10){
            menu_cur_pos = 0;
        }  else {
            menu_cur_pos++;
        }
        
        break;

    default: // AC1 //AC2 //DC1 //DC2
        goto AC_PARAM_EXIT;
        break;
    }
    return;

AC_PARAM_EXIT:
    // chancel all unsaved parameters
    bool needWarning = false;
    if (!Uref->change_req)
    {
        if (Uref->lock)
            needWarning = true;
        Uref->lock = false;
    }

    if (!Iref->change_req)
    {
        if (Uref->lock)
            needWarning = true;
        Iref->lock = false;
    }

    if (!Fref->change_req)
    {
        if (Uref->lock)
            needWarning = true;
        Fref->lock = false;
    }

    // lock page
    pageAcSetupUnlocked = false;

    if (needWarning)
    {
        pageSaveWarningRetPoint = current_page;
        current_page = Page_SaveWarning;
    }

    return;
}

static inline void Page_DcSetupTemplate(TypeDef_MB_Holding* (*foo)(uint8_t adr), int dcnum, void* arg)
{
    char* pntr = (char*)arg;
    static const uint8_t pageDcTemplateCursorPos[] = { 56, 54, 53, 52, 76, 75, 74, 73 }; // TODO select correct positions
    static const uint16_t pageDcTemplateDlt[] = { 1, 10, 100, 1000, 1, 10, 100, 1000 };
    static TypeDef_MB_Holding* pageDcTemplateAdr[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    static bool pageDcSetupUnlocked;
    static bool pageDcTemplateIsOnConfirmWait;

    memset(pntr, 0, 80);

    TypeDef_MB_Holding* Uref = foo(102);
    for (size_t i = 0; i < 4; i++)
    {
        pageDcTemplateAdr[i] = Uref;
    }
    TypeDef_MB_Holding* Iref = foo(101);
    for (size_t i = 4; i < 8; i++)
    {
        pageDcTemplateAdr[i] = Iref;
    }

    // check if we are on save parameters confirm wait
    if (pageDcTemplateIsOnConfirmWait)
    {
        if (pageConfirmedRetVal)
        { // save
            if (Uref->lock)
                Uref->change_req = true;
            if (Iref->lock)
                Iref->change_req = true;
        }
        else
        { // chancel
            Uref->change_req = false;
            Uref->lock = false;
            Iref->change_req = false;
            Iref->lock = false;
        }
        pageDcTemplateIsOnConfirmWait = false; // acknowledge wait
    }

    // snprintf(pntr, 80,
    //          " DC%1d      %sS                   E   U,B     %3d,%1d   T   LimI,A  %3d,%1d   ", dcnum, LG_NAME,
    //          *Uref->pntr / 10, *Uref->pntr - (*Uref->pntr / 10) * 10,
    //          *Iref->pntr_base / 10, *Iref->pntr - (*Iref->pntr / 10) * 10);

    snprintf(pntr, 80,
        " DC%1d      %sS                   E   U,B     %3d,%1d   T   LimI,A   %4d   ", dcnum, LG_NAME,
        *Uref->pntr / 10, *Uref->pntr - (*Uref->pntr / 10) * 10,
        *Iref->pntr_base);

    if (pageDcSetupUnlocked)
    {
        if (menu_cur_pos >= sizeof(pageDcTemplateCursorPos))
            menu_cur_pos = 0;
        flash_cursor(pntr, pageDcTemplateCursorPos[menu_cur_pos]);
    }

    TypeDef_MB_Holding* selected = pageDcTemplateAdr[menu_cur_pos];

    switch (buttonState)
    {
    case KEY_UP:
        if (!pageDcSetupUnlocked)
            break;
        if (*selected->pntr < (9999 - pageDcTemplateDlt[menu_cur_pos]))
        {
            selected->lock = true; // lock parameter for changing
            *selected->pntr += pageDcTemplateDlt[menu_cur_pos];
        }
        break;
    case KEY_DOWN:
        if (!pageDcSetupUnlocked)
            break;
        if (*selected->pntr >= (0 + pageDcTemplateDlt[menu_cur_pos]))
        {
            selected->lock = true; // lock parameter for changing
            *selected->pntr -= pageDcTemplateDlt[menu_cur_pos];
        }
        break;
    case KEY_LONGENTER:
        // move to ac advanced param setup
        if (dcnum == 1)
            current_page = Page_Dc1AdvancedSetup;
        if (dcnum == 2)
            current_page = Page_Dc2AdvancedSetup;
        goto DC_PARAM_EXIT;
        break;

    case KEY_ENTER:
        if (Uref->lock || Iref->lock)
        {
            // move to confirm page
            if (dcnum == 1)
                pageConfirmedRetPoint = Page_Dc1Setup;
            if (dcnum == 2)
                pageConfirmedRetPoint = Page_Dc2Setup;
            pageConfirmedRetVal = false;
            pageDcTemplateIsOnConfirmWait = true;
            pageDcSetupUnlocked = false;
            current_page = Page_Confirm;
        }
        else
        {
            // move to ac indi page
            if (dcnum == 1)
                current_page = Page_Dc1Indi;
            if (dcnum == 2)
                current_page = Page_Dc2Indi;
            goto DC_PARAM_EXIT;
        }
        break;

    case KEY_NO:
        break;

    case KEY_LEFT:
        if (!pageDcSetupUnlocked)
            break;
        // move cursor
        menu_cur_pos++;
        break;

    case KEY_RIGHT:
        // move cursor
        if (!pageDcSetupUnlocked)
        {
            pageDcSetupUnlocked = true;
            break;
        }
        if (menu_cur_pos != 0)
            menu_cur_pos--;
        break;

    default: // AC1 //AC2 //DC1 //DC2
        goto DC_PARAM_EXIT;
        break;
    }
    return;

DC_PARAM_EXIT:
    // chancel all unsaved parameters
    bool needWarning = false;
    if (!Uref->change_req)
    {
        if (Uref->lock)
            needWarning = true;
        Uref->lock = false;
    }

    if (!Iref->change_req)
    {
        if (Uref->lock)
            needWarning = true;
        Iref->lock = false;
    }

    // lock page
    pageDcSetupUnlocked = false;

    if (needWarning)
    {
        pageSaveWarningRetPoint = current_page;
        current_page = Page_SaveWarning;
    }

    return;
}

/***
 *  @brief Advanced menu universal (AD/DC) template
 *
 *  @param foo  {TypeDef_MB_Holding * (*)(uint8_t)} pointer to function for searching holding by addr
 *  @param arg  {char *} Display buffer pointer
 *  @param label {char[3]} Label to display in upper left corner
 *  @param backPointer {void (*)()} Pointer to setup function
 *  @param indiPointer {void (*)()} Pointer to monitoring function
 */
static inline void Page_AdvancedSetupTemplate(
    TypeDef_MB_Holding* (*foo)(uint8_t),
    void* arg,                                                   //
    TypeDef_AdvancedMenuItem const (*menuStructureTemplate)[15], // menu item structure
    const char label[3], 
    void (*backPointer)(),
    void (*indiPointer)())
{
    static volatile bool pageConfirmedRetValIsOnConfirmWait = false;
    static volatile bool  pageAcAdvancedSetupUnlocked = false;
                                                            //    0  1   2   3   4   5   6   7      8       9
    static const uint8_t pageAdvancedSetupTemplateCursorPos[] = { 9, 29,       33,   34,  35,  36, 37,  51, 44, 69,    }; // active display positions
    static const uint16_t pageAdvancedSetupTemplateDlt[]      = { 1,  1,    10000, 1000, 100,  10,  1,   0,  0,  0,     };

    char* pntr = (char*)arg; // display buffer pointer

    TypeDef_AdvancedMenuItem const(*menuStructure)[15] = menuStructureTemplate;

    static uint16_t pageAdvancedSetupTemplateCat = 0;   // use this value to navigate catalog
    static uint16_t pageAdvancedSetupTemplateParam = 0; // use this value to set parameter

    // lock state
    if(!pageAcAdvancedSetupUnlocked){
        menu_cur_pos = 8;
    }

    if (pageAdvancedSetupTemplateCat >= acAdvancedMenuSize)
        pageAdvancedSetupTemplateCat = 0;
    // if(pageAdvancedSetupTemplateParam > sizeof(menuStructure)/sizeof(menuStructure[0])) pageAdvancedSetupTemplateParam = 0;

    // Pointer to selected menu item
    TypeDef_AdvancedMenuItem const* menuItemSelected = &menuStructure[pageAdvancedSetupTemplateCat][pageAdvancedSetupTemplateParam];
    if (menuItemSelected->adr == 0)
    { // something is wrong and modbuss addres is 0, so return to initial position
        pageAdvancedSetupTemplateParam = 0;
        return;
    };
    // Get holding pointer
    TypeDef_MB_Holding* holdingSelected = foo(menuItemSelected->adr);
    // Null check
    if (IS_HOLDING_NULL_POINTER(holdingSelected))
    {
        onError(pntr);
        return;
    }

    // param save wait for confirm state
    if(pageConfirmedRetValIsOnConfirmWait){

        if (pageConfirmedRetVal)
        {
            if(holdingSelected->lock) holdingSelected->change_req = true; 
        }else{
            holdingSelected->change_req = false;
            holdingSelected->lock = false;  
        }
        pageConfirmedRetValIsOnConfirmWait = false;
    }

    uint16_t crntCursorPos = pageAdvancedSetupTemplateCursorPos[menu_cur_pos];
    uint16_t crntDelta = pageAdvancedSetupTemplateDlt[menu_cur_pos];
    uint16_t displayedValue = *holdingSelected->pntr;

    if (menuItemSelected->isBitfieldBit)
    { // item is bit field, so display only bit value
        uint8_t bit = menuItemSelected->bitNumber;
        displayedValue = (displayedValue & (1U << bit)) >> bit;
    }

    memset(pntr, 0, 80);

    snprintf(pntr, 80,
        // '|' symbol do not displayed at winstar display (BUG fixing 26/10/2025)
        "%s  %1d       %s%s %1d   %5d  %s      %s%s           ",
        LABEL_7_KATALOG,
        pageAdvancedSetupTemplateCat,
        label,
        LABEL_8_PARAMETR,
        pageAdvancedSetupTemplateParam, displayedValue,
        LABEL_5_NAZAD, LABEL_9_SAVE, LABEL_10_MONIT
    );

    if (menu_cur_pos >= sizeof(pageAdvancedSetupTemplateCursorPos))
        menu_cur_pos = 0;
    flash_cursor(pntr, pageAdvancedSetupTemplateCursorPos[menu_cur_pos]);

/*

       if (menu_cur_pos == 0)
                {menu_cur_pos = 7;}
            else if (menu_cur_pos == 4)
                {menu_cur_pos = 10;}
            else if (menu_cur_pos == 8)
                {menu_cur_pos = 3;}
            else{ menu_cur_pos--;}

*/


    switch (buttonState)
    {
    case KEY_RIGHT:
        if( !pageAcAdvancedSetupUnlocked ) {
            pageAcAdvancedSetupUnlocked = true;
            menu_cur_pos = 0;
            break;
        } 
        if (menu_cur_pos == 9)
            {menu_cur_pos = 0;}
        else
            {menu_cur_pos++;}
        break;

    case KEY_LEFT:
        if(!pageAcAdvancedSetupUnlocked) break;
        
        if (menu_cur_pos == 0)
            {menu_cur_pos = 9;}
        else
            {menu_cur_pos--;}
        break;


        break;

    case KEY_NO:
        break;

    case KEY_ENTER:
        if (crntCursorPos == 51)
        { // save button position
            if(!pageAcAdvancedSetupUnlocked) break;
            if (holdingSelected->lock){
                pageConfirmedRetValIsOnConfirmWait = true;
                pageAcAdvancedSetupUnlocked=false;
                pageConfirmedRetPoint = current_page;
                current_page = Page_Confirm;

            }
         //       holdingSelected->change_req = true; // mark holding to be sent to slave
        }
        if (crntCursorPos == 44)
        { // goto setup button position
            if (!holdingSelected->change_req)
            {
                holdingSelected->lock = false;
            }
            current_page = backPointer;
            pageAcAdvancedSetupUnlocked = false;
        }
        if (crntCursorPos == 69)
        { // goto indicators button position
            if (!holdingSelected->change_req)
            {
                holdingSelected->lock = false;
            }
            current_page = indiPointer;
            pageAcAdvancedSetupUnlocked = false;
        }
        buttonState = KEY_NO;
        
        break;
    case KEY_UP:
    case KEY_DOWN:
        if (!pageAcAdvancedSetupUnlocked) break;
        if (crntDelta != 0) // just check if it number, not button
        {
            if (crntCursorPos == 9)
            { // catalog counter
                if (!holdingSelected->change_req)
                    holdingSelected->lock = false; // disable lock when change menu item, so value does not saved
                if(buttonState==KEY_UP  ) pageAdvancedSetupTemplateCat++;
                if(buttonState==KEY_DOWN) pageAdvancedSetupTemplateCat--;
            }
            else if (crntCursorPos == 29)
            { // parameter counter
                if (!holdingSelected->change_req)
                    holdingSelected->lock = false; // disable lock when change menu item, so value does not saved
                if(buttonState==KEY_UP  )pageAdvancedSetupTemplateParam++;
                if(buttonState==KEY_DOWN)pageAdvancedSetupTemplateParam--;
            }
            else
            {                                 // holding value
                holdingSelected->lock = true; // enable lock when change menu item, to prevent value changing during holdings update
                if (menuItemSelected->isBitfieldBit)
                {
                    // bit modifying
                    *holdingSelected->pntr ^= (1U << menuItemSelected->bitNumber);
                }
                else
                {
                    if (buttonState == KEY_UP)
                    {
                        *holdingSelected->pntr += pageAdvancedSetupTemplateDlt[menu_cur_pos];
                    }
                    else if (buttonState == KEY_DOWN)
                    {
                        *holdingSelected->pntr -= pageAdvancedSetupTemplateDlt[menu_cur_pos];
                    }
                }
            }
        }
        buttonState = KEY_NO;
        break;

    case KEY_AC1:
    case KEY_AC2:
    case KEY_DC1:
    case KEY_DC2: // AC1 //AC2 //DC1 //DC2
        pageAcAdvancedSetupUnlocked=false;
        if (!holdingSelected->change_req)
            holdingSelected->lock = false; // unlock not saved
        break;

    default:
        break;
    }
}

static inline void onError(void* pntr)
{
    memset(pntr, 0, 80);
    snprintf(pntr, 80,
        "NULL POINTER ERROR");
}

static inline void menu2dCheckLimit(int* cursorHPos, int* cursorVPos, int* firstLinePos, size_t menuSize, int vCnt, int hCnt)
{
    int vInd = vCnt - 1;
    int vhInd = hCnt - 1;

    if (vhInd <= 0) return;
    if (*cursorHPos < 0)
        *cursorHPos = vhInd;
    if (*cursorHPos > vhInd)
        *cursorHPos = 0;

    if (*cursorVPos < 0)
    {
        *cursorVPos = 0;
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

static inline void menu2DrawCursor(int* cursorHPos, int* cursorVPos, char* p)
{
    if (*cursorHPos == 0)
    {
        p[0 + 20 * *cursorVPos] = '[';
        p[19 + 20 * *cursorVPos] = ']';
    }

    if (*cursorHPos == 1)
    {
        p[73] = '[';
        p[79] = ']';
    }

    if (*cursorHPos == 2)
    {
        p[60] = '[';
        p[66] = ']';
    }
}

static TypeDef_ConfigMenuItem* pageMenuItemEditItem = &nullMenuItem;
static int pageConfigCursorVer = 0;
static int pageConfigCursorHor = 0;
static int pageConfigFirstLine = 0;
static void Page_Config(void* arg)
{
    char* pntr = (char*)arg;


    static TypeDef_ConfigMenuItem* pageConfigMenuItemSelected;

    for (size_t i = 0; i < configMenuSize; i++)
    {
        if (!configMenu[i].modified)
        {
            configMenu[i].temVl = *configMenu[i].val;
        }
    }

    menu2dCheckLimit(&pageConfigCursorHor, &pageConfigCursorVer, &pageConfigFirstLine, configMenuSize, 3, 3);

    memset(pntr, 0, 80);

    snprintf(pntr, 80,
        " %10s%s  %5d  %10s%s  %5d  %10s%s %6d  Apply         Exit  ",
        configMenu[pageConfigFirstLine + 0].label,
        configMenu[pageConfigFirstLine + 0].modified ? "*" : " ",
        configMenu[pageConfigFirstLine + 0].temVl,
        configMenu[pageConfigFirstLine + 1].label,
        configMenu[pageConfigFirstLine + 1].modified ? "*" : " ",
        configMenu[pageConfigFirstLine + 1].temVl,
        configMenu[pageConfigFirstLine + 2].label,
        configMenu[pageConfigFirstLine + 2].modified ? "*" : " ",
        configMenu[pageConfigFirstLine + 2].temVl);

    menu2DrawCursor(&pageConfigCursorHor, &pageConfigCursorVer, pntr);

    menu_cur_pos = 0;

    pageConfigMenuItemSelected = &configMenu[pageConfigFirstLine + pageConfigCursorVer];

    switch (buttonState)
    {
    case KEY_UP:
        pageConfigCursorVer--;
        break;

    case KEY_DOWN:
        pageConfigCursorVer++;
        break;
    case KEY_LEFT:
        pageConfigCursorHor++;
        break;
    case KEY_RIGHT:
        pageConfigCursorHor--;
        break;
    case KEY_ENTER:
        if (pageConfigCursorHor == 0)
        { // goto edit menu
            if (pageConfigMenuItemSelected->disabled) {
                pageConfigMenuItemSelected->itemChangedEvent();
                goto EXIT;
            }
            pageMenuItemEditItem = pageConfigMenuItemSelected;
            current_page = Page_MenuItemEdit;
        }
        else if (pageConfigCursorHor == 2)
        { // save
            for (size_t i = 0; i < configMenuSize; i++)
            {
                // change value
                if (configMenu[i].modified)
                {
                    configMenu[i].modified = false;
                    *configMenu[i].val = configMenu[i].temVl;
                    if (configMenu[i].itemChangedEvent != NULL) {
                        configMenu[i].itemChangedEvent();
                    }
                    MenuItemGeneralChangedEvent();
                    goto EXIT;
                }
            }
        }
        else if (pageConfigCursorHor == 1)
        { // exit

            for (size_t i = 0; i < configMenuSize; i++)
            {
                configMenu[i].modified = false;
                configMenu[i].temVl = *configMenu[i].val;
            }
            goto EXIT;
        }
        break;
    default:
        break;
    }
    return;
EXIT:
    current_page = Page_Logo;
    pageConfigCursorHor = 0;
    pageConfigCursorVer = 0;
    pageConfigFirstLine = 0;
}

static int pageMenuItemEditCursorVer = 0;
static int pageMenuItemEditFirstLine = 0;
static int pageMenuItemEditCursorHor = 0;

static void Page_MenuItemEdit(void* arg)
{
    char* pntr = (char*)arg;
    TypeDef_ConfigMenuItem* itemSelected = pageMenuItemEditItem;


    static const uint8_t pageMenuItemEditCursorPos[] = { 36, 35, 34, 33, 32, 0 }; // active display positions
    static const int pageMenuItemEditDelta[] = { 1, 10, 100, 1000, 10000, 0 };    // active display positions
    static uint8_t pageMenuItemEditCursorPosEdit = 0;

    menu2dCheckLimit(
        &pageMenuItemEditCursorHor,
        &pageMenuItemEditCursorVer,
        &pageMenuItemEditFirstLine,
        itemSelected->options_len,
        2, 2
    );


    pageMenuItemEditCursorPosEdit = menu_cur_pos;
    if (menu_cur_pos >= 5)
    {
        pageMenuItemEditCursorPosEdit = 5;
        pageMenuItemEditCursorHor = 1;
    }
    else
    {
        if (itemSelected->options_len == 0) pageMenuItemEditCursorHor = 0;
    }
    if (menu_cur_pos > 6)
        menu_cur_pos = 0;

    memset(pntr, 0, 80);
    snprintf(pntr, 20,
        "    *%10s *    ", itemSelected->label);

    snprintf(&pntr[60], 20,
        " Exit  ");

    if (itemSelected->options_len != 0)
    {
        snprintf(&pntr[20 + 20 * 0], 20, "             %6d ", (int)itemSelected->options[pageMenuItemEditFirstLine + 0]);
        snprintf(&pntr[20 + 20 * 1], 20, "             %6d ", (int)itemSelected->options[pageMenuItemEditFirstLine + 1]);
        itemSelected->temVl = itemSelected->options[pageMenuItemEditFirstLine + pageMenuItemEditCursorVer];
    }
    else
    {
        snprintf(&pntr[20], 20,
            "            %5d", itemSelected->temVl);

        if (pageMenuItemEditCursorHor == 0)
            flash_cursor(pntr, pageMenuItemEditCursorPos[pageMenuItemEditCursorPosEdit]);

        if (pageMenuItemEditItem->enableLim)
        {
            if (pageMenuItemEditItem->temVl > pageMenuItemEditItem->limHi)
            {
                pageMenuItemEditItem->temVl = pageMenuItemEditItem->limHi;
            }
            if (pageMenuItemEditItem->temVl < pageMenuItemEditItem->limLo)
            {
                pageMenuItemEditItem->temVl = pageMenuItemEditItem->limLo;
            }
        }
    }

    //menu2DrawCursor(&pageMenuItemEditCursorHor, &pageMenuItemEditCursorVer, pntr);

    if (pageMenuItemEditCursorHor == 0)
    {
        pntr[20 + 20 * pageMenuItemEditCursorVer] = '[';
        pntr[39 + 20 * pageMenuItemEditCursorVer] = ']';
    }

    if (pageMenuItemEditCursorHor == 1)
    {
        pntr[60] = '[';
        pntr[66] = ']';
    }

    switch (buttonState)
    {
    case KEY_UP:
        if (itemSelected->options_len == 0)
        {
            itemSelected->temVl += pageMenuItemEditDelta[pageMenuItemEditCursorPosEdit];
        }
        else
        {
            pageMenuItemEditCursorVer--;
        }
        break;

    case KEY_DOWN:
        if (itemSelected->options_len == 0)
        {
            itemSelected->temVl -= pageMenuItemEditDelta[pageMenuItemEditCursorPosEdit];
        }
        else
        {
            pageMenuItemEditCursorVer++;
        }
        break;
    case KEY_LEFT:
        if (itemSelected->options_len == 0)
        {
            menu_cur_pos++;
        }
        else
        {
            pageMenuItemEditCursorHor++;
        }
        break;
    case KEY_RIGHT:
        if (itemSelected->options_len == 0)
        {
            if (menu_cur_pos == 0)
            {
                menu_cur_pos = 6;
            }
            else
            {
                menu_cur_pos--;
            }
        }
        else
        {
            pageMenuItemEditCursorHor--;
        }
        break;
    case KEY_ENTER:
        if (pageMenuItemEditCursorHor == 0)
        {                                          // goto edit menu
            pageMenuItemEditItem->modified = true; // confirm
            goto EXIT;
        }
        else if (pageMenuItemEditCursorHor == 1)
        { // exit
            itemSelected->temVl = *itemSelected->val;
            itemSelected->modified = false;
            goto EXIT;
        }

        break;
    default:
        break;
    }

    return;

EXIT:
    current_page = Page_Config;
    pageMenuItemEditCursorHor = 0;
    pageMenuItemEditCursorVer = 0;
    pageMenuItemEditFirstLine = 0;
    menu_cur_pos = 0;
    return;
}

static void Page_SlaveFault(char* pntr, const char* label, int code)
{

    memset(pntr, 0, 80);
    snprintf(
        pntr,
        80,
        " %6s %s %3d",
       LABEL_AVARIA, label, code);
}

// https://radioaktiv.ru/custom_character_generator_for_hd44780.html