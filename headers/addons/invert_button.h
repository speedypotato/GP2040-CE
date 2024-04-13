#ifndef _INVERT_BUTTON_H
#define _INVERT_BUTTON_H

#include "gpaddon.h"

#include "GamepadEnums.h"
#include "types.h"

#ifndef INVERT_BUTTON_ENABLED
#define INVERT_BUTTON_ENABLED 0
#endif

#ifndef INVERT_B1
#define INVERT_B1 false
#endif

#ifndef INVERT_B2
#define INVERT_B2 false
#endif

#ifndef INVERT_B3
#define INVERT_B3 false
#endif

#ifndef INVERT_B4
#define INVERT_B4 false
#endif

#ifndef INVERT_L1
#define INVERT_L1 false
#endif

#ifndef INVERT_R1
#define INVERT_R1 false
#endif

#ifndef INVERT_L2
#define INVERT_L2 false
#endif

#ifndef INVERT_R2
#define INVERT_R2 false
#endif

#ifndef INVERT_S1
#define INVERT_S1 false
#endif

#ifndef INVERT_S2
#define INVERT_S2 false
#endif

#ifndef INVERT_L3
#define INVERT_L3 false
#endif

#ifndef INVERT_R3
#define INVERT_R3 false
#endif

#ifndef INVERT_A1
#define INVERT_A1 false
#endif

#ifndef INVERT_A2
#define INVERT_A2 false
#endif

#ifndef INVERT_UP
#define INVERT_UP false
#endif

#ifndef INVERT_DOWN
#define INVERT_DOWN false
#endif

#ifndef INVERT_LEFT
#define INVERT_LEFT false
#endif

#ifndef INVERT_RIGHT
#define INVERT_RIGHT false
#endif

// InvertButtonName Module Name
#define InvertButtonName "Invert Button"

class InvertButton : public GPAddon {
public:
    virtual bool available();
	virtual void setup();       // Invert Button Setup
    virtual void preprocess() {}
	virtual void process();     // Invert Button process
    virtual std::string name() { return InvertButtonName; }

    typedef struct {
        bool enabled = false;
        bool invertB1 = false;
        bool invertB2 = false;
        bool invertB3 = false;
        bool invertB4 = false;
        bool invertL1 = false;
        bool invertR1 = false;
        bool invertL2 = false;
        bool invertR2 = false;
        bool invertS1 = false;
        bool invertS2 = false;
        bool invertL3 = false;
        bool invertR3 = false;
        bool invertA1 = false;
        bool invertA2 = false;
        bool invertUp = false;
        bool invertDown = false;
        bool invertLeft = false;
        bool invertRight = false;
    } InvertButtonMap;
private:
    InvertButtonMap invertButtonMap;
};

#endif  // _INVERT_BUTTON_H