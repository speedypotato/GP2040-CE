#ifndef _Sdvx_H
#define _Sdvx_H

#include "gpaddon.h"

#include "GamepadEnums.h"

#include "BoardConfig.h"

#include "enums.pb.h"

#ifndef SDVX_INPUT_ENABLED
#define SDVX_INPUT_ENABLED 0
#endif

// Sdvx Module Name
#define SdvxName "Sdvx"

class SdvxInput : public GPAddon {
public:
	virtual bool available();
	virtual void setup();       // SDVX Setup
	virtual void process();     // SDVX Process
	virtual void preprocess() {}
    virtual std::string name() { return SdvxName; }
};

#endif  // _Sdvx_H