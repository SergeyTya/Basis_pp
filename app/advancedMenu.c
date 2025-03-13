#include "task_panel.h"

const uint8_t acAdvancedMenuSize = 4;
const TypeDef_AdvancedMenuItem acAdvancedMenu[4][15] = {
    {
        {.adr = 110},
        {.adr = 102},
        {.adr = 103, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 103, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 103, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 103, .isBitfieldBit = true, .bitNumber = 3},
        {.adr = 103, .isBitfieldBit = true, .bitNumber = 4},
        {.adr = 103, .isBitfieldBit = true, .bitNumber = 5},
        {.adr = 103, .isBitfieldBit = true, .bitNumber = 6},
        {.adr = 103, .isBitfieldBit = true, .bitNumber = 7}
    },
    {
        {.adr = 110},
        {.adr = 111},
        {.adr = 112},
        {.adr = 113},
        {.adr = 114},
    },
    {
        {.adr = 121},
        {.adr = 122},
        {.adr = 123},
        {.adr = 124},
        {.adr = 125},
        {.adr = 126},
    },
    {
        {.adr = 130},
        {.adr = 131},
        {.adr = 132},
        {.adr = 133}
    }
};

const uint8_t adAdvancedMenuSize = 14;
const TypeDef_AdvancedMenuItem dcAdvancedMenu[14][15] = {
    {
        {.adr = 100},
        {.adr = 101},
        {.adr = 102},
        {.adr = 103},
        {.adr = 104, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 104, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 104, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 104, .isBitfieldBit = true, .bitNumber = 3},
        {.adr = 104, .isBitfieldBit = true, .bitNumber = 4},
    },
    {
        {.adr = 110},
        {.adr = 111},
        {.adr = 112},
        {.adr = 113},
        {.adr = 114},
        {.adr = 115},
    },
    {
        {.adr = 120},
        {.adr = 121},
        {.adr = 122},
        {.adr = 123},
        {.adr = 124},
        {.adr = 125},
        {.adr = 126},
        {.adr = 127},
        {.adr = 128},
        {.adr = 129},
        {.adr = 134},
        {.adr = 135},
    },
    {
        {.adr = 130},
        {.adr = 131},
        {.adr = 132},
        {.adr = 133},
    },
    {
        {.adr = 140, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 140, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 140, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 140, .isBitfieldBit = true, .bitNumber = 3},
        {.adr = 140, .isBitfieldBit = true, .bitNumber = 4},
        {.adr = 140, .isBitfieldBit = true, .bitNumber = 5},
        {.adr = 140, .isBitfieldBit = true, .bitNumber = 6},
        {.adr = 140, .isBitfieldBit = true, .bitNumber = 7},
        {.adr = 140, .isBitfieldBit = true, .bitNumber = 8},
        {.adr = 141}
    },
    {
        {.adr = 150, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 150, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 150, .isBitfieldBit = true, .bitNumber = 2},
    },
    {
        {.adr = 160, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 160, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 160, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 160, .isBitfieldBit = true, .bitNumber = 3},
        {.adr = 160, .isBitfieldBit = true, .bitNumber = 4},
        {.adr = 161, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 161, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 161, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 161, .isBitfieldBit = true, .bitNumber = 3},
        {.adr = 161, .isBitfieldBit = true, .bitNumber = 4},
    },
    {
        {.adr = 170, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 170, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 170, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 171},
        {.adr = 172},
        {.adr = 173},
        {.adr = 174},
        {.adr = 175},
    },
    {
        {.adr = 180},
        {.adr = 181},
        {.adr = 182},
    },
    {
        {.adr = 190, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 190, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 190, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 190, .isBitfieldBit = true, .bitNumber = 3},
        {.adr = 190, .isBitfieldBit = true, .bitNumber = 4},
        {.adr = 190, .isBitfieldBit = true, .bitNumber = 5},
        {.adr = 190, .isBitfieldBit = true, .bitNumber = 6},
        {.adr = 190, .isBitfieldBit = true, .bitNumber = 7},
    },
    {
        {.adr = 200, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 200, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 200, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 200, .isBitfieldBit = true, .bitNumber = 3},
        {.adr = 200, .isBitfieldBit = true, .bitNumber = 4},
    },
    {
        {.adr = 210},
        {.adr = 211},
        {.adr = 212},
        {.adr = 213},
        {.adr = 214}
    },
    {
        {.adr = 220, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 220, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 220, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 220, .isBitfieldBit = true, .bitNumber = 3},
        {.adr = 220, .isBitfieldBit = true, .bitNumber = 4},
        {.adr = 220, .isBitfieldBit = true, .bitNumber = 5},
        {.adr = 220, .isBitfieldBit = true, .bitNumber = 6},
        {.adr = 220, .isBitfieldBit = true, .bitNumber = 7},
    },
    {
        {.adr = 230},
        {.adr = 231},
    },
    {
        {.adr = 240, .isBitfieldBit = true, .bitNumber = 0},
        {.adr = 240, .isBitfieldBit = true, .bitNumber = 1},
        {.adr = 240, .isBitfieldBit = true, .bitNumber = 2},
        {.adr = 240, .isBitfieldBit = true, .bitNumber = 3},
        {.adr = 240, .isBitfieldBit = true, .bitNumber = 4},
        {.adr = 240, .isBitfieldBit = true, .bitNumber = 5},
        {.adr = 240, .isBitfieldBit = true, .bitNumber = 6},
        {.adr = 240, .isBitfieldBit = true, .bitNumber = 7},
        {.adr = 240, .isBitfieldBit = true, .bitNumber = 8},
    }
};