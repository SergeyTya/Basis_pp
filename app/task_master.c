#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "mbsupport.h"
#include "task_master.h"

#include "panelConfig.h"
#include "masterTransport.h"

#include "timers.h"

#include "meter.h"
#include "clock.h"

static const uint8_t  MASTER_GLOB_TRANSPORT_TO = 30;


TypeDef_Master master;

extern Typedef_PanelConfig panelConfig;
extern Typedef_Meter meter;
extern Typedef_Clock clock;

extern void master_LEDonFaultReset();
extern void master_LEDonFaultState();
extern void master_LEDonRUNstate(int dev);
extern void master_LEDonReadyState(int dev);
extern void master_LEDonTimeoutState();
extern void master_LEDonGlobRunState(bool state);

extern void master_RDIOonRUNstate(int dev);

// Used for encode slave fault bits
static inline int checkCode(TypeDef_MB_Holding *holding, uint16_t slaveAdr, int mask, int16_t offset);

static inline void MASTER_TRANSPORT_CHECK_TIMEOUT(TypedefEnum_MasterTransportSates state, int slaveAdr)
{
    if ((state) != MASTER_TRANSPORT_NOERROR)
    {
        master.slave[slaveAdr].slaveStates = MASTER_STATE_onTIMEOUT;
        master.slave[slaveAdr].master_wdg = true;
    }
    else
    {
        master.slave[slaveAdr].master_wdg = false;
    }

    vTaskDelay(6);
}

void vTask_MasterHWstates(void *argument);
void vTask_MasterAC(void *argument);
void vTask_MasterDC(void *argument);
void vTask_MasterDev(void *argument);

static void read_all_from_slave(uint16_t slaveAdr);
void handle_AC(uint16_t slaveAdr);
void handle_DC(uint16_t slaveAdr);

