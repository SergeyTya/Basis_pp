/**
\addtogroup MCI_HW Parallel ESC Access
@{
*/

/**
\file mcihw.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains the interface to the ESC via MCI

\version 5.10

<br>Changes to version V5.01:<br>
V5.10 HW1: Add 32Bit test access during hardware initialization (only for 32Bit controller)<br>
V5.10 HW4: Add volatile directive for direct ESC DWORD/WORD/BYTE access<br>
           Add missing swapping in mcihw.c<br>
           Add "volatile" directive vor dummy vairables in enable and disable SyncManger functions<br>
           Add missing swapping in EL9800hw files<br>
<br>Changes to version V5.0:<br>
V5.01 HW1: Invalid ESC access function was used<br>
<br>Changes to version V4.30:<br>
V5.0 ESC4: Save SM disable/Enable. Operation may be pending due to frame handling.<br>
<br>Changes to version V4.20:<br>
V4.30 ESM: if mailbox SyncManager is disabled and bMbxRunning is true the SyncManger settings need to be revalidate<br>
V4.30 SYNC: change synchronisation control function. Add usage of 0x1C32:12 [SM missed counter].<br>
Calculate bus cycle time (0x1C32:02 ; 0x1C33:02) CalcSMCycleTime()<br>
V4.30 PDO: rename PDO specific functions (COE_xxMapping -> PDO_xxMapping and COE_Application -> ECAT_Application)<br>
V4.20 PCI 1: PC_APPLICATION removed<br>
V4.10 PCI 1: Support for PCI EtherCAT Slave<br>
V4.10 MCI 1: Support for PCI EtherCAT Slave<br>
<br>Changes to version V4.07:<br>
V4.08 ECAT 3: The AlStatusCode is changed as parameter of the function AL_ControlInd<br>
<br>Changes to version V4.06:<br>
V4.07 ECAT 1: The sources for SPI and MCI were merged (in ecat_def.h<br>
                   set the switch MCI_HW to 1 when using the MCI,<br>
                   set the switch SPI_HW (obsolete now EL9800_HW)to 1 when using the SPI<br>
V4.07 COEAPPL 2: The example is working for the NIOS with the evaluation board DBC2C20 V1.2<br>
                       which is available by Altera<br>
<br>Changes to version V3.20:<br>
V4.00 MCI 1: When the MAILBOX_QUEUE-switch was set, the mailbox access<br>
             in boot mode was not working correctly<br>
V4.00 MCI 2: The mailbox data link layer was not working correctly<br>
V4.00 MCI 3: if no outputs are supported, the watchdog has to be triggered by the SM3-Event<br>
V4.00 ECAT 1: The handling of the Sync Manager Parameter was included according to<br>
              the EtherCAT Guidelines and Protocol Enhancements Specification<br>
V4.00 APPL 1: The watchdog checking should be done by a microcontroller<br>
                 timer because the watchdog trigger of the ESC will be reset too<br>
                 if only a part of the sync manager data is written<br>
V4.00 APPL 4: The EEPROM access through the ESC is added
*/

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"

#include "ecat_def.h"

//#define    _MCIHW_ 1

#if MCI_HW
#define    _MCIHW_ 1
#include "mcihw.h"
#undef _MCIHW_
#define    _MCIHW_ 0

#include "ecatslv.h"
#include "ecatappl.h"

/*--------------------------------------------------------------------------------------
------
------    local Types and Defines
------
--------------------------------------------------------------------------------------*/
#define    ECAT_TIMER_ACK_INT             timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP) //clear interrupt flag
#define    HW_EcatIsr                     EXTI0_IRQHandler/*Irq interrupt service function*/
#define    APPL_1MsTimerIsr               TIMER2_IRQHandler/*Interrupt service function of millisecond timer*/
#define    ENABLE_ECAT_TIMER_INT          NVIC_EnableIRQ(TIMER2_IRQn) ;	
#define    DISABLE_ECAT_TIMER_INT         NVIC_DisableIRQ(TIMER2_IRQn) ;

#define    INIT_SYNC0_INT                 EXTI2_Configuration()		
#define    Sync0Isr                       EXTI2_IRQHandler                  //SYNC0 interrupt function
#define    DISABLE_SYNC0_INT              NVIC_DisableIRQ(EXTI2_IRQn)	    // disable SYNC0 interrupt 
#define    ENABLE_SYNC0_INT               NVIC_EnableIRQ(EXTI2_IRQn)	    // enable SYNC0 interrupt
#define    ACK_SYNC0_INT                  exti_interrupt_flag_clear(EXTI_2)


