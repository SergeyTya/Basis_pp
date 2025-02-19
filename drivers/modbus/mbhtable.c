
#include "stdint.h"
#include "stddef.h"

#include "mb.h"
#include "mbhtable.h"

uint16_t shreg[2] = {0, 0}; // used for check limits "mbhtable.h"

TypeDef_MB_Table holdings_table[REG_HOLDINGS_TABLE_LEN] = REG_HOLDINGS_TABLE_INIT;

void mb_setHoldingValue(TypeDef_MB_Holding *reg_base_adr, uint16_t value)
{

  if (MB_GET_HOLDING_LIMENBL(reg_base_adr) == true)
  {
    if (MB_GET_HOLDING_INDEX(reg_base_adr) > 1)
    {
      return;
    }

    switch (MB_GET_HOLDING_TYPE(reg_base_adr))
    {
    case HTYPE_UINT16:
      shreg[0] = value;
      HCHECk_LIMITS(uint16_t, reg_base_adr);
      break;

    case HTYPE_INT16:
      shreg[0] = value;
      HCHECk_LIMITS(int16_t, reg_base_adr);
      break;

    case HTYPE_UINT32:

      shreg[MB_GET_HOLDING_INDEX(reg_base_adr)] = value;
      if (MB_GET_HOLDING_INDEX(reg_base_adr) == 1)
      {
        HCHECk_LIMITS(uint32_t, reg_base_adr);
      }
      break;
    case HTYPE_INT32:

      shreg[MB_GET_HOLDING_INDEX(reg_base_adr)] = value;
      if (MB_GET_HOLDING_INDEX(reg_base_adr) == 1)
      {
        HCHECk_LIMITS(int32_t, reg_base_adr);
      }
      break;
    case HTYPE_FLOAT:
      shreg[MB_GET_HOLDING_INDEX(reg_base_adr)] = value;
      if (MB_GET_HOLDING_INDEX(reg_base_adr) == 1)
      {
        HCHECk_LIMITS(float, reg_base_adr);
      }

      break;

    default:
      break;
    }
  }
  else
  {
    *MB_GET_HOLDING_VALUE_PNTR(reg_base_adr) = value;
  }
}

TypeDef_MB_Holding * mb_getHoldingBaseArd(uint16_t holding_addr)
{
  TypeDef_MB_Holding *ret_val = NULL;

  for (size_t i = 0; i < REG_HOLDINGS_TABLE_LEN; i++)
  {
    /* searching address in tables of holdings */
    if (holdings_table[i].adr == (holding_addr & 0xFF00U))
    {
      TypeDef_MB_Holding * src = holdings_table[i].holdings;
      size_t len =  holdings_table[i].len;

      for (size_t i = 0; i < len; i++)
      {
        if (src[i].reg_adr == holding_addr)
        {
          ret_val = &src[i];
          break;
        }
      }
      break;
    }
  }

  return ret_val;
}