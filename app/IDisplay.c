#include "IDisplay.h"

#include "IDisplay_WH2004A.h"
#include "IDisplay_M204D08AA.h"

void IDisplay_dummy();
static Typeded_DisplayHw displayHw = { .SetBrightnessLevel = IDisplay_dummy, .Init = IDisplay_dummy, .UpdateDisplayFromBuffer = IDisplay_dummy };

void IDisplay_dummy() {
    ;
}

void DisplayInit() {
    displayHw.Init();
}

void DisplayUpdateFromBuffer(char buff[80]) {
    displayHw.UpdateDisplayFromBuffer(buff);
}

void DisplaySetBrightnessLevel(int lvl) {
    displayHw.SetBrightnessLevel(lvl);
}

void DisplayMapMutaba() {

    displayHw.Init = M204D08AA_DisplayInit;
    displayHw.UpdateDisplayFromBuffer = M204D08AA_UpdateDisplayFromBuffer;
    displayHw.SetBrightnessLevel = M204D08AA_SetBrightnessLevel;

}

void DisplayMapWinstar() {
    displayHw.Init = WH2004A_DisplayInit;
    displayHw.UpdateDisplayFromBuffer = WH2004A_DisplayUpdateFromBuffer;
    displayHw.SetBrightnessLevel = WH2004A_DisplaySetBrightnessLevel;
}