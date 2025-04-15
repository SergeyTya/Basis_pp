#include <string.h>
#include "M204D08AA.h"
#include "M204D08AA_port.h"


static void M204D08AA_WriteCmdAsync(uint8_t cmd, uint8_t data);
static void PrintString(char *string,size_t len);


void M204D08AA_WriteCmdAsync(uint8_t cmd, uint8_t data)
{
    uint16_t tmp = cmd*256 + data;
    M204D08AA_STB_SetState(true);
    M204D08AA_STB_WriteWord(tmp);
    for (size_t i = 0; i < 100; i++) // need some delay?
    {
        M204D08AA_STB_SetState(false);  
    }
    
    
}

//win1251 only!!!!!!!!!!
const char Decode2Rus[] = {
    'A',(char)0x80, 'B',(char)0x92,(char) 0x81,
    'E',(char)0x82, (char) 0x83,(char) 0x84,
    (char)0x85,'K',(char) 0x86,'M','H','O',
    (char) 0x87,'P','C','T', (char)0x88,'�','X',
    (char) 0x89, (char)0x8A,(char) 0x8B,
    (char) 0x8C,(char) 0x8D,(char) 0x8E,'b',
    (char) 0x8F,(char) 0xAC,(char) 0xAD
};
    
static void PrintString(char *string,size_t len )
{
    for (size_t i = 0; i < len; i++)
    {
        char c=string[i];
        if(c >= -64 && c< -32) {
            M204D08AA_WriteCmdAsync(250, Decode2Rus[64+c]);
        }
        else {
            M204D08AA_WriteCmdAsync(250,c);
        }
    }
}   

void M204D08AA_UpdateDisplayFromBuffer(char buff[80]){

    uint8_t cmd[] = {128, 192, 148, 212};

    // spilt by rows
    char (*pntr)[20]  = (char (*)[20])buff;

    M204D08AA_WriteCmdAsync(248,1);   //Display clean

    for (size_t i = 0; i < 4; i++)
    {
       
        M204D08AA_WriteCmdAsync(248,cmd[i]);
        PrintString(pntr[i],20);
    }
} 

void M204D08AA_DisplayInit(){
    M204D08AA_HardInit(); 
    vTaskDelay(300);
    M204D08AA_WriteCmdAsync(248,12);   //Display enable
    vTaskDelay(300);                  // Dispaly clean
    M204D08AA_WriteCmdAsync(248,1);   //Display clean
    vTaskDelay(300);
    M204D08AA_WriteCmdAsync(248,63);  // Brightness 25%
    vTaskDelay(300);
}
    
void M204D08AA_SetBrightnessLevel(int lvl){

    M204D08AA_WriteCmdAsync(248,8) ;
    vTaskDelay(100);
    M204D08AA_WriteCmdAsync(248,1);
    
    switch (lvl)
    {
        case 25:
        /* 25% */
        M204D08AA_WriteCmdAsync(248,0b111000); 
        break;
        case 50:
        /* 50% */
        M204D08AA_WriteCmdAsync(248,0b111001); 
        break;
        case 75:
        /* 75% */
        M204D08AA_WriteCmdAsync(248,0b111010); 
        break;
        case 100:
        /* 100% */
        M204D08AA_WriteCmdAsync(248,0b111011); 
        break;

    default:
        break;
    }

    vTaskDelay(100);
    M204D08AA_WriteCmdAsync(248,12); 

}


// https://habr.com/ru/articles/392757/