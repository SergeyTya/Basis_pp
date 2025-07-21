#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"

void SRAM_Init(void)
{

  
        // LFCS PIN ON
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);           
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_7); 
    gpio_bit_set(GPIOD, GPIO_PIN_7);

    // gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);           
    // gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6); 
    // gpio_bit_reset(GPIOD, GPIO_PIN_6);

    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3| GPIO_PIN_4);           
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_3| GPIO_PIN_4); 
    gpio_bit_reset(GPIOG, GPIO_PIN_3|GPIO_PIN_4);

  /* Enable FSMC clock */
  rcu_periph_clock_enable(RCU_GPIOD);
  rcu_periph_clock_enable(RCU_GPIOE);
  rcu_periph_clock_enable(RCU_GPIOF);
  rcu_periph_clock_enable(RCU_GPIOG);
  rcu_periph_clock_enable(RCU_EXMC);
  
/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+------------------+------------------+
 | PD0  <-> FSMC_D2  | 									  | PF0 <-> FSMC_A0  | PG0 <-> FSMC_A10 |
 | PD1  <-> FSMC_D3  | 									  | PF1 <-> FSMC_A1  | PG1 <-> FSMC_A11 |
 | PD4  <-> FSMC_NOE | 									  | PF2 <-> FSMC_A2  | PG2 <-> FSMC_A12 |
 | PD5  <-> FSMC_NWE | 									  | PF3 <-> FSMC_A3  | PG3 <-> FSMC_A13 |
 | PD8  <-> FSMC_D13 |									  | PF4 <-> FSMC_A4  | PG4 <-> FSMC_A14 |
 | PD9  <-> FSMC_D14 | 									  | PF5 <-> FSMC_A5  |  |
 | PD10 <-> FSMC_D15 | 									  | PF12 <-> FSMC_A6 |  |
 | 					 | PE7  <-> FSMC_D4   | PF13 <-> FSMC_A7 |------------------+
 |  				 | PE8  <-> FSMC_D5   | PF14 <-> FSMC_A8 |
 |                   | PE9  <-> FSMC_D6   | PF15 <-> FSMC_A9 |
 | PD14 <-> FSMC_D0  | PE10 <-> FSMC_D7   |------------------+
 | PD15 <-> FSMC_D1  | PE11 <-> FSMC_D8   |
 +-------------------| PE12 <-> FSMC_D9   |
   PD6-> FSMC_NWAIT  | PE13 <-> FSMC_D10  |
   PD7 <-> FSMC_NE1  | PE14 <-> FSMC_D11  |
                     | PE15 <-> FSMC_D12  |
                     +--------------------+
*/

#define EXMC_GPIOD \
          GPIO_PIN_0  /* EXMC_D2  */\
        | GPIO_PIN_1  /* EXMC_D3  */\
        | GPIO_PIN_4  /* EXMC_NOE */\
        | GPIO_PIN_5  /* EXMC_NWE */\
        | GPIO_PIN_8  /* EXMC_D13 */\
        | GPIO_PIN_9  /* EXMC_D14 */\
        | GPIO_PIN_10 /* EXMC_D15 */\
        | GPIO_PIN_14 /* EXMC_D0  */\
        | GPIO_PIN_15 /* EXMC_D1  */\
        | GPIO_PIN_6  

    gpio_af_set  (GPIOD, GPIO_AF_12, EXMC_GPIOD);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, EXMC_GPIOD);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, EXMC_GPIOD);



     


#define EXMC_GPIOF \
          GPIO_PIN_0  /* EXMC_A0 */\
        | GPIO_PIN_1  /* EXMC_A1 */\
        | GPIO_PIN_2  /* EXMC_A2 */\
        | GPIO_PIN_3  /* EXMC_A3 */\
        | GPIO_PIN_4  /* EXMC_A4 */\
        | GPIO_PIN_5  /* EXMC_A5 */\
        | GPIO_PIN_12 /* EXMC_A6 */\
        | GPIO_PIN_13 /* EXMC_A7 */\
        | GPIO_PIN_14 /* EXMC_A8 */\
        | GPIO_PIN_15 /* EXMC_A9 */\

    gpio_af_set  (GPIOF, GPIO_AF_12, EXMC_GPIOF);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, EXMC_GPIOF);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, EXMC_GPIOF);

#define EXMC_GPIOE \
          GPIO_PIN_7  /* EXMC_D4  */\
        | GPIO_PIN_8  /* EXMC_D5  */\
        | GPIO_PIN_9  /* EXMC_D6  */\
        | GPIO_PIN_10 /* EXMC_D7  */\
        | GPIO_PIN_11 /* EXMC_D8  */\
        | GPIO_PIN_12 /* EXMC_D9  */\
        | GPIO_PIN_13 /* EXMC_D10 */\
        | GPIO_PIN_14 /* EXMC_D11 */\
        | GPIO_PIN_15 /* EXMC_D12 */\

    gpio_af_set  (GPIOE, GPIO_AF_12, EXMC_GPIOE);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, EXMC_GPIOE);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, EXMC_GPIOE);

#define EXMC_GPIOG \
          GPIO_PIN_0  /* EXMC_A40  */\
        | GPIO_PIN_1  /* EXMC_A11  */\
        | GPIO_PIN_2  /* EXMC_A12  */\
        | GPIO_PIN_12 /* EXMC_NE3  */\

    gpio_af_set  (GPIOG, GPIO_AF_12, EXMC_GPIOG);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, EXMC_GPIOG);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, EXMC_GPIOG);

    /*-- FSMC Configuration ------------------------------------------------------*/
    exmc_norsram_parameter_struct nor_init_struct;
    exmc_norsram_timing_parameter_struct nor_timing_init_struct;

    /* configure timing parameter */
    nor_timing_init_struct.asyn_address_setuptime = 4*1;  // reference values for 168 Mhz
    nor_timing_init_struct.asyn_address_holdtime  = 4*1;
    nor_timing_init_struct.asyn_data_setuptime = 60*1;
    nor_timing_init_struct.bus_latency = 1;
    nor_timing_init_struct.syn_clk_division = EXMC_SYN_CLOCK_RATIO_2_CLK;
    nor_timing_init_struct.syn_data_latency = EXMC_DATALAT_2_CLK;
    nor_timing_init_struct.asyn_access_mode = EXMC_ACCESS_MODE_A;

     /* configure EXMC bus parameters */
    nor_init_struct.norsram_region = EXMC_BANK0_NORSRAM_REGION3;//
    nor_init_struct.address_data_mux = DISABLE;
    nor_init_struct.memory_type = EXMC_MEMORY_TYPE_SRAM;
    nor_init_struct.databus_width = EXMC_NOR_DATABUS_WIDTH_16B;
    nor_init_struct.burst_mode = DISABLE;
    nor_init_struct.asyn_wait = DISABLE; //
    nor_init_struct.nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
    nor_init_struct.wrap_burst_mode = DISABLE;

    nor_init_struct.memory_write = ENABLE; //
    nor_init_struct.write_mode = EXMC_ASYN_WRITE; //
    nor_init_struct.nwait_signal = DISABLE; //
    nor_init_struct.extended_mode = DISABLE; //
    nor_init_struct.nwait_config = EXMC_NWAIT_CONFIG_BEFORE;


    nor_init_struct.read_write_timing = &nor_timing_init_struct;
    nor_init_struct.write_timing = &nor_timing_init_struct;
    exmc_norsram_init(&nor_init_struct);

    /* enable the EXMC bank0 NORSRAM */
    exmc_norsram_enable(EXMC_BANK0_NORSRAM_REGION3);



}

