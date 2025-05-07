
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "task_scope.h"

xQueueHandle xQueue;
TypeDef_xSCOPE xSCOPE;


void vTask_Scope(__attribute__((unused)) void *argument) {

	uint16_t frm_cnt = 0;
	uint16_t scp_cnt = 0;
	uint8_t chnum = 1;
	uint8_t delay = 1;

	//xSCOPE.ctrl = (count&0x3)+(freq<<2);
	while (1) {

		chnum = (xSCOPE.CNTRL.chn_num);  
		if(chnum>SCOPE_CHANNEL_MAX) chnum = SCOPE_CHANNEL_MAX;
		delay = (xSCOPE.CNTRL.delay   & 0xF) + 1;

		for (uint8_t i = 0; i < chnum;) {
			
			uint16_t temp_scp = *xSCOPE.adr[i++];
			xSCOPE.Send_buf[frm_cnt + scp_cnt] = ((uint8_t *) &temp_scp)[1];
			scp_cnt++;
			xSCOPE.Send_buf[frm_cnt + scp_cnt] = ((uint8_t *) &temp_scp)[0];
			scp_cnt++;
		};

		if (scp_cnt >= (SCOPE_DATAFRAME_SIZE)) {
			xSCOPE.Send_buf[frm_cnt + scp_cnt] = delay;
			scp_cnt++;
			xSCOPE.Send_buf[frm_cnt + scp_cnt] = chnum;
			scp_cnt++;
			xSCOPE.Send_buf[frm_cnt + scp_cnt] = (uint8_t) (uxQueueMessagesWaiting(xQueue));
			scp_cnt = 0;
			uint32_t temp_pntr = (uint32_t) &(xSCOPE.Send_buf[frm_cnt]);
			frm_cnt += (SCOPE_FRAME_SIZE);
			if (frm_cnt >= (6 * (SCOPE_FRAME_SIZE))) frm_cnt = 0;
			xQueueSendToBack(xQueue, &temp_pntr, ( TickType_t ) portMAX_DELAY);
		};
		
		vTaskDelay(delay);

	}
}