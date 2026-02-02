#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include  "task_list.h"
#include  "mcu_init.h"
#include  "dev_cnfg.h"
#include  "main.h"

#include "task_panel.h"
#include "task_modbus.h"

#include "mb.h"
#include "mbport.h"

#include "task_master.h"
#include "task_keybord.h"

#include "gd32f4xx_enet_init.h"
#include "netconf.h"

#include "task_scope.h"

#include "http_server.h"

#include "IDisplay.h"

#include "task_tempmes.h"

#include "task_logger.h"



uint16_t holdings[256];


void vTask_ethercat(__attribute__((unused)) void* argument);
void TaskLive();
void vTask_ethernet_start();

int main() {

  vMCU_init();
  StructureInit_PanelConfig(&panelConfig);
  ConfigMenuReadAll();

  uint8_t hwr = hw_get_revision();
  if (hwr == 0b001) {
    DisplayMapMutaba();
  }
  if (hwr == 0b010) {
    DisplayMapWinstar();
  }

  if (panelConfig.modbus_RTU.enable == 1) {
    xTaskCreate(vTask_modbusRTU, "ModbusSlaveRTU", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL);
  }

  xTaskCreate(vTask_ethernet_start, "Estart", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
  xTaskCreate(        vTask_Master, "Master", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
  xTaskCreate(         vTask_Panel,  "Panel", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, NULL);
  hw_hbl_set(2, 0);
  xTaskCreate(vTask_keyboard, "Keyboard", 100U, NULL, tskIDLE_PRIORITY + 6, NULL);

  if (panelConfig.ethercat.enable == 1) {
    xTaskCreate(vTask_ethercat, "EtherCAT", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 8, NULL);
  }

  hw_hbl_set(3, 0);
  xTaskCreate(TaskLive, "Hartbeat", 100U, NULL, tskIDLE_PRIORITY + 8, NULL);
  xTaskCreate(vTask_TemperatureControl, "Tempmes", 100U, NULL, tskIDLE_PRIORITY + 1, NULL);

  xTaskCreate(vTask_logger_writer, "logger_writer", 100U, NULL, tskIDLE_PRIORITY + 1, NULL);
  
  vTaskStartScheduler();
  return 0;
}

void TaskLive() {
  while (1) {
    hwDriveHartBit_led1();
    vTaskDelay(1000);
  }
}

extern volatile uint32_t enet_init_status;
void vTask_ethernet_start() {
  uint8_t ipadr[4] = { panelConfig.modbus_TCP.ip0, panelConfig.modbus_TCP.ip1,panelConfig.modbus_TCP.ip2,panelConfig.modbus_TCP.ip3 };
  uint8_t ipmas[4] = { panelConfig.modbus_TCP.mask0, panelConfig.modbus_TCP.mask1,panelConfig.modbus_TCP.mask2,panelConfig.modbus_TCP.mask3 };

  // ipadr[0] = BOARD_IP_ADDR0; ipadr[1] = BOARD_IP_ADDR1; ipadr[2] = BOARD_IP_ADDR2; ipadr[3] = BOARD_IP_ADDR3;
  // ipmas[0] = BOARD_NETMASK_ADDR0 ; ipmas[1] =BOARD_NETMASK_ADDR1 ;ipmas[2] = BOARD_NETMASK_ADDR2; ipmas[3] =BOARD_NETMASK_ADDR3;

  while (enet_init_status == 0) {
    enet_system_setup();
    vTaskDelay(1000);
  }

  hw_hbl_set(1, 0);

  lwip_stack_init((uint8_t*)ipadr, (uint8_t*)ipmas);
  http_server_init();

  if (panelConfig.modbus_RTU.enable == false) {
    xTaskCreate(vTask_modbusTCP, "ModbusSlaveTCP", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL);
  }

  vTaskDelete(NULL);
}

