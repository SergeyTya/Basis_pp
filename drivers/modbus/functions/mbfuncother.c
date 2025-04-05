/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbfuncother.c,v 1.8 2006/12/07 22:10:34 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
#include <stdio.h>


/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "FreeRTOS.h"
#include "queue.h"
extern QueueHandle_t xQueue;

#include "task_scope.h"
extern TypeDef_xSCOPE xSCOPE;



/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"

const char * build_info  = HASH_INFO ; 


#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/
static UCHAR ucMBSlaveID[MB_FUNC_OTHER_REP_SLAVEID_BUF];
static USHORT usMBSlaveIDLen;

/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBSetSlaveID(UCHAR ucSlaveID, BOOL xIsRunning,
              UCHAR const *pucAdditional, USHORT usAdditionalLen)
{
    eMBErrorCode eStatus = MB_ENOERR;

    /* the first byte and second byte in the buffer is reserved for
     * the parameter ucSlaveID and the running flag. The rest of
     * the buffer is available for additional data. */
    if (usAdditionalLen + 2 < MB_FUNC_OTHER_REP_SLAVEID_BUF)
    {
        usMBSlaveIDLen = 0;
        ucMBSlaveID[usMBSlaveIDLen++] = ucSlaveID;
        ucMBSlaveID[usMBSlaveIDLen++] = (UCHAR)(xIsRunning ? 0xFF : 0x00);
        if (usAdditionalLen > 0)
        {
            memcpy(&ucMBSlaveID[usMBSlaveIDLen], pucAdditional,
                   (size_t)usAdditionalLen);
            usMBSlaveIDLen += usAdditionalLen;
        }
    }
    else
    {
        eStatus = MB_ENORES;
    }

    return eStatus;
}

eMBException
eMBFuncReportSlaveID(UCHAR *pucFrame, USHORT *usLen)
{
    memcpy(&pucFrame[MB_PDU_DATA_OFF], &ucMBSlaveID[0], (size_t)usMBSlaveIDLen);
    *usLen = (USHORT)(MB_PDU_DATA_OFF + usMBSlaveIDLen);
    return MB_EX_NONE;
}

#endif

/**
*   0x14 function - Read scope channels data

     *        ____________________________________________RESPONSE FRAME________________________________
     *       +-----+----+----------+----+-----------+----------+----+-----------+-------+-----+-----+-----+
     *       |     |    |                   SCOPE_DATAFRAME_SIZE                |       |     |     |     |
     *       +-----+----+----------+----+-----------+----------+----+-----------+-------+-----+-----+-----+
     * SIZE  | 1b  | 1b |    2b    |    |    2b     |    2b    |    |    2b     |   1b  |  1b | 1b  | 2b  |
     *       +-----+----+----------+----+-----------+----------+----+-----------+-------+-----+-----+-----+
     * FRAME | ADR |CMD | DATA_CH_1| .. |DATA_CH_CNT| DATA_CH_1| .. |DATA_CH_CNT| DELAY | CNT | FIFO| CRC |
     *       +-----+----+---------------+-----------+----------+----+-----------+-------+-----+-----+-----+
     *
*/

eMBException
eMBFuncSendScopeData(UCHAR *pucFrame, USHORT *usLen)
{
    uint32_t Fadr;

    portBASE_TYPE xStatus = xQueueReceive(xQueue, &Fadr, 0);

    if (xStatus != pdPASS)
    {
        return MB_EX_SLAVE_BUSY;
    }

    memcpy(&pucFrame[MB_PDU_DATA_OFF], (uint8_t *)Fadr, SCOPE_FRAME_SIZE);
    *usLen = (USHORT)(MB_PDU_DATA_OFF + SCOPE_FRAME_SIZE);

    return MB_EX_NONE;
}

/**
*   0x18 function - Read scope channels address

     *        _________________________RESPONSE FRAME________________________________
     *
     *       +-----+----+--------+------+------+---------+---------+
     * index | 0   | 1  | 2    |   2-5  | ...  |        |          |
     *       +-----+----+--------+------+------+---------+---------+
     * FRAME | ADR |CMD | CNT  | ADR_CH1|      | ADR_CNT |    CRC  |
     *       +-----+----+------+--------+------+---------+---------+

*/

eMBException
eMBFuncGetScopeChnlAdr(UCHAR *pucFrame, USHORT *usLen)
{
    // pucFrame[MB_PDU_DATA_OFF] = SCOPE_CHANNEL_MAX;
    // memcpy(&pucFrame[MB_PDU_DATA_OFF+1], xSCOPE.adr, 4*SCOPE_CHANNEL_MAX);
    // *usLen = (USHORT)(MB_PDU_DATA_OFF+1+4*SCOPE_CHANNEL_MAX);
    return MB_EX_NONE;
}