/*ECATCHANGE_START(V5.10) HW3*/

#define    INIT_SYNC1_INT                 EXTI3_Configuration()
#define    Sync1Isr                       EXTI3_IRQHandler
#define    DISABLE_SYNC1_INT              NVIC_DisableIRQ(EXTI3_IRQn)// disable SYNC1 interrupt 
#define    ENABLE_SYNC1_INT               NVIC_EnableIRQ(EXTI3_IRQn)  // enable SYNC1 interrupt 
#define    ACK_SYNC1_INT                  exti_interrupt_flag_clear(EXTI_3)

#define INIT_ECAT_TIMER           		  TMR_Init(10)

#define STOP_ECAT_TIMER            		  DISABLE_ECAT_TIMER_INT;/*disable timer interrupt*/ \

#define START_ECAT_TIMER          		  ENABLE_ECAT_TIMER_INT

// #define STOP_ECAT_TIMER                   timer_enable(TIMER2);		 
// #define START_ECAT_TIMER                  timer_disable(TIMER2);   			
/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/
TSYNCMAN    TmpSyncMan;

/*-----------------------------------------------------------------------------------------
------
------    local functions
------
-----------------------------------------------------------------------------------------*/

void SRAM_Init(void);
void TMR_Init(uint8_t);

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0 if initialization was successful

 \brief    This function initialize the EtherCAT Slave Interface.
*////////////////////////////////////////////////////////////////////////////////////////
UINT32 intMask = 0;
UINT16 ECAT_HW_Init(void)
{
    SRAM_Init();
    //TMR_Init();
	//Pdi reading and writing test
	do
    {
        intMask = 0x0093;
        HW_EscWriteDWord(intMask, ESC_AL_EVENTMASK_OFFSET);
        intMask = 0;
		HW_EscReadDWord(intMask, ESC_AL_EVENTMASK_OFFSET);
    }while(intMask != 0x0093);// PDI�ӿڲ���
	
			HW_EscReadDWord(intMask, 0);
			HW_EscReadDWord(intMask, 4);
			HW_EscReadDWord(intMask, 8);
			HW_EscReadDWord(intMask, 12);
	//Irq interrupt initialization
    INIT_ESC_INT;/* initialize the PDI - interrupt source*/
    /* initialize the AL_Event Mask Register */
    /* the AL Event-Mask register is initialized with 0, so that no ESC interrupt is generated yet,
       the AL Event-Mask register will be adapted in the function StartInputHandler in ecatslv.c
        when the state transition from PREOP to SAFEOP is made */
    HW_EscWriteWord(intMask, ESC_AL_EVENTMASK_OFFSET);

    /* enable the ESC-interrupt microcontroller specific,
        the macro ENABLE_ESC_INT should be defined in ecat_def.h */
    ENABLE_ESC_INT();

	//--------------------------------------------------SYNC0&&SYNC1�жϳ�ʼ��
    INIT_SYNC0_INT;
    INIT_SYNC1_INT;
    ENABLE_SYNC0_INT;
    ENABLE_SYNC1_INT;
	INIT_ECAT_TIMER;
    START_ECAT_TIMER;
    return 0;
}

void HW_Release(void)
{
}

#if BOOTSTRAPMODE_SUPPORTED
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function resets the hardware
*////////////////////////////////////////////////////////////////////////////////////////

void     HW_RestartTarget(void)
{
}
#endif /* BOOTSTRAPMODE_SUPPORTED */

#if ESC_EEPROM_EMULATION
/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0 if reload was successful

 \brief    This function is called when the master has request an EEPROM reload during EEPROM emulation

*////////////////////////////////////////////////////////////////////////////////////////
UINT16 HW_EepromReload (void)
{
    return 0;
}
#endif

#if AL_EVENT_ENABLED

volatile uint8_t exti_flg = 0;
// irq_handler
void HW_EcatIsr(void)
{	
    exti_flg++;
	PDI_Isr();
    exti_interrupt_flag_clear(EXTI_0);	
}
// sync0_handler
void Sync0Isr(void)
{
	Sync0_Isr();
	ACK_SYNC0_INT;
	
}
// sync1_handler
void Sync1Isr(void)
{	
	Sync1_Isr();
	ACK_SYNC1_INT;
}

#endif
#if ECAT_TIMER_INT
// 1ms_timer_handler
void APPL_1MsTimerIsr(void)
{
	ECAT_CheckTimer();
	ECAT_TIMER_ACK_INT;
}
		

#endif

#endif //#if MCI_HW
/** @} */