void vTask_Master(__attribute__((unused)) void *argument)
{
    // Init master hardware for modbus RTU

    master_hwInit(panelConfig.modbus_master.speed);
    master_transport_init();

    vTaskDelay(300);

    xTaskCreate(vTask_MasterHWstates, "MasterStates", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
  
    for (size_t i = 1; i < 5; i++)
    {
        master.slave[i].slaveStates = MASTER_STATE_onTIMEOUT;
    }

    uint8_t meter_cnt = 0;

    while (1)
    { // This task only read slave parameters an write it if needed



        bool slave_enable[5] = {false,
                                    panelConfig.enableAC1,
                                    panelConfig.enableAC2,
                                    panelConfig.enableDC1,
                                    panelConfig.enableDC2};


        for (size_t j = 1; j < 5; j++) // Read all slaves from j (slave addr) = 1 to 4
        {
           // Current slave address
            uint16_t slaveAdr = j;

            // Current slave holding register table pointer

            if (slave_enable[slaveAdr] != true)
            { // skip if slave not enabled
                master.slave[slaveAdr].slaveStates = MASTER_STATE_EMPTY;
            }
            else
            {
                /* Check slave displayed */
                if (slaveAdr == panelConfig.active_slave)
                { // skip not active slave
                    read_all_from_slave(slaveAdr);
                }
                /* Handle AC*/
                handle_AC(slaveAdr);
                /* Handle DC*/
                handle_DC(slaveAdr);
            }
 
        }



         // Read Meter
        if (meter.enable)
        {
            if(meter_cnt++ > 2){
             meter.read(&meter);
                meter_cnt = 0;
            }
        }

      
            // Read Clock
        if (clock.enable)
        {
            clock.read(&clock);
            vTaskDelay(10); // NEED Delay
                // if(clock.state == MASTER_TRANSPORT_TIMEOUT)  vTaskDelay(2000);
        }

        if(
            // no slave selected
            (panelConfig.enableAC1||panelConfig.enableAC2||panelConfig.enableDC1||panelConfig.enableDC2) == 0
        )
        {
            vTaskDelay(100);
        }
    }
}

static void read_all_from_slave(uint16_t slaveAdr)
{
    /* Read displayed slave holding registers */

    TypeDef_MB_Table *table = &holdings_table[slaveAdr];
    TypeDef_MB_Holding *holding;

    int tocntr = 0;
    for (size_t i = 0; i < table->len; i++)
    {
        holding = &table->holdings[i];

        // Write HR to slave
        if (holding->lock && holding->change_req)
        {

            /* Send holding value to slave */
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, *holding->pntr, MASTER_GLOB_TRANSPORT_TO),
                slaveAdr);
            holding->lock = false;
            holding->change_req = false;
         //   vTaskDelay(5); // Delay after write

            // save to AC slave memory
            if (slaveAdr == CONFIG_SLAVE_AC1 || slaveAdr == CONFIG_SLAVE_AC2)
            {
                uint16_t reg900CurVal = 0;
                // read save control HR adr=900
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHoldingOs(slaveAdr, 900, &reg900CurVal, MASTER_GLOB_TRANSPORT_TO),
                    slaveAdr);
                // modify bit 1
                reg900CurVal |= (1 << 0);
                // send back
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_writeHoldingOs(slaveAdr, 900, reg900CurVal, MASTER_GLOB_TRANSPORT_TO * 2),
                    slaveAdr);
            }

            // save to DC slave memory
            if (slaveAdr == CONFIG_SLAVE_DC1 || slaveAdr == CONFIG_SLAVE_DC2)
            {
                uint16_t reg170CurVal = 0;
                // read save control HR adr=170
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHoldingOs(slaveAdr, 170, &reg170CurVal, MASTER_GLOB_TRANSPORT_TO),
                    slaveAdr);
                // modify bit 1
                reg170CurVal |= (1 << 0);
                // send back
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_writeHoldingOs(slaveAdr, 170, reg170CurVal, MASTER_GLOB_TRANSPORT_TO * 2),
                    slaveAdr);
            }
        }

        // read holding value to slave
        uint16_t out = 0;
        TypedefEnum_MasterTransportSates res = master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, &out, MASTER_GLOB_TRANSPORT_TO);
        if (res == MASTER_TRANSPORT_NOERROR)
        {
            if (holding->lock == false)
            {
                *holding->pntr = out;
            }
            // feed master wdg
            master.slave[slaveAdr].master_wdg = false;
        }
        else
        {
            master.slave[slaveAdr].master_wdg = true;
            tocntr++;
            master.slave[slaveAdr].slaveStates = MASTER_STATE_onTIMEOUT;
            if (tocntr > 3)
                break;
        }
    }
}

static inline int checkCode(TypeDef_MB_Holding *holding, uint16_t slaveAdr, int mask, int16_t offset)
{
    uint16_t faultCode = (*holding->pntr) & 0x0FFF;
    if (faultCode > 0)
    {
        for (int i = 0; i < mask; i++)
        { // fault code is bit number
            uint8_t bit = (faultCode & (1 << i)) >> i;
            if (bit == 1)
            {
                master.slave[slaveAdr].slaveStates = MASTER_STATE_onFAULT;
                master.slave[slaveAdr].fault_source = true;
                master.slave[slaveAdr].fault_code = i + 1 + offset;
                return master.slave[slaveAdr].fault_code;
            }
        }
    }
    return 0;
}

