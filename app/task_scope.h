/*
 * McuScope.h
 *
 *  Created on: 28 апр. 2021 г.
 *      Author: Sergey Tyagushev
 */

#include "stdint.h"

#ifndef INC_MCUSCOPE_H_
#define INC_MCUSCOPE_H_

#define SCOPE_DATAFRAME_SIZE 	(240*4)
#define SCOPE_FRAME_SIZE 		(SCOPE_DATAFRAME_SIZE + 3)
#define SCOPE_FRAMEBUFFER_SIZE 	(SCOPE_FRAME_SIZE * 7)
#define SCOPE_CHANNEL_MAX 		(6)

void vTask_Scope(void *argument);

typedef struct { // Scope structure
  uint8_t Send_buf[SCOPE_FRAMEBUFFER_SIZE];
  	struct{
		uint8_t chn_num;
		uint8_t delay;
	}CNTRL;
  uint16_t *adr[SCOPE_CHANNEL_MAX];
} TypeDef_xSCOPE;


#endif /* INC_MCUSCOPE_H_ */
