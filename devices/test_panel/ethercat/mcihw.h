/**
 * \addtogroup MCI_HW Parallel ESC Access
 * @{
 */

/**
\file mcihw.h
\author EthercatSSC@beckhoff.com
\brief Defines and Macros to access the ESC via a parallel interface

\version 5.10

<br>Changes to version V5.01:<br>
V5.10 HW4: Add volatile directive for direct ESC DWORD/WORD/BYTE access<br>
           Add missing swapping in mcihw.c<br>
           Add "volatile" directive vor dummy vairables in enable and disable SyncManger functions<br>
           Add missing swapping in EL9800hw files<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */


#ifndef _MCIHW_H_
#define _MCIHW_H_


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include <string.h>
#include "stm32f4xx.h"
#include "stdlib.h"
#include "esc.h"
#include "system.h"

extern 	uint16_t uhADCxConvertedValue;
#define SWITCH_1             PGin(4)				// PORTDbits.RD7 /**< \brief Access to switch 1 input*/
#define SWITCH_2             PGin(5)				// PORTDbits.RD6 /**< \brief Access to switch 2 input*/
#define SWITCH_3             PGin(6)//PAin(14)				// PORTDbits.RD5 /**< \brief Access to switch 3 input*/
#define SWITCH_4             PGin(7)//PAin(13)				// PORTDbits.RD4 /**< \brief Access to switch 4 input*/
#define SWITCH_5             PGin(8)				// PORTDbits.RD3 /**< \brief Access to switch 5 input*/
#define SWITCH_6             PCin(6)				// PORTDbits.RD2 /**< \brief Access to switch 6 input*/
#define SWITCH_7             PCin(7)				// PORTDbits.RD1 /**< \brief Access to switch 7 input*/
#define SWITCH_8             PCin(8)				// PORTDbits.RD0 /**< \brief Access to switch 8 input*/

#define LED_1                PGout(3)				// LATBbits.LATB8 /**< \brief Access to led 1 output*/
#define LED_2                PDout(13)				//LATBbits.LATB9 /**< \brief Access to led 2 output*/
#define LED_3                PDout(12)				// LATBbits.LATB10 /**< \brief Access to led 3 output*/
#define LED_4                PDout(11)				//LATBbits.LATB11 /**< \brief Access to led 4 output*/
#define LED_5                PBout(15)				// LATBbits.LATB12 /**< \brief Access to led 5 output*/
#define LED_6                PBout(14)				// LATBbits.LATB13 /**< \brief Access to led 6 output*/
#define LED_7                PBout(13)				//LATBbits.LATB14 /**< \brief Access to led 7 output*/
#define LED_8                PBout(12)			


#define ET1100_PDI_BASEADDR  ((uint32_t)0x6c000000) /*Bank1_SRAM1_ADDR*/

void EXTI0_Configuration(void);// IRQ interrupt init
void EXTI1_Configuration(void);// SYNC0 interrupt init
void EXTI2_Configuration(void);// SYNC1 interrupt init
 
/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/
#define ESC_MEM_ADDR    UINT16 /**< \brief ESC address type (16Bit)*/

#if !ECAT_TIMER_INT || COE_SUPPORTED
#ifndef HW_GetTimer
/**
 * \todo Define a macro or implement a function to get the hardware timer value.<br>See SSC Application Note for further details, www.beckhoff.com/english.asp?download/ethercat_development_products.htm?id=71003127100387
 */
#define HW_GetTimer()		TIM3->CNT	
#endif

#ifndef HW_ClearTimer
/**
 * \todo Define a macro or implement a function to clear the hardware timer value.<br>See SSC Application Note for further details, www.beckhoff.com/english.asp?download/ethercat_development_products.htm?id=71003127100387
 */
#define HW_ClearTimer() 	TIM3->CNT	=0
#endif

#define    INIT_ESC_INT           		EXTI0_Configuration()	/*IRQ interrupt init*/

#define    DISABLE_ESC_INT()          NVIC_DisableIRQ(EXTI0_IRQn)/* disable IRQ interrupt*/	/**< \brief Disable interrupt source INT1*/

#define    ENABLE_ESC_INT()           NVIC_EnableIRQ(EXTI0_IRQn) /* enable IRQ interrupt*//**< \brief Enable interrupt source INT1*/


/**
 * \todo Define the hardware timer ticks per millisecond.
 *  2000 �Ƕ�ʱ����ʱ1ms�ļ���ֵ----2Mhz---1��clk0.5us
 */
#define ECAT_TIMER_INC_P_MS    2000

//#warning "define access to timer register(counter)"
#endif //#if !ECAT_TIMER_INT || COE_SUPPORTED
#define     HW_GetALEventRegister()                  (*(volatile UINT16 ESCMEM *)(ET1100_PDI_BASEADDR+(ESC_AL_EVENT_OFFSET))) /**< \brief Returns the first 16Bit of the AL Event register (0x220)*/
#define     HW_GetALEventRegister_Isr                HW_GetALEventRegister /**< \brief Returns the first 16Bit of the AL Event register (0x220).<br>Called from ISRs.*/

