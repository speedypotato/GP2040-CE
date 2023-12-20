#include "addons/sdvx.h"
#include "storagemanager.h"
#include "helper.h"
#include "config.pb.h"

#include "enums.pb.h"

#include "hardware/adc.h"

#include <math.h>

#define ADC_MAX ((1 << 12) - 1)
#define ANALOG_CENTER 0.5f // 0.5f is center
#define ANALOG_MAX 1.0f    // 1.0f is max

bool SdvxInput::available() {
    return Storage::getInstance().getAddonOptions().sdvxOptions.enabled;
}

void SdvxInput::setup() {
    const SdvxOptions& sdvxOptions = Storage::getInstance().getAddonOptions().sdvxOptions;

    // Make sure GPIO is high-impedance, no pullups etc
    if ( isValidPin(sdvxOptions.analogAdc1PinX) ) {
        adc_gpio_init(sdvxOptions.analogAdc1PinX);
    }
    if ( isValidPin(sdvxOptions.analogAdc1PinY) ) {
        adc_gpio_init(sdvxOptions.analogAdc1PinY);
    }
    if ( isValidPin(sdvxOptions.analogAdc2PinX) ) {
        adc_gpio_init(sdvxOptions.analogAdc2PinX);
    }
    if ( isValidPin(sdvxOptions.analogAdc2PinY) ) {
        adc_gpio_init(sdvxOptions.analogAdc2PinY);
    }
	
	// Read pins deviation from center for calibration
	if (sdvxOptions.auto_calibrate) {
		if ( isValidPin(sdvxOptions.analogAdc1PinX) ) {
			adc_select_input(sdvxOptions.analogAdc1PinX-26); // ANALOG1-X
			adc_1_x_center = adc_read();
		}
		if ( isValidPin(sdvxOptions.analogAdc1PinY) ) {
			adc_select_input(sdvxOptions.analogAdc1PinY-26); // ANALOG1-Y
			adc_1_y_center = adc_read();
		}
		if ( isValidPin(sdvxOptions.analogAdc2PinX) ) {
			adc_select_input(sdvxOptions.analogAdc2PinX-26); // ANALOG2-X
			adc_2_x_center = adc_read();
		}
		if ( isValidPin(sdvxOptions.analogAdc2PinY) ) {
			adc_select_input(sdvxOptions.analogAdc2PinY-26); // ANALOG2-Y
			adc_2_y_center = adc_read();
		}
	}
}

