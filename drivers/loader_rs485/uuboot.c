/**
 ******************************************************************************
 * @file    bootuart.c
 * @author  Tyagushev Sergey
 * @version V0.1.0
 * @date    17/07/2024
 * @brief   Universal Unit Bootloader
 ******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "uuboot.h"

// Ram buffer for programming
static volatile uint8_t suiaUUB_bufdata[0x100U];
// CRC32 value
static volatile uint32_t UUB_crc32_val = 0;
// Address counter for programming
static uint32_t suiUUBImg_len = 0;
// Address counter for reading
static uint32_t suiUUBVrf_Adr = 0;

static void UUB_run(void);
static void UUB_Erase(void);
static void UUB_Program(void);
static void UUB_Verify(void);
static void UUB_StoreCRC(void);
static inline void UUB_sleep(int id)
{
  do
  {
    id++;
    id -= 2;
  } while (id != 0);
}
unsigned char * __attribute__((__weak__, __alias__("GetDefaultID"))) UUBport_GetID(void);
size_t          __attribute__((__weak__, __alias__("GetDefaultIDlen"))) UUBport_GetIDlen(void);

unsigned char id_str[12] = "UUBOOT_0.777";


unsigned char * GetDefaultID(void){
  return id_str;
}

size_t GetDefaultIDlen(void){
  return sizeof(id_str);
}

void vBootloader_run(void)
{
  UUBport_ConfigHw();
  UUB_run();
}

static void UUB_run(void)
{
  uint8_t unlock_loader = 0;
  unsigned char tmp;
  uint32_t tmp32;
  unsigned char * id_str = UUBport_GetID();
  size_t  id_str_len = UUBport_GetIDlen();

  while (1)
  {
    switch (UUBport_ReceiveByte())
    {
    case UUB_INF_CHAR:
      UUBport_Send(id_str, id_str_len);
      break;
    case UUB_ERS_CHAR:
      if (unlock_loader)
      {
        UUB_Erase();
      }
      unlock_loader = 0;
      break;
    case UUB_PRG_CHAR:
      UUB_Program();
      break;
    case UUB_VRF_CHAR:
      if (unlock_loader)
      {
        UUB_Verify();
      }
      break;
    case UUB_AQN_CHAR:
      if (suiUUBImg_len != 0)
      {
        suiUUBVrf_Adr = 0;
        suiUUBImg_len = 0;
      }
      suiUUBVrf_Adr = 0;
      tmp = UUB_AQN_CHAR;
      UUBport_Send(&tmp, 1);
      unlock_loader = 1;
      break;
    case UUB_RST_CHAR:
      tmp = UUB_RST_CHAR;
      UUBport_Send(&tmp, 1);
      UUB_sleep(200);
      UUBport_RebootHW();
      break;
    case UUB_BAD_CHAR:
      tmp32 = UUB_BAS_ADR;
      UUBport_Send((uint8_t *)&tmp32, 4);
      break;
    case UUB_CID_CHAR:
      UUBport_Send(cpu_id, 9);
      break;
    case UUB_CRC_CHAR:
      UUB_StoreCRC();
      break;
    default:
      break;
    }
  }
}

void UUB_Erase(void)
{
  unsigned char str1[] = "EOK";
  unsigned char str2[] = "FLT";
  bool isOk = true;

  UUBport_EraseEEPROM();

  for (size_t i = 0; i < 0x60000; i++)
  {
    uint8_t *adr_vrf = (uint8_t *)(UUB_STR_ADR + i);
    if(*adr_vrf != 0xFF){
      isOk = false;
    }
  }
  
  if (isOk == true)
  {
    UUBport_Send(str1, 3);
  }
  else
  {
    UUBport_Send(str2, 3);
  }
}

void UUB_Program(void)
{
  unsigned char tmp[] = {UUB_AQN_CHAR};

  UUB_sleep(1000);

  for (size_t i = 0; i < 256; i++)
  {
    suiaUUB_bufdata[i] = UUBport_ReceiveByte();
  }

  UUBport_WriteArrayToEEPROM(UUB_STR_ADR + suiUUBImg_len, (uint8_t *)suiaUUB_bufdata, 256);

  suiUUBImg_len += 0x100U;
  UUBport_Send(tmp, 1);
}

void UUB_Verify(void)
{
  uint8_t *adr_vrf = (uint8_t *)(UUB_STR_ADR + suiUUBVrf_Adr);
  UUBport_Send((uint8_t *)adr_vrf, 0x100U);
  suiUUBVrf_Adr += 0x100U;
  
}

void UUB_StoreCRC()
{
  const unsigned char tmp[] = {UUB_CRC_CHAR};

  for (size_t i = 0; i < 4; i++)
  {
    ((uint8_t *)&UUB_crc32_val)[i] = UUBport_ReceiveByte();
  }
  for (size_t i = 0; i < 4; i++)
  {
    ((uint8_t *)&suiUUBImg_len)[i] = UUBport_ReceiveByte();
  }

  UUB_sleep(1000);

  UUBport_WriteArrayToEEPROM(UUB_INF1_ADR        , (uint8_t *)&suiUUBImg_len, 4);
  UUBport_WriteArrayToEEPROM(UUB_INF1_ADR + 0x10U, (uint8_t *)&UUB_crc32_val, 4);

  uint32_t crc_strg = *( (uint32_t *) (UUB_INF1_ADR+0x10)); 
  uint32_t len_strg = *( (uint32_t *) (UUB_INF1_ADR     )); 

  if(
       crc_strg == UUB_crc32_val
    && len_strg == suiUUBImg_len
    && crc_strg != 0xFFFFFFFF
    && len_strg != 0xFFFFFFFF
  ){
    UUBport_Send(tmp, 1);
  }
  
}
