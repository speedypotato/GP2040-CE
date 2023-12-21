#include "addons/sdvx.h"
#include "storagemanager.h"
#include "helper.h"
#include "config.pb.h"

#include "enums.pb.h"

#include "hardware/adc.h"

#include <math.h>

bool SdvxInput::available() {
    return Storage::getInstance().getAddonOptions().sdvxOptions.enabled;
}

void SdvxInput::setup() {
    const SdvxOptions& sdvxOptions = Storage::getInstance().getAddonOptions().sdvxOptions;
}

void SdvxInput::process()
{
    const SdvxOptions& sdvxOptions = Storage::getInstance().getAddonOptions().sdvxOptions;
    Gamepad * gamepad = Storage::getInstance().GetGamepad();

    gamepad->state.lx = (uint16_t)(65535.0f*0.5);
    gamepad->state.ly = (uint16_t)(65535.0f*0.25);
}