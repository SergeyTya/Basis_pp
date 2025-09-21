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



uint16_t holdings[256];


void vTask_ethercat(__attribute__((unused)) void *argument);
void TaskLive();

int main() {

      vMCU_init();
      StructureInit_PanelConfig(&panelConfig);
      ConfigMenuReadAll();

      // if(hw_get_revision() == 0){
      //    DisplayMapMutaba();
      // }
      // if(hw_get_revision() == 1){
      //     DisplayMapWinstar();
      // }
    DisplayMapWinstar();

    hw_hbl_set(0,0);
   
    enet_system_setup();
    


    uint8_t ipadr[4] =  {panelConfig.modbus_TCP.ip0, panelConfig.modbus_TCP.ip1,panelConfig.modbus_TCP.ip2,panelConfig.modbus_TCP.ip3} ; 
    uint8_t ipmas[4] =  {panelConfig.modbus_TCP.mask0, panelConfig.modbus_TCP.mask1,panelConfig.modbus_TCP.mask2,panelConfig.modbus_TCP.mask3} ; 

    // ipadr[0] = BOARD_IP_ADDR0; ipadr[1] = BOARD_IP_ADDR1; ipadr[2] = BOARD_IP_ADDR2; ipadr[3] = BOARD_IP_ADDR3;
    // ipmas[0] = BOARD_NETMASK_ADDR0 ; ipmas[1] =BOARD_NETMASK_ADDR1 ;ipmas[2] = BOARD_NETMASK_ADDR2; ipmas[3] =BOARD_NETMASK_ADDR3;

    lwip_stack_init((uint8_t *) ipadr, (uint8_t *) ipmas);

    http_server_init();

    panelConfig.modbus_RTU.enable = false;
    if(panelConfig.modbus_RTU.enable){
        xTaskCreate(vTask_modbusRTU    , "ModbusSlaveRTU"   , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL);
    }else{
        xTaskCreate(vTask_modbusTCP    , "ModbusSlaveTCP"   , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL);
    } 
    hw_hbl_set(1,0);
  


    xTaskCreate(vTask_Master    , "Master"        , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(vTask_Panel     , "Panel"         , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, NULL);
    hw_hbl_set(2,0);
    xTaskCreate(vTask_keyboard  , "Keyboard"      , 100U, NULL, tskIDLE_PRIORITY + 6, NULL);

    if(panelConfig.ethercat.enable ==1 ){
      xTaskCreate(vTask_ethercat, "EtherCAT", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 8, NULL);
    }
      
    hw_hbl_set(3,0); 
    xTaskCreate(TaskLive,                "Hartbeat", 100U, NULL, tskIDLE_PRIORITY + 8, NULL); 
    xTaskCreate(vTask_TemperatureControl,"Tempmes" , 100U, NULL, tskIDLE_PRIORITY + 1, NULL);
    

    vTaskStartScheduler();
    return 0;
}

void TaskLive(){
    while(1){
        hwDriveHartBit_led1();
        vTaskDelay(1000);
    }
}

