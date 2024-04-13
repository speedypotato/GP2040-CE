#include "addons/invert_button.h"

#include "storagemanager.h"
#include "types.h"

#include "GamepadEnums.h"
#include "helper.h"
#include "config.pb.h"

bool InvertButton::available() {
    const InvertButtonOptions& options = Storage::getInstance().getAddonOptions().invertButtonOptions;
    return options.enabled;
}

void InvertButton::setup() {
    const InvertButtonOptions& options = Storage::getInstance().getAddonOptions().invertButtonOptions;
    invertButtonMap.enabled = options.enabled;
    invertButtonMap.invertB1 = options.invertB1;
    invertButtonMap.invertB2 = options.invertB2;
    invertButtonMap.invertB3 = options.invertB3;
    invertButtonMap.invertB4 = options.invertB4;
    invertButtonMap.invertL1 = options.invertL1;
    invertButtonMap.invertR1 = options.invertR1;
    invertButtonMap.invertL2 = options.invertL2;
    invertButtonMap.invertR2 = options.invertR2;
    invertButtonMap.invertS1 = options.invertS1;
    invertButtonMap.invertS2 = options.invertS2;
    invertButtonMap.invertL3 = options.invertL3;
    invertButtonMap.invertR3 = options.invertR3;
    invertButtonMap.invertA1 = options.invertA1;
    invertButtonMap.invertA2 = options.invertA2;
    invertButtonMap.invertUp = options.invertUp;
    invertButtonMap.invertDown = options.invertDown;
    invertButtonMap.invertLeft = options.invertLeft;
    invertButtonMap.invertRight = options.invertRight;
}

void InvertButton::process() {
    Gamepad * gamepad = Storage::getInstance().GetGamepad();
    if (invertButtonMap.invertB1) gamepad->state.buttons ^= GAMEPAD_MASK_B1;
    if (invertButtonMap.invertB2) gamepad->state.buttons ^= GAMEPAD_MASK_B2;
    if (invertButtonMap.invertB3) gamepad->state.buttons ^= GAMEPAD_MASK_B3;
    if (invertButtonMap.invertB4) gamepad->state.buttons ^= GAMEPAD_MASK_B4;
    if (invertButtonMap.invertL1) gamepad->state.buttons ^= GAMEPAD_MASK_L1;
    if (invertButtonMap.invertR1) gamepad->state.buttons ^= GAMEPAD_MASK_R1;
    if (invertButtonMap.invertL2) gamepad->state.buttons ^= GAMEPAD_MASK_L2;
    if (invertButtonMap.invertR2) gamepad->state.buttons ^= GAMEPAD_MASK_R2;
    if (invertButtonMap.invertS1) gamepad->state.buttons ^= GAMEPAD_MASK_S1;
    if (invertButtonMap.invertS2) gamepad->state.buttons ^= GAMEPAD_MASK_S2;
    if (invertButtonMap.invertL3) gamepad->state.buttons ^= GAMEPAD_MASK_L3;
    if (invertButtonMap.invertR3) gamepad->state.buttons ^= GAMEPAD_MASK_R3;
    if (invertButtonMap.invertA1) gamepad->state.buttons ^= GAMEPAD_MASK_A1;
    if (invertButtonMap.invertA2) gamepad->state.buttons ^= GAMEPAD_MASK_A2;
    if (invertButtonMap.invertUp) gamepad->state.buttons ^= GAMEPAD_MASK_DU;
    if (invertButtonMap.invertDown) gamepad->state.buttons ^= GAMEPAD_MASK_DD;
    if (invertButtonMap.invertLeft) gamepad->state.buttons ^= GAMEPAD_MASK_DL;
    if (invertButtonMap.invertRight) gamepad->state.buttons ^= GAMEPAD_MASK_DR;

    //todo: lx/ly/rx/ry?
}