#define     HW_EscRead(pData,Address,Len)  		     ESCMEMCPY((MEM_ADDR *)(pData), (ESC_MEM_ADDR ESCMEM *)(ET1100_PDI_BASEADDR+((Address))), (Len)) /**< \brief Generic ESC (register and DPRAM) read access.*/
#define     HW_EscReadIsr                            HW_EscRead /**< \brief Generic ESC (register and DPRAM) read access.<br>Called for ISRs.*/
#define     HW_EscReadDWord(DWordValue, Address)     ((DWordValue) = (UINT32)(*(volatile UINT32 *)(ET1100_PDI_BASEADDR+(Address)))) /**< \brief 32Bit specific ESC (register and DPRAM) read access.*/
#define     HW_EscReadDWordIsr(DWordValue, Address)  ((DWordValue) =(UINT32)(*(volatile UINT32 *)(ET1100_PDI_BASEADDR+(Address))))  /**< \brief 32Bit specific ESC (register and DPRAM) read access.<br>Called from ISRs.*/
#if !ESC_32BIT_ACCESS
#define     HW_EscReadWord(WordValue, Address)       ((WordValue) = (*(volatile UINT16 *)(ET1100_PDI_BASEADDR+(Address)))) /**< \brief 16Bit specific ESC (register and DPRAM) read access.*/
#define     HW_EscReadWordIsr(WordValue, Address)    HW_EscReadWord(WordValue, Address) /**< \brief 16Bit specific ESC (register and DPRAM) read access.<br>Called from ISRs.*/
#if !ESC_16BIT_ACCESS
#define     HW_EscReadByte(ByteValue, Address)       ((ByteValue) = (*(volatile UINT8 *)(ET1100_PDI_BASEADDR+Address))) /**< \brief 8Bit specific ESC (register and DPRAM) read access.*/
#define     HW_EscReadByteIsr(ByteValue, Address)    HW_EscReadByte(ByteValue, Address) /**< \brief 8Bit specific ESC (register and DPRAM) read access.<br>Called from ISRs.*/
#endif
#endif
#define     HW_EscReadMbxMem(pData,Address,Len)      ESCMBXMEMCPY((MEM_ADDR *)(pData), (ESC_MEM_ADDR ESCMEM *)(ET1100_PDI_BASEADDR+((Address))), (Len)) /**< \brief Macro to copy data from the application mailbox memory(not the ESC memory, this access is handled by HW_EscRead).*/


#define     HW_EscWrite(pData,Address,Len)           ESCMEMCPY((ESC_MEM_ADDR ESCMEM *)(ET1100_PDI_BASEADDR+(Address)), (MEM_ADDR *)(pData), (Len)) /**< \brief Generic ESC (register and DPRAM) write access.*/
#define     HW_EscWriteIsr                           HW_EscWrite /**< \brief Generic ESC (register and DPRAM) write access.<br>Called for ISRs.*/
#define     HW_EscWriteDWord(DWordValue, Address)    ((*(volatile UINT32 *)(ET1100_PDI_BASEADDR+(Address))) = (DWordValue)) /**< \brief 32Bit specific ESC (register and DPRAM) write access.*/
#define     HW_EscWriteDWordIsr(DWordValue, Address) ((*(volatile UINT32 *)(ET1100_PDI_BASEADDR+(Address)))= (DWordValue)) /**< \brief 32Bit specific ESC (register and DPRAM) write access.<br>Called from ISRs.*/
#if !ESC_32BIT_ACCESS
#define     HW_EscWriteWord(WordValue, Address)      ((*(volatile UINT16 *)(ET1100_PDI_BASEADDR+(Address))) = (WordValue)) /**< \brief 16Bit specific ESC (register and DPRAM) write access.*/
#define     HW_EscWriteWordIsr(WordValue, Address)   HW_EscWriteWord(WordValue, Address) /**< \brief 16Bit specific ESC (register and DPRAM) write access.<br>Called from ISRs.*/
#if !ESC_16BIT_ACCESS
#define     HW_EscWriteByte(ByteValue, Address)      ((*(volatile UINT8 *)(ET1100_PDI_BASEADDR+Address)) = (ByteValue)) /**< \brief 8Bit specific ESC (register and DPRAM) write access.*/
#define     HW_EscWriteByteIsr(ByteValue, Address)   HW_EscWriteByte(ByteValue, Address) /**< \brief 8Bit specific ESC (register and DPRAM) write access.<br>Called from ISRs.*/
#endif
#endif
#define     HW_EscWriteMbxMem(pData,Address,Len)     ESCMBXMEMCPY(((ESC_MEM_ADDR ESCMEM *)(ET1100_PDI_BASEADDR+((Address)))),(MEM_ADDR *)(pData), (Len)) /**< \brief Macro to copy data from the application mailbox memory (not the ESC memory, this access is handled by HW_EscWrite).*/


#ifndef TIMER_INT_HEADER
#define    TIMER_INT_HEADER /**< \brief Compiler directive before the hardware timer ISR function*/
#endif

#define     ESC_RD                    0x02 /**< \brief Indicates a read access to ESC or EEPROM*/
#define     ESC_WR                    0x04 /**< \brief Indicates a write access to ESC or EEPROM.*/

#endif//_MCIHW_H_


/*-----------------------------------------------------------------------------------------
------
------    Global variables
------
-----------------------------------------------------------------------------------------*/

#if _MCIHW_
    #define PROTO
#else
    #define PROTO extern
#endif

//PROTO MEM_ADDR ESCMEM *            pEsc;            /**< \brief Pointer to the ESC.<br>Shall be initialized in HW_Init().*/


/*-----------------------------------------------------------------------------------------
------
------    Global functions
------
-----------------------------------------------------------------------------------------*/
		
void TIM_Configuration(uint8_t period);	
void TIM2_Configuration(uint8_t period);	
		
PROTO UINT16 ECAT_HW_Init(void);
PROTO void HW_Release(void);

#if BOOTSTRAPMODE_SUPPORTED
PROTO    void HW_RestartTarget(void);
#endif

#if ESC_EEPROM_EMULATION
PROTO UINT16 HW_EepromReload (void);
#endif

#undef PROTO
/** @}*/
