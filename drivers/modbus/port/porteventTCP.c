/*
 * FreeModbus Libary: lwIP Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- System includes ----------------------------------*/
#include "assert.h"


#include "cmsis_os.h"
/* ----------------------- lwIP ---------------------------------------------*/

#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/err.h"

#include "main.h"

#ifndef MBOX_NULL
#define MBOX_NULL 0
#endif

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_POLL_CYCLETIME       10     /* Poll cycle time is 100ms */
/* ----------------------- Static variables ---------------------------------*/
static QueueHandle_t xMailBox = MBOX_NULL;
static eMBEventType eMailBoxEvent;



/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInitTCP( void )
{
    eMailBoxEvent = EV_READY;
    // sys_mbox_new(&xMailBox, 10);
    // xMailBox = sys_mbox_new(  );
    xMailBox = xQueueCreate(5, sizeof(eMBEventType *));
    return xMailBox != MBOX_NULL ? TRUE : FALSE;
}


static inline void free_buf(sys_mbox_t *mbox)
{
    // if(osMessageQueueGetCount(*mbox))
    // {
    //     portNOP();
    // }
    // osMessageQueueDelete(*mbox);
}

void
vMBPortEventClose( void )
{
    if( xMailBox != MBOX_NULL )
    {
       // sys_mbox_free( xMailBox );
      // free_buf( xMailBox );
       
    }
}

static inline void post_event(QueueHandle_t mbox, void * data){
    while(
        xQueueSendToBack(mbox, &data, portMAX_DELAY) != pdPASS
    );
}

BOOL
xMBPortEventPostTCP( eMBEventType eEvent )
{
    eMailBoxEvent = eEvent;
   // sys_mbox_post( xMailBox, &eMailBoxEvent );
    post_event( xMailBox, &eMailBoxEvent );
    return TRUE;
}

#ifndef MY_ARCH_TIMEOUT
#define MY_ARCH_TIMEOUT 0xffffffffUL
#endif

static inline uint32_t fetch_event(QueueHandle_t mbox, void **msg, u32_t timeout){

    portBASE_TYPE status;
    uint32_t starttime = xTaskGetTickCount();

    if(timeout != 0)
    {
        status = xQueueReceive(mbox, msg, timeout);
        if (status == pdPASS)
        {
          return (xTaskGetTickCount() - starttime);
        }else{
            return MY_ARCH_TIMEOUT;
        }
    }else{
        xQueueReceive(mbox, msg, portMAX_DELAY );
        return (xTaskGetTickCount() - starttime);
    }
}

BOOL
xMBPortEventGetTCP( eMBEventType * eEvent )
{
    eMBEventType *  peMailBoxEvent;
    BOOL            xEventHappend = FALSE;
    u32_t           uiTimeSpent;                                             

  //  uiTimeSpent = sys_arch_mbox_fetch( xMailBox, &peMailBoxEvent, MB_POLL_CYCLETIME );
    uiTimeSpent = fetch_event( xMailBox,(void **) &peMailBoxEvent, MB_POLL_CYCLETIME );
  
    
    if( uiTimeSpent != MY_ARCH_TIMEOUT )
    {
        *eEvent = *( eMBEventType * ) peMailBoxEvent;
        eMailBoxEvent = EV_READY;
        xEventHappend = TRUE;
    }
    return xEventHappend;
}