/**
*   0x16 function - Write scope channels address

       ________________________________REQUEST FRAME________________________________
     *
     *       +-----+----+--------+------+------+---------+---------+
     * index | 0   | 1  | 2    |   2-5  | ...  |        |          |
     *       +-----+----+--------+------+------+---------+---------+
     * FRAME | ADR |CMD | CNT  | ADR_CH1|      | ADR_CNT |    CRC  |
     *       +-----+----+------+--------+------+---------+---------+

*/

eMBException
eMBFuncSetScopeChnlAdr(UCHAR *pucFrame, USHORT *usLen)
{
    // uint8_t cnt = pucFrame[MB_PDU_DATA_OFF];
    // if(cnt>SCOPE_CHANNEL_MAX){
    //      return MB_EX_ILLEGAL_DATA_VALUE;
    // }
    // memcpy(xSCOPE.adr, &pucFrame[MB_PDU_DATA_OFF+1], 4*cnt);
    // *usLen = 0;
    return MB_EX_NONE;
}

/**
*   0x19 function - Setup scope parameters

       ________________________________REQUEST FRAME________________________________
     *
     *       +-----+----+--------+--------+---------+
     * index | 0   | 1  |   2    |    3   | 4  | 5  |
     *       +-----+----+--------+--------+---------+
     * FRAME | ADR |CMD | CH_CNT | DELAY  |   CRC   |
     *       +-----+----+--------+--------+---------+

*/

eMBException
eMBFuncSetScopeParam(UCHAR *pucFrame, __attribute__((unused)) USHORT *usLen)
{

    // /*
    //  0 - count
    //  1 - freq
    //  * */
    // uint8_t count;
    // uint8_t freq;

    // count = pucFrame[MB_PDU_DATA_OFF];
    // freq = pucFrame[MB_PDU_DATA_OFF + 1];
    // if(count==0) {
    //     return MB_EX_ILLEGAL_DATA_VALUE;
    // }
    // if(count > SCOPE_CHANNEL_MAX){
    //     return MB_EX_ILLEGAL_DATA_VALUE;
    // }
    // xSCOPE.CNTRL.chn_num = count;
    // xSCOPE.CNTRL.delay = freq;
    return MB_EX_NONE;
}

/**
*   0x2B function - Report device info

     *        _________________________RESPONSE FRAME________________________________
     *
     *       +-----+----+-----------------+---------+
     * index | 0   | 1  |                 |    |    |
     *       +-----+----+-----------------+---------+
     * FRAME | ADR |CMD |   info string   |   CRC   |
     *       +-----+----+-----------------+---------+

*/
extern const char *build_info;
extern const char *device_info;


eMBException
eMBFunGetDeviceInfo(UCHAR *pucFrame, USHORT *usLen)
{
    size_t device_info_len = strlen(device_info); // device name
    size_t build_info_len = strlen(build_info);   // git hash

    char crc_str_info[8] = {0};                   // fw crc
    sprintf((char *) crc_str_info, "%X", *((unsigned *) (0x10)));

    memcpy(pucFrame, device_info, device_info_len);
    memcpy(&pucFrame[device_info_len], build_info, build_info_len);
    memcpy(&pucFrame[device_info_len + build_info_len], crc_str_info , 8);
  
    *usLen = device_info_len + build_info_len + 8;
    return MB_EX_NONE;
}

/**
   *   26 function - Get registers count

     *
     *
     *        _________________________RESPONSE FRAME________________________________
     *
     *       +-----+----+-----------------+-----------+
     * index | 0   | 1  |  2    |  3      |  4  |  5  |
     *       +-----+----+-----------------+-----------+
     * FRAME | ADR |CMD | CNT_HI| CNT_LO  |    CRC    |
     *       +-----+----+-----------------+-----------+
     *
*/

eMBException
eMBFunGetRegistersCount(UCHAR *pucFrame, USHORT *usLen)
{
     // holding address
    // uint16_t holding_cnt = REG_HOLDING_NREGS_x00;
    // pucFrame[MB_PDU_DATA_OFF    ] = ( (uint8_t *) &holding_cnt)[1];
    // pucFrame[MB_PDU_DATA_OFF + 1] = ( (uint8_t *) &holding_cnt)[0];
    // *usLen =  3;

    return MB_EX_NONE;
}

