#include "mem.h"
#include "mbholding_type.h"
#include "mb.h"

extern uint16_t usMBCRC16();

/*
 *   Compute CRC16 based on holding pointers using only RW registers
 */
static uint16_t getHoldingsCRC()
{

    uint16_t *pntr = NULL;
    pntr = (uint16_t *)malloc(REG_HOLDING_NREGS_x00 * 2);
    size_t k = 0;

    for (size_t i = 0; i < REG_HOLDING_NREGS_x00; i++)
    {
        TypeDef_MB_Holding * hldg_ref = MB_GET_HOLDING_BASE_ADR(i);
        if ( MB_GET_HOLDING_isRO(hldg_ref)  == 0)              // check if RW
        {                                                // save only RW registers
            pntr[k++] = (uint16_t) MB_GET_HOLDING_VALUE_PNTR(hldg_ref); // store pointer address value
        }
    }

    uint16_t res = usMBCRC16((uint8_t *)pntr, k * 2); // compute crc
    free(pntr);

    return res;
}

/*
 * Save register values to memory
 */
bool save_holdings()
{
    uint16_t crc = getHoldingsCRC(); // get RW only holdings CRC

    uint16_t *pntr = NULL;
    pntr = (uint16_t *)malloc(REG_HOLDING_NREGS_x00 * 2);
    size_t k = 0;

    if (pntr == NULL){return false;}
        
    pntr[k++] = crc; // store crc
    for (size_t i = 0; i < REG_HOLDING_NREGS_x00; i++)
    {
        TypeDef_MB_Holding * hldg_ref = MB_GET_HOLDING_BASE_ADR(i);
        if (MB_GET_HOLDING_isRO(hldg_ref) == 0)
        {                                                     // save only RW registers
            pntr[k++] = *((uint16_t *)MB_GET_HOLDING_VALUE_PNTR(hldg_ref)); // store register value
        }
    }

    bool res = hw_write_FLASH(pntr, k); // sent to hw

    free(pntr);
    return res;
}

/*
 * Read register values from memory
 */
void load_holdings()
{
    uint16_t crc = getHoldingsCRC(); // get RW only holdings CRC
    size_t k = 0;
    uint16_t *pntr = NULL;
    pntr = (uint16_t *)malloc(REG_HOLDING_NREGS_x00 * 2);
        if (pntr == NULL){
        goto FOO_EXIT;
    }

    hw_read_FLASH(pntr, 1); // read CRC from HW

    if (crc != pntr[0])
    {
        goto FOO_EXIT;
    }

    // get RW registers count
    for (size_t i = 0; i < REG_HOLDING_NREGS_x00; i++)
    {
        TypeDef_MB_Holding * hldg_ref = MB_GET_HOLDING_BASE_ADR(i);
        if (MB_GET_HOLDING_isRO(hldg_ref) == 0)
        {
            k++;
        }
    }

    // read data from HW
    hw_read_FLASH(pntr, k + 1);
    k = 1;

    // write new registers values
    for (size_t i = 0; i < REG_HOLDING_NREGS_x00; i++)
    {
        TypeDef_MB_Holding * hldg_ref = MB_GET_HOLDING_BASE_ADR(i);
        if (MB_GET_HOLDING_isRO(hldg_ref) == 0)
        {
            *(uint16_t *)MB_GET_HOLDING_VALUE_PNTR(hldg_ref) = pntr[k++];
        }
    }
FOO_EXIT:
    free(pntr);
}

/*
 * Reset register to default values
 */
void default_holdings()
{
    for (size_t i = 0; i < REG_HOLDING_NREGS_x00; i++)
    {
         TypeDef_MB_Holding * hldg_ref = MB_GET_HOLDING_BASE_ADR(i);
        if (MB_GET_HOLDING_isRO(hldg_ref) == 0)
        { // save only RW registers
            switch (MB_GET_HOLDING_TYPE(hldg_ref))
            {
            case HTYPE_UINT16:
                *((uint16_t *)MB_GET_HOLDING_VALUE_PNTR(hldg_ref)) = (uint16_t) MB_GET_HOLDING_DEF(hldg_ref);
                break;

            case HTYPE_INT16:
                *((int16_t *)MB_GET_HOLDING_VALUE_PNTR(hldg_ref)) = (int16_t)MB_GET_HOLDING_DEF(hldg_ref);
                break;

            case HTYPE_UINT32:
                if (MB_GET_HOLDING_INDEX(hldg_ref) == 0)
                {
                    *((uint32_t *)MB_GET_HOLDING_VALUE_PNTR(hldg_ref)) = (uint32_t)MB_GET_HOLDING_DEF(hldg_ref);
                }
                break;
            case HTYPE_INT32:
                if (MB_GET_HOLDING_INDEX(hldg_ref) == 0)
                {
                    *((int32_t *)MB_GET_HOLDING_VALUE_PNTR(hldg_ref)) = (int32_t)MB_GET_HOLDING_DEF(hldg_ref);
                }
                break;
            case HTYPE_FLOAT:
                if (MB_GET_HOLDING_INDEX(hldg_ref) == 0)
                {
                    *((float *)MB_GET_HOLDING_VALUE_PNTR(hldg_ref)) = (float)MB_GET_HOLDING_DEF(hldg_ref);
                }
                break;

            default:
                break;
            }
        }
    }
}