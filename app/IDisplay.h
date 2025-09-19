#ifndef IDisplay_H_
#define IDisplay_H_

void DisplayInit();

void DisplayUpdateFromBuffer(char buff[80]);

void DisplaySetBrightnessLevel(int lvl);

void DisplayMapMutaba();
void DisplayMapWinstar();

typedef struct {
    void (*UpdateDisplayFromBuffer)(char buff[80]);
    void (*Init)();
    void (*SetBrightnessLevel)();
}Typeded_DisplayHw;

#endif