bool blinker = false;
static volatile uint16_t blinker_cntr = 0;
void vTask_MasterHWstates(void *argument)
{

    bool onGlobalFault = false;
    bool onGlobalTimeout = false;
    bool onGlobalRUN = false;
    while (1)
    {

        if (blinker_cntr == 0)
        {
            blinker_cntr = 3;
            blinker = !blinker;
        }
        else
        {
            blinker_cntr--;
            // return;
        }

        vTaskDelay(100);

        if (!onGlobalFault && !onGlobalTimeout)
        {
            master_LEDonFaultReset();
        }
        else
        {
            if (onGlobalTimeout)
            {
                master_LEDonTimeoutState();
            }
            else
            {
                master_LEDonFaultState();
            }
        }

        master_LEDonGlobRunState(onGlobalRUN);

        onGlobalFault = false;
        onGlobalTimeout = false;
        onGlobalRUN = false;

        for (size_t i = 1; i < 5; i++)
        {
            onGlobalFault |= master.slave[i].slaveStates == MASTER_STATE_onFAULT;
            onGlobalTimeout |= master.slave[i].slaveStates == MASTER_STATE_onTIMEOUT;
            onGlobalRUN |= master.slave[i].slaveStates == MASTER_STATE_onRUN;

            switch (master.slave[i].slaveStates)
            {
            case MASTER_STATE_onREADY:
            case MASTER_STATE_onFAULT:
                master_LEDonReadyState(i);
                break;
            case MASTER_STATE_onRUN:
                master_LEDonRUNstate(i);
                break;
            case MASTER_STATE_onWAIT_FOR_AC_OK:
                master_LEDonWaitForAcOkState(i);
                break;
            case MASTER_STATE_onWAIT_FOR_MAIN_RELAY:
                master_LEDonWaitForMainRelayState(i);
                break;
            default:
                break;
            }
        }

        if (clock.enable)
            onGlobalTimeout |= clock.state == MASTER_TRANSPORT_TIMEOUT;
        if (meter.enable)
            onGlobalTimeout |= meter.state == MASTER_TRANSPORT_TIMEOUT;
    }
}

void master_stop_dc(uint8_t slaveAdr)
{

    uint16_t valStop = 0;
    master_readHoldingOs(slaveAdr, 104, &valStop, MASTER_GLOB_TRANSPORT_TO);

    valStop = valStop & ~(1U << 1);
    MASTER_TRANSPORT_CHECK_TIMEOUT(
        master_writeHoldingOs(slaveAdr, 104, valStop, MASTER_GLOB_TRANSPORT_TO),
        slaveAdr);
}

