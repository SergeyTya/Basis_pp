#include "MotorControl_interface.h"
#include "dev_cnfg.h"
#include "fastmath.h"
#include "inverter.h"
#include "mcu_control.h"
#include "mem.h"
#include "realtime.h"
#include "stdint.h"
#include "task_list.h"
#include "builddep.h"

extern MotorControl_TypeDef motor;
extern TypeDef_Inverter inverter;
extern MotorControlInitStructure_TypeDef motorInit;
extern MotorControlEncoder_TypeDef encoder_4pole;  // TODO encoder init

uint32_t CCR[3];



void vTask_init(__attribute__((unused)) void *argument) {
  TypeDef_Inverter *inv = &inverter;

  inverter_StructureInit(inv);
  inverter_setInitState(inv);

  // Motor structure init

  motorInit.motor_type = MotorType_SCALAR;
  motorInit.TimerARRValue = PWM_MAX;

  motorInit.maximum_current = DEV_CONFIG_SHORTCURRENT_LIMIT / 3;
  motorInit.maximum_speed = 4500.f * 6.28f / 60.f;
  motorInit.maximum_power = 30000.f;
  motorInit.maximum_torque = 0.9f;
  motorInit.rated_busVoltage = DEV_CONFIG_BUS_RATED;
  motorInit.flux_ref = 0.05f;

  motorInit.Rs = 80.0f * 0.001f / 2.f;
  motorInit.Rr = 0.f;
  motorInit.L1 = 0.180f * 0.001f / 2.f;
  motorInit.L2 = 0.525f * 0.001f / 2.f;
  motorInit.L3 = 267.f * 1.41 / (4 * 4500 * 6.28 / 60);
  motorInit.pp = 4.f;

  motorInit.timeStep = 1.f / (float)RT_FREQ;
  motorInit.maximum_speed = 50.f * 2.f * M_PI;

  motorInit.Freq_pnt[0] = 0.5f; motorInit.Freq_pnt[1] = 0.5f; motorInit.Freq_pnt[2] = 0.5f; motorInit.Freq_pnt[3] = 0.5f;
  motorInit.Volt_pnt[0] = 0.5f; motorInit.Volt_pnt[1] = 0.5f; motorInit.Volt_pnt[2] = 0.5f; motorInit.Volt_pnt[3] = 0.5f;

  MotorInit(&motor, &motorInit);

  default_holdings();
  load_holdings();
  inverter.control_command = InverterCommands_NONE;

  MOTOR_IDLE(&motor);

  vBuildDepInitFunc();

  vTaskDelay(1000);

  // setup current sensors shifts
  inv->state_flags.flags._CurSens_calib = true;
  TypeDef_InverterSensor *sensors[3] = {&inv->sensors[DEV_CONFIG_ADC_CH_I1],
                                        &inv->sensors[DEV_CONFIG_ADC_CH_I2],
                                        &inv->sensors[DEV_CONFIG_ADC_CH_I3]};
  inv->state_flags.flags._CurSens_calib = current_sensors_init(sensors);

  // startup tasks
  xTaskCreate(vTask_1s, "1s", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 3, NULL);
  xTaskCreate(vTask_1ms, "1ms", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 4, NULL);
  xTaskCreate(vTask_10ms, "1ms", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 3, NULL);


  

  vTaskDelay(1000);
  inverter_resetFaultState(inv);
  
  // starting interfaces
  xTaskCreate(vTask_modbus, "Modbus", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 2, NULL);
  xTaskCreate(vTask_Scope, "Scope ", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(vTask_J1939, "j1939", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 3, NULL);
  xTaskCreate(vTask_CAN,   "can", configMINIMAL_STACK_SIZE, NULL,
               tskIDLE_PRIORITY + 3, NULL);

  // inverter_setReadyState(inv);
  // inverter_resetFaultState(inv);
  // vTaskDelay(200);
  // inverter.state_flags.flags._chk_disable = true;
  // vTaskDelay(200);
  // inverter_resetFaultState(inv);
  // inverter_setRunState(inv);
 
  vTaskDelete(NULL);
}

bool current_sensors_init(TypeDef_InverterSensor *currentSensors[3]) {
  int siTask1ms_curSensCnt = 0;
  float fltr[3] = {0.f};

  while (1) {
    for (size_t i = 0; i < 3; i++) {
      float in = (float)(*currentSensors[i]->RAW_value);
      CONTROL_FASTLPF(in, fltr[i], 0.001f * 100.f);
      int16_t out = (int16_t)fltr[i];
      currentSensors[i]->RAW_offset = out;
    }

    if (siTask1ms_curSensCnt > 1000) {
      bool done = ((currentSensors[0]->RAW_unival) < 3) &&
                  ((currentSensors[1]->RAW_unival) < 3) &&
                  ((currentSensors[2]->RAW_unival) < 3);

      if (done == true) {
        siTask1ms_curSensCnt = 0;
        return false;
      }
    }

    siTask1ms_curSensCnt++;
    if (siTask1ms_curSensCnt > 5000)
    {
      HW_REBOOT;
    }
    /* code */
    vTaskDelay(1);
  }
}