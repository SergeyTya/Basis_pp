/**
* \addtogroup BasisPanelDevice BasisPanelDevice
* @{
*/

/**
\file BasisPanelDeviceObjects
\author ET9300Utilities.ApplicationHandler (Version 1.3.6.0) | EthercatSSC@beckhoff.com

\brief BasisPanelDevice specific objects<br>
\brief NOTE : This file will be overwritten if a new object dictionary is generated!<br>
*/

#if defined(_BASIS_PANEL_DEVICE_) && (_BASIS_PANEL_DEVICE_ == 1)
#define PROTO
#else
#define PROTO extern
#endif
/******************************************************************************
*                    Object 0x1600 : Control unit process data mapping
******************************************************************************/
/**
* \addtogroup 0x1600 0x1600 | Control unit process data mapping
* @{
* \brief Object 0x1600 (Control unit process data mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - SubIndex 001<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1600[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex1 - SubIndex 001 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1600[] = "Control unit process data mapping\000"
"SubIndex 001\000\377";
#endif //#ifdef _OBJD_

#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Reference to 0x7000.1 */
} OBJ_STRUCT_PACKED_END
TOBJ1600;
#endif //#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1600 ControlUnitProcessDataMapping0x1600
#if defined(_BASIS_PANEL_DEVICE_) && (_BASIS_PANEL_DEVICE_ == 1)
={1,0x70000120}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1A00 : Slave Data process data mapping
******************************************************************************/
/**
* \addtogroup 0x1A00 0x1A00 | Slave Data process data mapping
* @{
* \brief Object 0x1A00 (Slave Data process data mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - SubIndex 001<br>
* SubIndex 2 - SubIndex 002<br>
* SubIndex 3 - SubIndex 003<br>
* SubIndex 4 - SubIndex 004<br>
* SubIndex 5 - SubIndex 005<br>
* SubIndex 6 - SubIndex 006<br>
* SubIndex 7 - SubIndex 007<br>
* SubIndex 8 - SubIndex 008<br>
* SubIndex 9 - SubIndex 009<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A00[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex1 - SubIndex 001 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex2 - SubIndex 002 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex3 - SubIndex 003 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex4 - SubIndex 004 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex5 - SubIndex 005 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex6 - SubIndex 006 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex7 - SubIndex 007 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex8 - SubIndex 008 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex9 - SubIndex 009 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1A00[] = "Slave Data process data mapping\000"
"SubIndex 001\000"
"SubIndex 002\000"
"SubIndex 003\000"
"SubIndex 004\000"
"SubIndex 005\000"
"SubIndex 006\000"
"SubIndex 007\000"
"SubIndex 008\000"
"SubIndex 009\000\377";
#endif //#ifdef _OBJD_

#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Reference to 0x6000.1 */
UINT32 SI2; /* Subindex2 - Reference to 0x6000.2 */
UINT32 SI3; /* Subindex3 - Reference to 0x6000.3 */
UINT32 SI4; /* Subindex4 - Reference to 0x6000.4 */
UINT32 SI5; /* Subindex5 - Reference to 0x6000.5 */
UINT32 SI6; /* Subindex6 - Reference to 0x6000.6 */
UINT32 SI7; /* Subindex7 - Reference to 0x6000.7 */
UINT32 SI8; /* Subindex8 - Reference to 0x6000.8 */
UINT32 SI9; /* Subindex9 - Reference to 0x6000.9 */
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 SlaveDataProcessDataMapping0x1A00
#if defined(_BASIS_PANEL_DEVICE_) && (_BASIS_PANEL_DEVICE_ == 1)
={9,0x60000120,0x60000220,0x60000320,0x60000420,0x60000520,0x60000620,0x60000720,0x60000820,0x60000920}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1C12 : SyncManager 2 assignment
******************************************************************************/
/**
* \addtogroup 0x1C12 0x1C12 | SyncManager 2 assignment
* @{
* \brief Object 0x1C12 (SyncManager 2 assignment) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
* 
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C12[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0x1C12[] = "SyncManager 2 assignment\000\377";
#endif //#ifdef _OBJD_

#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT16 aEntries[1];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJ1C12;
#endif //#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1C12 sRxPDOassign
#if defined(_BASIS_PANEL_DEVICE_) && (_BASIS_PANEL_DEVICE_ == 1)
={1,{0x1600}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1C13 : SyncManager 3 assignment
******************************************************************************/
/**
* \addtogroup 0x1C13 0x1C13 | SyncManager 3 assignment
* @{
* \brief Object 0x1C13 (SyncManager 3 assignment) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
* 
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C13[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0x1C13[] = "SyncManager 3 assignment\000\377";
#endif //#ifdef _OBJD_

#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT16 aEntries[1];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJ1C13;
#endif //#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1C13 sTxPDOassign
#if defined(_BASIS_PANEL_DEVICE_) && (_BASIS_PANEL_DEVICE_ == 1)
={1,{0x1A00}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x6000 : Slave Data
******************************************************************************/
/**
* \addtogroup 0x6000 0x6000 | Slave Data
* @{
* \brief Object 0x6000 (Slave Data) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - AC1_Voltage<br>
* SubIndex 2 - AC2_Voltage<br>
* SubIndex 3 - DC1_Voltage<br>
* SubIndex 4 - DC2_Voltage<br>
* SubIndex 5 - AC1_Current<br>
* SubIndex 6 - AC2_Currnet<br>
* SubIndex 7 - DC1_Current<br>
* SubIndex 8 - DC2_Current<br>
* SubIndex 9 - State<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }, /* Subindex1 - AC1_Voltage */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }, /* Subindex2 - AC2_Voltage */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }, /* Subindex3 - DC1_Voltage */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }, /* Subindex4 - DC2_Voltage */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }, /* Subindex5 - AC1_Current */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }, /* Subindex6 - AC2_Currnet */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }, /* Subindex7 - DC1_Current */
{ DEFTYPE_REAL32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }, /* Subindex8 - DC2_Current */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ | OBJACCESS_TXPDOMAPPING }}; /* Subindex9 - State */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "Slave Data\000"
"AC1_Voltage\000"
"AC2_Voltage\000"
"DC1_Voltage\000"
"DC2_Voltage\000"
"AC1_Current\000"
"AC2_Currnet\000"
"DC1_Current\000"
"DC2_Current\000"
"State\000\377";
#endif //#ifdef _OBJD_

