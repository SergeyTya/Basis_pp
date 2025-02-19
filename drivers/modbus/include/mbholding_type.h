#ifndef MBHOLDING_TYPE_H_
#define MBHOLDING_TYPE_H_

#include "stdbool.h"

typedef struct TypeDef_MB_Holding{
  /*-------FRAME---------*/
  uint16_t *  pntr;
  uint8_t     type;
  uint8_t     index; // use it for 32bit 
  bool        readOnly;
  const char  * desc;  
  /*---------------------*/
  uint16_t      reg_adr;  
  uint16_t *    pntr_base; // use it for 32bit 
  float         min;
  float         max;
  float         def;

  bool        lim_enbl;

  void (*on_change)(void *);
  
} TypeDef_MB_Holding;

typedef struct TypeDef_MB_Table {

  TypeDef_MB_Holding holdings[256];
  size_t len;
  uint16_t adr;

}TypeDef_MB_Table;


/*
        type
        0 - uint16_t
        1 -  int16_t
        2 - uint32_t
        3 -  int32_t
        4 -  float
        4  - _iq16
        6  - _iq18
        8  - _iq20
        10 - _iq16
        6  - _iq18
        8  - _iq20

*/

#define    HTYPE_NONE     255
#define    HTYPE_UINT16   0
#define    HTYPE_INT16    1
#define    HTYPE_UINT32   2
#define    HTYPE_INT32    3
#define    HTYPE_FLOAT    4

#define   MB_INDEX_0 0
#define   MB_INDEX_1 1

/*Read only general*/
#define HR_CREATE_RO(adr, var, var_base, info, tp, ind)\
{.reg_adr = adr, .pntr = (uint16_t *) &var, .pntr_base = (uint16_t *) &var_base, .desc = info, .type = tp, .index = ind, .readOnly = 1, .on_change=NULL, } 

/*read write general*/
#define HR_CREATE_RW(adr, var, var_base, info, tp, ind)\
{.reg_adr = adr, .pntr = (uint16_t *) &var, .pntr_base = (uint16_t *) &var_base, .desc = info, .type = tp, .index = ind, .readOnly = 0, .on_change=NULL}

/*Read only UINT16 register*/
#define HR_CREATE_UINT16_RO(adr, var, info) \
HR_CREATE_RO(adr, var, var, info, (uint8_t) HTYPE_UINT16, MB_INDEX_0)

/*Read only INT16 register*/
#define HR_CREATE_INT16_RO(adr, var, info) \
HR_CREATE_RO(adr, var, var, info, (uint8_t) HTYPE_INT16, MB_INDEX_0)

