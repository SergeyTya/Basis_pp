/* -----------------------MODBUS----------------------------------*/
#ifndef _PORTSERIAL_FSESC6_7_MINI_H
#define _PORTSERIAL_FSESC6_7_MINI_H

#include "stdint.h"

#define ENTER_CRITICAL_SECTION() ;
#define EXIT_CRITICAL_SECTION()  ;

#define MB_BUF_SIZE_MAX     256L     /*!< Maximum size of a Modbus RTU frame. */

#ifndef BOOL
#define BOOL uint8_t
#define TRUE 1
#define FALSE 0
#endif

void usb_modbus_callback(const uint8_t* Buf, const uint32_t *Len);

#endif