void SdvxInput::process()
{
    const sdvxOptions& sdvxOptions = Storage::getInstance().getAddonOptions().sdvxOptions;
    Gamepad * gamepad = Storage::getInstance().GetGamepad();
    float adc_1_x = ANALOG_CENTER;
    float adc_1_y = ANALOG_CENTER;
    float adc_2_x = ANALOG_CENTER;
    float adc_2_y = ANALOG_CENTER;
    float adc_deadzone = sdvxOptions.analog_deadzone / 200.0f;
    float x_magnitude_1 = 0.0f;
    float y_magnitude_1 = 0.0f;
    float x_magnitude_2 = 0.0f;
    float y_magnitude_2 = 0.0f;
    float magnitude = 0.0f;

    if ( isValidPin(sdvxOptions.analogAdc1PinX) ) {
        adc_1_x = readPin(sdvxOptions.analogAdc1PinX, adc_1_x_center, sdvxOptions.auto_calibrate);
        
        if ( sdvxOptions.analogAdc1Invert == InvertMode::INVERT_X ||
            sdvxOptions.analogAdc1Invert == InvertMode::INVERT_XY) {
            adc_1_x = ANALOG_MAX - adc_1_x;
        }
    }
    if ( isValidPin(sdvxOptions.analogAdc1PinY) ) {
        adc_1_y = readPin(sdvxOptions.analogAdc1PinY, adc_1_y_center, sdvxOptions.auto_calibrate);
        
        if ( sdvxOptions.analogAdc1Invert == InvertMode::INVERT_Y ||
            sdvxOptions.analogAdc1Invert == InvertMode::INVERT_XY) {
            adc_1_y = ANALOG_MAX - adc_1_y;
        }
    }
    if ( isValidPin(sdvxOptions.analogAdc2PinX) ) {
        adc_2_x = readPin(sdvxOptions.analogAdc2PinX, adc_2_x_center, sdvxOptions.auto_calibrate);
        
        if ( sdvxOptions.analogAdc2Invert == InvertMode::INVERT_X ||
            sdvxOptions.analogAdc2Invert == InvertMode::INVERT_XY) {
            adc_2_x = ANALOG_MAX - adc_2_x;
        }
    }
    if ( isValidPin(sdvxOptions.analogAdc2PinY) ) {
        adc_2_y = readPin(sdvxOptions.analogAdc2PinY, adc_2_y_center, sdvxOptions.auto_calibrate);

        if ( sdvxOptions.analogAdc2Invert == InvertMode::INVERT_Y ||
            sdvxOptions.analogAdc2Invert == InvertMode::INVERT_XY) {
            adc_2_y = ANALOG_MAX - adc_2_y;
        }
    }

    // Calculations for radialDeadzone() and adjustCircularity()
    // Apply scaled radial deadzones
    if (adc_1_x != ANALOG_CENTER && adc_1_y != ANALOG_CENTER) {
        x_magnitude_1 = adc_1_x - ANALOG_CENTER;
        y_magnitude_1 = adc_1_y - ANALOG_CENTER;
        magnitude = sqrt((x_magnitude_1 * x_magnitude_1) + (y_magnitude_1 * y_magnitude_1));
        if (adc_deadzone) {
            radialDeadzone(adc_1_x, adc_1_y, adc_deadzone, x_magnitude_1, y_magnitude_1, magnitude);
        }
    }

    if (adc_2_x != ANALOG_CENTER && adc_2_y != ANALOG_CENTER) {
        x_magnitude_2 = adc_2_x - ANALOG_CENTER;
        y_magnitude_2 = adc_2_y - ANALOG_CENTER;
        magnitude = sqrt((x_magnitude_2 * x_magnitude_2) + (y_magnitude_2 * y_magnitude_2));

        if (adc_deadzone) {
            radialDeadzone(adc_2_x, adc_2_y, adc_deadzone, x_magnitude_2, y_magnitude_2, magnitude);
        }
    }

    // Alter coordinates to force perfect circularity
    if (sdvxOptions.forced_circularity) {
        if (adc_1_x != ANALOG_CENTER && adc_1_y != ANALOG_CENTER)
            adjustCircularity(adc_1_x, adc_1_y, adc_deadzone, x_magnitude_1, y_magnitude_1, magnitude);
        if (adc_2_x != ANALOG_CENTER && adc_2_y != ANALOG_CENTER)
            adjustCircularity(adc_2_x, adc_2_y, adc_deadzone, x_magnitude_2, y_magnitude_2, magnitude);
    }

    // Convert to 16-bit value
    if ( sdvxOptions.analogAdc1Mode == DpadMode::DPAD_MODE_LEFT_ANALOG) {
        gamepad->state.lx = (uint16_t)(65535.0f*adc_1_x);
        gamepad->state.ly = (uint16_t)(65535.0f*adc_1_y);
    } else if ( sdvxOptions.analogAdc1Mode == DpadMode::DPAD_MODE_RIGHT_ANALOG) {
        gamepad->state.rx = (uint16_t)(65535.0f*adc_1_x);
        gamepad->state.ry = (uint16_t)(65535.0f*adc_1_y);
    }
    if ( sdvxOptions.analogAdc2Mode == DpadMode::DPAD_MODE_LEFT_ANALOG) {
        gamepad->state.lx = (uint16_t)(65535.0f*adc_2_x);
        gamepad->state.ly = (uint16_t)(65535.0f*adc_2_y);
    } else if ( sdvxOptions.analogAdc2Mode == DpadMode::DPAD_MODE_RIGHT_ANALOG) {
        gamepad->state.rx = (uint16_t)(65535.0f*adc_2_x);
        gamepad->state.ry = (uint16_t)(65535.0f*adc_2_y);
    }
}

float SdvxInput::readPin(int pin, uint16_t center, bool autoCalibrate) {
	adc_select_input(pin - 26);
	uint16_t adc_hold = adc_read();

	// Calibrate axis based on off-center
	uint16_t adc_calibrated;

	if (autoCalibrate) {
		if (adc_hold > center) {
			adc_calibrated = map(adc_hold, center, ADC_MAX, ADC_MAX / 2, ADC_MAX);
		}
		else if (adc_hold == center) {
			adc_calibrated = ADC_MAX / 2;
		}
		else {
			adc_calibrated = map(adc_hold, 0, center, 0, ADC_MAX / 2);
		}
	}
	else {
		adc_calibrated = adc_hold;
	}

	float adc_value = ((float)adc_calibrated) / ADC_MAX;

	return adc_value;
}

uint16_t SdvxInput::map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void SdvxInput::radialDeadzone(float& x, float& y, float deadzone, float x_magnitude, float y_magnitude, float magnitude) {
    if (magnitude < deadzone) {
        x = ANALOG_CENTER;
        y = ANALOG_CENTER;
    }
    else {
        float scaling_factor = (magnitude - deadzone) / (1.0f - (deadzone + (deadzone * 0.6f)));
        x = ((x_magnitude / magnitude) * scaling_factor) + ANALOG_CENTER;
        y = ((y_magnitude / magnitude) * scaling_factor) + ANALOG_CENTER;

        x = std::fmin(x, 1.0f);
        y = std::fmin(y, 1.0f);
    }
}

void SdvxInput::adjustCircularity(float& x, float& y, float deadzone, float x_magnitude, float y_magnitude, float magnitude) {
    if (magnitude > ANALOG_CENTER) {
        x = ((x_magnitude / magnitude) * ANALOG_CENTER + ANALOG_CENTER);
        y = ((y_magnitude / magnitude) * ANALOG_CENTER + ANALOG_CENTER);
    }
}