void handle_AC(uint16_t slaveAdr)
{

    if (
           (slaveAdr != CONFIG_SLAVE_AC1) 
        && (slaveAdr != CONFIG_SLAVE_AC2)
    ) return;

    // Current slave holding register table pointer
    TypeDef_MB_Table *table = &holdings_table[slaveAdr];
    TypeDef_MB_Holding *holding;

    // Reset start req if TO
    if (master.slave[slaveAdr].slaveStates == MASTER_STATE_onTIMEOUT)
    {

        master.slave[slaveAdr].start_req_hw = false;
        // read AC slaves status only
        holding = GetHoldingByAdrFromTable(270, table);
        MASTER_TRANSPORT_CHECK_TIMEOUT(
            master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO),
            slaveAdr);
        if (!master.slave[slaveAdr].master_wdg)
            master.slave[slaveAdr].slaveStates = MASTER_STATE_onREADY;

        return;
    }

    {
        {
            // Read AC indicators
            //  Currents 243, 244, 245
            holding = GetHoldingByAdrFromTable(243, table);
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
            holding = GetHoldingByAdrFromTable(244, table);
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
            holding = GetHoldingByAdrFromTable(245, table);
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
            // Voltage  240, 241, 242
            holding = GetHoldingByAdrFromTable(240, table);
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
            holding = GetHoldingByAdrFromTable(241, table);
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
            holding = GetHoldingByAdrFromTable(242, table);
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
        }

        // read AC slaves status
        holding = GetHoldingByAdrFromTable(270, table);
        MASTER_TRANSPORT_CHECK_TIMEOUT(
            master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
        // check faults
        // check faults
        int fltcode = 0;
        fltcode = checkCode(holding, slaveAdr, 12, 0);
        holding = GetHoldingByAdrFromTable(271, table);
        MASTER_TRANSPORT_CHECK_TIMEOUT(
            master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
        fltcode += checkCode(holding, slaveAdr, 11, 16);

        // Fault Reset
        if (fltcode == 0)
        {
            if (master.slave[slaveAdr].slaveStates == MASTER_STATE_onFAULT)
            {
                master.slave[slaveAdr].slaveStates = MASTER_STATE_onREADY;
                master.slave[slaveAdr].fault_source = false;
                master.slave[slaveAdr].fault_code = 0;
            }
        }

        if (master.slave[slaveAdr].slaveStates != MASTER_STATE_onFAULT)
        {
            // Read control register
            holding = GetHoldingByAdrFromTable(103, table);
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);

            uint16_t ac_slave_CR = *holding->pntr;

            if (!master.slave[slaveAdr].master_wdg)
            { // Skip if onTimeout state
                /*
                                READY - RDO off, inv - off --> start_hw
                   stop hw <--- WAIT  - RDO off, inv - on  --> start_rdo
                  stop rdo <--- RUN   - RDO on , inv - on
                */

                if ((ac_slave_CR & 0x8) == 0)
                { // Ready state
                    master.slave[slaveAdr].slaveStates = MASTER_STATE_onREADY;
                    // master.slaveStates[2] = MASTER_STATE_onREADY;

                    master.slave[slaveAdr].start_req_rdo = false; // always reset rdo start req in ready state
                    if (master.slave[slaveAdr].start_req_hw)
                    {
                        master.slave[slaveAdr].fault_source_pm = false;
                        // send start
                        holding = GetHoldingByAdrFromTable(103, table);
                        uint16_t valACStart = *holding->pntr | 8U; // bit #3
                        MASTER_TRANSPORT_CHECK_TIMEOUT(
                            master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, valACStart, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
                        master.slave[slaveAdr].start_req_hw = false;
                        master.slave[slaveAdr].slaveStates = MASTER_STATE_onWAIT_FOR_AC_OK;
                    }
                }
                else
                {
                    // Here we check if output voltage reach reference

                    if (master.slave[slaveAdr].start_req_rdo)
                    {
                        // Short press
                        if (master.slave[slaveAdr].slaveStates == MASTER_STATE_onWAIT_FOR_AC_OK)
                        {

                            uint16_t Uacr[3];
                            uint16_t ref_ra;

                            for (int i = 0; i < 3; i++) {
                                master_readHoldingOs(slaveAdr, 240 + i, &Uacr[i], MASTER_GLOB_TRANSPORT_TO);
                            }

                            master_readHoldingOs(slaveAdr, 102, &ref_ra, MASTER_GLOB_TRANSPORT_TO);

                            ref_ra = ref_ra*9/10;

                            bool reached = (Uacr[0] >= ref_ra) && (Uacr[1] >= ref_ra) && (Uacr[2] >= ref_ra);

                            // if output voltage is good
                            if (reached)
                            {
                                // Enable contactor RDO
                                master.slave[slaveAdr].slaveStates = MASTER_STATE_onWAIT_FOR_MAIN_RELAY;
                            }
                        }

                        if (master.slave[slaveAdr].slaveStates == MASTER_STATE_onRUN)
                        {
                            // Обратный порядок выключения  краткое нажатие - контактор, длинное инвертор
                            // Switch off the contactor
                            master.slave[slaveAdr].slaveStates = MASTER_STATE_onWAIT_FOR_AC_OK;
                        }

                        master.slave[slaveAdr].start_req_rdo = false;
                        master.slave[slaveAdr].start_req_hw = false;
                        vTaskDelay(300);
                    }

                    // check main contactor feedback
                    if (master.slave[slaveAdr].slaveStates == MASTER_STATE_onWAIT_FOR_MAIN_RELAY)
                    {
                        // read HR 220
                       // static volatile uint16_t acreg220 = 0;

                        uint16_t acreg220 = 0;
                        MASTER_TRANSPORT_CHECK_TIMEOUT(
                             master_readHoldingOs(slaveAdr, 220, &acreg220, MASTER_GLOB_TRANSPORT_TO)
                             ,slaveAdr
                        );

                        uint8_t  BIT_MAIN_RELAY_FEEDBACK = (1U << 1);
                        if ((acreg220 & BIT_MAIN_RELAY_FEEDBACK) != 0)
                        {
                            master.slave[slaveAdr].slaveStates = MASTER_STATE_onRUN;
                        }
                    }

                    if ( // Timeout state
                        (master.slave[slaveAdr].slaveStates == MASTER_STATE_onTIMEOUT) || (master.slave[slaveAdr].slaveStates == MASTER_STATE_onREADY))
                    {
                        uint16_t acreg220_2 = 0;
                        // Return after timeout
                        MASTER_TRANSPORT_CHECK_TIMEOUT(
                            master_readHoldingOs(slaveAdr, 220, &acreg220_2, MASTER_GLOB_TRANSPORT_TO), slaveAdr);

                        if ((acreg220_2 & 0x2) != 0)
                        {
                            master.slave[slaveAdr].slaveStates = MASTER_STATE_onRUN;
                        }
                        else
                        {
                            master.slave[slaveAdr].slaveStates = MASTER_STATE_onWAIT_FOR_AC_OK;
                        }
                    }

                    if (master.slave[slaveAdr].start_req_hw)
                    {
                        // Long press
                        // Обратный порядок выключения  краткое нажатие - контактор, длинное инвертор
                        if (
                            master.slave[slaveAdr].slaveStates != MASTER_STATE_onRUN)
                        {
                            // stop inverter (long press) only after contactor switched off
                            master.slave[slaveAdr].start_req_hw = false;
                            master.slave[slaveAdr].start_req_rdo = false;
                            //uint16_t valStopACx = *holding->pntr ^ 8;
                            uint16_t valStopACx = *holding->pntr & ~8U;
                            MASTER_TRANSPORT_CHECK_TIMEOUT(
                                master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, valStopACx, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
                        }
                    }
                }
            } // AC slave onTimeout state
        }
        else
        {
            // AC slave onFAULT
            master.slave[slaveAdr].start_req_hw = false;
            master.slave[slaveAdr].start_req_rdo = false;
        }
    }
}

void handle_DC(uint16_t slaveAdr)
{
    if ((slaveAdr != CONFIG_SLAVE_DC1) && (slaveAdr != CONFIG_SLAVE_DC2)) return;

    // Current slave holding register table pointer
    TypeDef_MB_Table *table = &holdings_table[slaveAdr];
    TypeDef_MB_Holding *holding;

    // Reset start req if TO
    if (master.slave[slaveAdr].slaveStates == MASTER_STATE_onTIMEOUT)
    {
        master.slave[slaveAdr].start_req_hw = false;

        // read status only
        holding = GetHoldingByAdrFromTable(220, table);
        MASTER_TRANSPORT_CHECK_TIMEOUT(
            master_readHoldingOs(slaveAdr, 220, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
        if (!master.slave[slaveAdr].master_wdg)
            master.slave[slaveAdr].slaveStates = MASTER_STATE_onREADY;

        return;
    }

    {
        // Read DC indicators Udc-211, Idc-210
        {
            holding = GetHoldingByAdrFromTable(210, table);
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_readHoldingOs(slaveAdr, 210, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
            holding = GetHoldingByAdrFromTable(211, table);
            MASTER_TRANSPORT_CHECK_TIMEOUT(
                master_readHoldingOs(slaveAdr, 211, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
        }

        // Read status register
        holding = GetHoldingByAdrFromTable(220, table);
        MASTER_TRANSPORT_CHECK_TIMEOUT(
            master_readHoldingOs(slaveAdr, 220, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);

        {

            if ((*holding->pntr & 0x4) != 0)
            {
                // DC onFault state
                master.slave[slaveAdr].slaveStates = MASTER_STATE_onFAULT;
                // Read Fault code register
                holding = GetHoldingByAdrFromTable(240, table);
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
                checkCode(holding, slaveAdr, 8, -1); // Set slave onFAULT state here!
                // stops active start requests
                master.slave[slaveAdr].start_req_hw = false;
            }
            else if ((*holding->pntr & 0x1) != 0)
            { // DC RUN STATE (in RUN state bow bit RUN and RDY are 1)

                if (master.slave[slaveAdr].slaveStates == MASTER_STATE_onWAIT_FOR_AC_OK)
                {
                    // wait for output good

                    uint16_t Uo = 0;
                    uint16_t Ur = 0;

                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_readHoldingOs(slaveAdr, 211, &Uo, MASTER_GLOB_TRANSPORT_TO), slaveAdr);

                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_readHoldingOs(slaveAdr, 102, &Ur, MASTER_GLOB_TRANSPORT_TO), slaveAdr);

                    if (Uo > Ur * 8 / 10)
                    {
                        master.slave[slaveAdr].slaveStates = MASTER_STATE_onRUN;
                    }
                }

                // Timeout state
                if (
                    (master.slave[slaveAdr].slaveStates == MASTER_STATE_onTIMEOUT) || (master.slave[slaveAdr].slaveStates == MASTER_STATE_onREADY))
                {
                    master.slave[slaveAdr].slaveStates = MASTER_STATE_onWAIT_FOR_AC_OK;
                }

                //   master.slave[slaveAdr].slaveStates = MASTER_STATE_onRUN;
                // DC onRun state

                // SLAVE START STOP
                if (master.slave[slaveAdr].start_req_hw == true)
                {
                    // send stop;
                    uint16_t valStop = 0;

                    master_readHoldingOs(slaveAdr, 104, &valStop, MASTER_GLOB_TRANSPORT_TO);

                    valStop = valStop & ~(1U << 1);
                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_writeHoldingOs(slaveAdr, 104, valStop, MASTER_GLOB_TRANSPORT_TO), slaveAdr);
                    master.slave[slaveAdr].start_req_hw = false;

                    vTaskDelay(10);
                }

                // DC correction 
                if(master.slave[slaveAdr].DcUCor_enable != 0){
                    uint16_t Inom = master.slave[slaveAdr].DcUCor_In;
                    uint16_t Iout = *GetHoldingByAdrFromTable(210, table)->pntr;
                    uint16_t Ucor = 0;
                    if(Inom != 0){
                        Ucor=master.slave[slaveAdr].DcUCor_Uk * Iout/Inom;
                        if(Ucor > master.slave[slaveAdr].DcUCor_Uk) Ucor = master.slave[slaveAdr].DcUCor_Uk;
                    }
                    uint16_t uref = master.slave[slaveAdr].DcUCor_Uxx + Ucor;
                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_writeHoldingOs(slaveAdr, 102, uref, MASTER_GLOB_TRANSPORT_TO), 
                        slaveAdr
                    );
                }

            } /* Handle DC start|stop req */
            else if ((*holding->pntr & 0x2) != 0)
            { // DC READY STATE

                master.slave[slaveAdr].fault_source = false;
                // DC onReady state
                master.slave[slaveAdr].slaveStates = MASTER_STATE_onREADY;
                // SLAVE START REQUEST
                if (master.slave[slaveAdr].start_req_hw == true)
                {
                    /**
                     Нажимаем в течение 3-4 секунд кнопку DC,
                     светодиод DC начинает моргать и продолжает так делать,
                     пока выходное напряжение не будет равно заданному.
                     После этого постоянно светит и также в этот момент загорается светодиод ВКЛ.
                 */

                    // Reset panel monitor fault
                    master.slave[slaveAdr].fault_source_pm = false;

                    uint16_t valStart = 0;
                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_readHoldingOs(slaveAdr, 104, &valStart, MASTER_GLOB_TRANSPORT_TO), slaveAdr);

                    valStart = valStart | 0x2;
                    master_writeHoldingOs(slaveAdr, 104, valStart, MASTER_GLOB_TRANSPORT_TO);
                    master.slave[slaveAdr].slaveStates = MASTER_STATE_onWAIT_FOR_AC_OK;
                    master.slave[slaveAdr].start_req_hw = false;
                    vTaskDelay(100);
                }
            }
        }
    }
}