/*Read only UINT32 register*/
#define HR_CREATE_UINT32_RO(adr, var, info) \
HR_CREATE_RO(adr,(((uint16_t *) &var)[0]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_UINT32, MB_INDEX_0),\
HR_CREATE_RO(adr+1,(((uint16_t *) &var)[1]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_UINT32, MB_INDEX_1)

/*Read only INT32 register*/
#define HR_CREATE_INT32_RO(adr, var, info) \
HR_CREATE_RO(adr,(((uint16_t *) &var)[0]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_INT32, MB_INDEX_0),\
HR_CREATE_RO(adr+1,(((uint16_t *) &var)[1]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_INT32, MB_INDEX_1)

/*Read only FLOAT register*/
#define HR_CREATE_FLOAT_RO(adr,var, info) \
HR_CREATE_RO(adr, (((uint16_t *) &var)[0]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_FLOAT, MB_INDEX_0),\
HR_CREATE_RO(adr+1, (((uint16_t *) &var)[1]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_FLOAT, MB_INDEX_1)

/*RW uint16_t register*/
#define HR_CREATE_UINT16_RW(adr,var, info) \
HR_CREATE_RW(adr, var, info, (uint8_t) HTYPE_UINT16, MB_INDEX_0)

/*RW int16_t register*/
#define HR_CREATE_INT16_RW(adr,var, info) \
HR_CREATE_RW(adr, var, info, (uint8_t) HTYPE_INT16, MB_INDEX_0)

/*Read only UINT32 register*/
#define HR_CREATE_UINT32_RW(adr,var, info) \
HR_CREATE_RW(adr, (((uint16_t *) &var)[0]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_UINT32, MB_INDEX_0),\
HR_CREATE_RW(adr+1,(((uint16_t *) &var)[1]), (((uint16_t *) &var)[0]),info, (uint8_t) HTYPE_UINT32, MB_INDEX_1)

/*Read only INT32 register*/
#define HR_CREATE_INT32_RW(adr,var, info) \
HR_CREATE_RW(adr,(((uint16_t *) &var)[0]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_INT32, MB_INDEX_0),\
HR_CREATE_RW(adr+1,(((uint16_t *) &var)[1]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_INT32, MB_INDEX_1)

/*Read write FLOAT register*/
#define HR_CREATE_FLOAT_RW(adr,var, info) \
HR_CREATE_RW(adr,(((uint16_t *) &var)[0]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_FLOAT, MB_INDEX_0),\
HR_CREATE_RW(adr+1,(((uint16_t *) &var)[1]), (((uint16_t *) &var)[0]), info, (uint8_t) HTYPE_FLOAT, MB_INDEX_1)

/*FLOAT parameter*/
#define HR_CREATE_FLOAT_PARAM(adr, var, info, min_val, max_val, def_val, change_even_handler) \
{.reg_adr = adr, .pntr = &(((uint16_t *) &var)[0]), .pntr_base = &(((uint16_t *) &var)[0]), .desc = info, .type = (uint8_t) HTYPE_FLOAT, .lim_enbl=1, .min=min_val, .max=max_val, .def=def_val, .index = MB_INDEX_0, .on_change=change_even_handler},\
{.reg_adr = adr+1, .pntr = &(((uint16_t *) &var)[1]), .pntr_base = &(((uint16_t *) &var)[0]), .desc = info, .type = (uint8_t) HTYPE_FLOAT, .lim_enbl=1, .min=min_val, .max=max_val, .def=def_val, .index = MB_INDEX_1, .on_change=change_even_handler}

/*UINT32 parameter*/
#define HR_CREATE_UINT32_PARAM(adr, var, info, min_val, max_val, def_val, change_even_handler) \
{.reg_adr = adr, .pntr = &(((uint16_t *) &var)[0]), .pntr_base = &(((uint16_t *) &var)[0]), .desc = info, .type = (uint8_t) HTYPE_UINT32, .lim_enbl=1, .min=min_val, .max=max_val, .def=def_val, .index = MB_INDEX_0, .on_change=change_even_handler},\
{.reg_adr = adr+1, .pntr = &(((uint16_t *) &var)[1]), .pntr_base = &(((uint16_t *) &var)[0]), .desc = info, .type = (uint8_t) HTYPE_UINT32, .lim_enbl=1, .min=min_val, .max=max_val, .def=def_val, .index = MB_INDEX_1, .on_change=change_even_handler}

/*INT32 parameter*/
#define HR_CREATE_INT32_PARAM(adr, var, info, min_val, max_val, def_val, change_even_handler) \
{.reg_adr = adr, .pntr = &(((uint16_t *) &var)[0]), .pntr_base = &(((uint16_t *) &var)[0]), .desc = info, .type = (uint8_t) HTYPE_INT32, .lim_enbl=1, .min=min_val, .max=max_val, .def=def_val, .index = MB_INDEX_0, .on_change=change_even_handler},\
{.reg_adr = adr+1, .pntr = &(((uint16_t *) &var)[1]), .pntr_base = &(((uint16_t *) &var)[0]), .desc = info, .type = (uint8_t) HTYPE_INT32, .lim_enbl=1, .min=min_val, .max=max_val, .def=def_val, .index = MB_INDEX_1, .on_change=change_even_handler}

/*RW uint16_t parameter*/
#define HR_CREATE_UINT16_PARAM(adr, var, info, min_val, max_val, def_val, change_even_handler) \
{.reg_adr = adr, .pntr = (uint16_t *) &var, .pntr_base = (uint16_t *) &var, .desc = info, .type = (uint8_t) HTYPE_UINT16, .lim_enbl=1, .min=min_val, .max=max_val, .def=def_val, .on_change=change_even_handler}

#define HR_CREATE_INT16_PARAM_NL(adr, var, info, min_val, max_val, def_val, change_even_handler) \
{.reg_adr = adr, .pntr = (uint16_t *) &var, .pntr_base = (uint16_t *) &var, .desc = info, .type = (uint8_t) HTYPE_INT16, .lim_enbl=0, .min=min_val, .max=max_val, .def=def_val, .on_change=change_even_handler}


/*RW bool parameter*/
#define HR_CREATE_BOOL_PARAM(adr, var, info, def, change_even_handler) \
HR_CREATE_UINT16_PARAM(adr, var, info, 0, 1, def, change_even_handler)



#endif