#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 AC1_Voltage; /* Subindex1 - AC1_Voltage */
UINT32 AC2_Voltage; /* Subindex2 - AC2_Voltage */
UINT32 DC1_Voltage; /* Subindex3 - DC1_Voltage */
UINT32 DC2_Voltage; /* Subindex4 - DC2_Voltage */
UINT32 AC1_Current; /* Subindex5 - AC1_Current */
UINT32 AC2_Currnet; /* Subindex6 - AC2_Currnet */
UINT32 DC1_Current; /* Subindex7 - DC1_Current */
UINT32 DC2_Current; /* Subindex8 - DC2_Current */
UINT32 State; /* Subindex9 - State */
} OBJ_STRUCT_PACKED_END
TOBJ6000;
#endif //#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 SlaveData0x6000
#if defined(_BASIS_PANEL_DEVICE_) && (_BASIS_PANEL_DEVICE_ == 1)
={9,0,0,0,0,0,0,0,0,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x7000 : Control unit
******************************************************************************/
/**
* \addtogroup 0x7000 0x7000 | Control unit
* @{
* \brief Object 0x7000 (Control unit) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Control<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READWRITE | OBJACCESS_RXPDOMAPPING }}; /* Subindex1 - Control */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7000[] = "Control unit\000"
"Control\000\377";
#endif //#ifdef _OBJD_

#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 Control; /* Subindex1 - Control */
} OBJ_STRUCT_PACKED_END
TOBJ7000;
#endif //#ifndef _BASIS_PANEL_DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7000 ControlUnit0x7000
#if defined(_BASIS_PANEL_DEVICE_) && (_BASIS_PANEL_DEVICE_ == 1)
={1,0}
#endif
;
/** @}*/







#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
/* Object 0x1600 */
{NULL , NULL ,  0x1600 , {DEFTYPE_UNSIGNED8 , 1 | (OBJCODE_REC << 8)} , asEntryDesc0x1600 , aName0x1600 , &ControlUnitProcessDataMapping0x1600, NULL , NULL , 0x0000 },
/* Object 0x1A00 */
{NULL , NULL ,  0x1A00 , {DEFTYPE_UNSIGNED8 , 9 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &SlaveDataProcessDataMapping0x1A00, NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign, NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign, NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_UNSIGNED8 , 9 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &SlaveData0x6000, NULL , NULL , 0x0000 },
/* Object 0x7000 */
{NULL , NULL ,  0x7000 , {DEFTYPE_UNSIGNED8 , 1 | (OBJCODE_REC << 8)} , asEntryDesc0x7000 , aName0x7000 , &ControlUnit0x7000, NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_
#undef PROTO

/** @}*/
#define _BASIS_PANEL_DEVICE_OBJECTS_H_