/**
*   27 function - Get register info

       ________________________________REQUEST FRAME________________________________
     *
     *       +-----+----+-----------------+-----------+
     * index | 0   | 1  |  2    |  3      |  4  |  5  |
     *       +-----+----+-----------------+-----------+
     * FRAME | ADR |CMD | ARD_HI| ARD_LO  |    CRC    |
     *       +-----+----+-----------------+-----------+
     *

        _______________________________RESPONSE FRAME_________________________________
     *
     *       +-----+----+--------------+-------+----+-----+----+------------------+---------+
     * index | 0   | 1  |   2  |   3   |4|5|6|7| 8  |  9  | 10 |                  |    |    |
     *       +-----+----+--------------+-------+--- +-----+----+------------------+---------+
     * FRAME | ADR |CMD |adr_hi|adr_lo |MEM_ADR|type|index| RO | info string      |   CRC   |
     *       +-----+----+--------------+-------+----+-----+----+------------------+---------+
*/

eMBException
eMBFunGetRegisterInfo(UCHAR *pucFrame, USHORT *usLen)
{
    uint16_t adr = pucFrame[MB_PDU_DATA_OFF+1] + (pucFrame[MB_PDU_DATA_OFF]<<8);
    TypeDef_MB_Holding * hldg_ref = MB_GET_HOLDING_BASE_ADR(adr);

    // wrong reques size or wrong holding address
    if (hldg_ref == NULL)
    {
           return MB_EX_ILLEGAL_DATA_ADDRESS;
    }

    // holding address
    pucFrame[MB_PDU_DATA_OFF    ] = ( (uint8_t *) &adr)[1];
    pucFrame[MB_PDU_DATA_OFF + 1] = ( (uint8_t *) &adr)[0];
    // read 5 bytes from stucture (pointer + type)
    memcpy(&pucFrame[MB_PDU_DATA_OFF + 2], (uint8_t *) hldg_ref, 7);
    // info
    size_t reg_info_len = strlen(MB_GET_HOLDING_DSCRP(hldg_ref));
    memcpy(&pucFrame[MB_PDU_DATA_OFF+9], MB_GET_HOLDING_DSCRP(hldg_ref), reg_info_len);
    // frame size
    *usLen = 10 + reg_info_len ;

    return MB_EX_NONE;

}

/**
   *   28 function - Put holdings to scope

     *       ________________________________REQUEST FRAME________________________________
     *
     *  
     *       +-----+----+-------------+---+-----------+
     * index | 0   | 1  |2 |3 | 4| 5| 6| 7|8 |9 |11|12|
     *       +-----+----+-----+-----+-----+-----+-----+
     * FRAME | ADR |CMD |HI|LO|HI|LO|HI|LO|HI|LO| CRC |
     *       +-----+----+-----+-----+-----+-----+-----+
     *                  |    holding address    |
     *                  +-----------------------+
     * 
*/

eMBException
eMBFunPutHoldingsToScope(UCHAR *pucFrame, USHORT *usLen)
{

    // uint16_t adr0 = pucFrame[MB_PDU_DATA_OFF+1] + (pucFrame[MB_PDU_DATA_OFF+0]<<8);
    // uint16_t adr1 = pucFrame[MB_PDU_DATA_OFF+3] + (pucFrame[MB_PDU_DATA_OFF+2]<<8);
    // uint16_t adr2 = pucFrame[MB_PDU_DATA_OFF+5] + (pucFrame[MB_PDU_DATA_OFF+4]<<8);
    // uint16_t adr3 = pucFrame[MB_PDU_DATA_OFF+7] + (pucFrame[MB_PDU_DATA_OFF+6]<<8);

    // TypeDef_MB_Holding * hldg_ref0 = MB_GET_HOLDING_BASE_ADR(adr0);
    // TypeDef_MB_Holding * hldg_ref1 = MB_GET_HOLDING_BASE_ADR(adr1);
    // TypeDef_MB_Holding * hldg_ref2 = MB_GET_HOLDING_BASE_ADR(adr2);
    // TypeDef_MB_Holding * hldg_ref3 = MB_GET_HOLDING_BASE_ADR(adr3);

    // // wrong reques size or wrong holding address
    // if ( *usLen != 9 || 
    //      adr0 == 0 ||
    //      adr1 == 0 ||
    //      adr2 == 0 ||
    //      adr3 == 0
    // )
    // {
    //        return MB_EX_ILLEGAL_DATA_ADDRESS;
    // }

    // xSCOPE.adr[0] = MB_GET_HOLDING_VALUE_PNTR( (hldg_ref0));
    // xSCOPE.adr[1] = MB_GET_HOLDING_VALUE_PNTR( (hldg_ref1));
    // xSCOPE.adr[2] = MB_GET_HOLDING_VALUE_PNTR( (hldg_ref2));
    // xSCOPE.adr[3] = MB_GET_HOLDING_VALUE_PNTR( (hldg_ref3));

    return MB_EX_NONE;
}

void events_dummy(void * arg){}
