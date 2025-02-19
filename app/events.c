#include "events.h"

#include "MotorControl_interface.h"
#include "inverter.h"
#include "mem.h"

#ifdef UUBOOT
#include "uuboot.h"
#endif

extern MotorControl_TypeDef motor;
extern TypeDef_Inverter inverter;

void events_dummy(void *arg)
{
}

volatile uint16_t res_e = 0;
void events_commandPoll(void *arg)
{

    TypeDef_Inverter *inv_now = &inverter;
    TypeDef_InverterSensor *sensors[3] = {
                &inv_now->sensors[DEV_CONFIG_ADC_CH_I1],
                &inv_now->sensors[DEV_CONFIG_ADC_CH_I2],
                &inv_now->sensors[DEV_CONFIG_ADC_CH_I3]
    };


    switch (inv_now->control_command)
    {
    case InverterCommands_START:
        if (INVERTER_isREADY(inv_now))
        {

            inverter_setRunState(inv_now);
        }
        break;
    case InverterCommands_STOP:
        if (INVERTER_isRUN(inv_now))
        {
            inverter_setReadyState(inv_now);
        }
        break;
    case InverterCommands_RESET:
            if(INVERTER_isFAULT(inv_now)){
                inv_now->state_flags.flags._CurSens_calib = current_sensors_init(sensors);     
            }
            inverter_resetFaultState(inv_now);
        break;
    case InverterCommands_SAVE:
        if (INVERTER_isRUN(inv_now) == false)
        {
            inv_now->control_command = InverterCommands_NONE;
            bool res = save_holdings();
            if (res == false)
            {
                inverter_setFault_Flash(inv_now);
            }
        }
        break;
    case InverterCommands_LOAD:
        load_holdings();
        break;
    case InverterCommands_DEFAULT:
        default_holdings();
        break;
    case InverterCommands_CHK_DSBL:
        inverter_setCheckDisableState(inv_now, ( !INVERTER_isCHEKCDSBL(inv_now) ));
        break;
    case InverterCommands_REBOOT:
        if (INVERTER_isRUN(inv_now) == false)
        {
            HW_REBOOT;
        }
        break;
    case InverterCommands_LOADER485:
        if (INVERTER_isRUN(inv_now) == false)
        {
#ifdef UUBOOT
            vBootloader_run();
#endif
        }
        break;

    default:
        break;
    }
    inv_now->control_command = InverterCommands_NONE;
}