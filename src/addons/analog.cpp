#include "addons/analog.h"
#include "config.pb.h"
#include "enums.pb.h"
#include "hardware/adc.h"
#include "helper.h"
#include "storagemanager.h"
#include "drivermanager.h"

#include <math.h>

#define ADC_MAX ((1 << 12) - 1) // 4095
#define ADC_PIN_OFFSET 26
#define ANALOG_MAX 1.0f
#define ANALOG_CENTER 0.5f
#define ANALOG_MINIMUM 0.0f

bool AnalogInput::available() {
    return Storage::getInstance().getAddonOptions().analogOptions.enabled;
}

void AnalogInput::setup() {
    const AnalogOptions& analogOptions = Storage::getInstance().getAddonOptions().analogOptions;
    
    // Setup our ADC Pair of Sticks
    adc_pairs[0].x_pin = analogOptions.analogAdc1PinX;
    adc_pairs[0].y_pin = analogOptions.analogAdc1PinY;
    adc_pairs[0].analog_invert = analogOptions.analogAdc1Invert;
    adc_pairs[0].analog_dpad = analogOptions.analogAdc1Mode;
    adc_pairs[0].ema_option = analogOptions.analog_smoothing;
    adc_pairs[0].ema_smoothing = analogOptions.smoothing_factor / 1000.0f;
    adc_pairs[0].error_rate = analogOptions.analog_error / 1000.0f;
    adc_pairs[0].in_deadzone = analogOptions.inner_deadzone / 100.0f;
    adc_pairs[0].out_deadzone = analogOptions.outer_deadzone / 100.0f;
    adc_pairs[0].auto_calibration = analogOptions.auto_calibrate;
    adc_pairs[0].forced_circularity = analogOptions.forced_circularity;
    adc_pairs[1].x_pin = analogOptions.analogAdc2PinX;
    adc_pairs[1].y_pin = analogOptions.analogAdc2PinY;
    adc_pairs[1].analog_invert = analogOptions.analogAdc2Invert;
    adc_pairs[1].analog_dpad = analogOptions.analogAdc2Mode;
    adc_pairs[1].ema_option = analogOptions.analog_smoothing2;
    adc_pairs[1].ema_smoothing = analogOptions.smoothing_factor2 / 1000.0f;
    adc_pairs[1].error_rate = analogOptions.analog_error2 / 1000.0f;
    adc_pairs[1].in_deadzone = analogOptions.inner_deadzone2 / 100.0f;
    adc_pairs[1].out_deadzone = analogOptions.outer_deadzone2 / 100.0f;
    adc_pairs[1].auto_calibration = analogOptions.auto_calibrate2;
    adc_pairs[1].forced_circularity = analogOptions.forced_circularity2;
    

    // Setup defaults and helpers
    for (int i = 0; i < ADC_COUNT; i++) {
        adc_pairs[i].x_pin_adc = adc_pairs[i].x_pin - ADC_PIN_OFFSET;
        adc_pairs[i].y_pin_adc = adc_pairs[i].y_pin - ADC_PIN_OFFSET;
        adc_pairs[i].x_value = ANALOG_CENTER;
        adc_pairs[i].y_value = ANALOG_CENTER;
        adc_pairs[i].xy_magnitude = 0.0f;
        adc_pairs[i].x_magnitude = 0.0f;
        adc_pairs[i].y_magnitude = 0.0f;
        adc_pairs[i].x_ema = 0.0f;
        adc_pairs[i].y_ema = 0.0f;
    }

    // Intialize and auto center X/Y for each pair
    for (int i = 0; i < ADC_COUNT; i++) {
        if(isValidPin(adc_pairs[i].x_pin)) {
            adc_gpio_init(adc_pairs[i].x_pin);
            if (adc_pairs[i].auto_calibration) {
                adc_select_input(adc_pairs[i].x_pin - ADC_PIN_OFFSET);
                adc_pairs[i].x_center = adc_read();
            }
        }
        if(isValidPin(adc_pairs[i].y_pin)) {
            adc_gpio_init(adc_pairs[i].y_pin);
            if (adc_pairs[i].auto_calibration) {
                adc_select_input(adc_pairs[i].y_pin - ADC_PIN_OFFSET);
                adc_pairs[i].y_center = adc_read();
            }
        }
    }
}

void AnalogInput::process() {
    Gamepad * gamepad = Storage::getInstance().GetGamepad();
    
    uint16_t joystickMid = GAMEPAD_JOYSTICK_MID;
    if ( DriverManager::getInstance().getDriver() != nullptr ) {
        joystickMid = DriverManager::getInstance().getDriver()->GetJoystickMidValue();
    }

    for(int i = 0; i < ADC_COUNT; i++) {
        // Read X-Axis
        if (isValidPin(adc_pairs[i].x_pin)) {
            adc_pairs[i].x_value = readPin(i, adc_pairs[i].x_pin_adc, adc_pairs[i].x_center);
            if (adc_pairs[i].analog_invert == InvertMode::INVERT_X || 
                adc_pairs[i].analog_invert == InvertMode::INVERT_XY) {
                adc_pairs[i].x_value = ANALOG_MAX - adc_pairs[i].x_value;
            }
            if (adc_pairs[i].ema_option) {
                adc_pairs[i].x_value = emaCalculation(i, adc_pairs[i].x_value, adc_pairs[i].x_ema);
                adc_pairs[i].x_ema = adc_pairs[i].x_value;
            }
        }
        // Read Y-Axis
        if (isValidPin(adc_pairs[i].y_pin)) {
            adc_pairs[i].y_value = readPin(i, adc_pairs[i].y_pin_adc, adc_pairs[i].y_center);
            if (adc_pairs[i].analog_invert == InvertMode::INVERT_Y || 
                adc_pairs[i].analog_invert == InvertMode::INVERT_XY) {
                adc_pairs[i].y_value = ANALOG_MAX - adc_pairs[i].y_value;
            }
            if (adc_pairs[i].ema_option) {
                adc_pairs[i].y_value = emaCalculation(i, adc_pairs[i].y_value, adc_pairs[i].y_ema);
                adc_pairs[i].y_ema = adc_pairs[i].y_value;
            }
        }
        // Look for dead-zones and circularity
        adc_pairs[i].xy_magnitude = magnitudeCalculation(i, adc_pairs[i]);
        if (adc_pairs[i].xy_magnitude < adc_pairs[i].in_deadzone) {
            adc_pairs[i].x_value = ANALOG_CENTER;
            adc_pairs[i].y_value = ANALOG_CENTER;
        } else {
            radialDeadzone(i, adc_pairs[i]);
        }

        if (adc_pairs[i].analog_dpad == DpadMode::DPAD_MODE_LEFT_ANALOG) {
            if ( joystickMid == 0x8000 ) {
                gamepad->state.lx = static_cast<uint16_t>(std::ceil(65535.0f * adc_pairs[i].x_value));
                gamepad->state.ly = static_cast<uint16_t>(std::ceil(65535.0f * adc_pairs[i].y_value));
            } else { // 0x7FFF
                gamepad->state.lx = static_cast<uint16_t>(65535.0f * adc_pairs[i].x_value);
                gamepad->state.ly = static_cast<uint16_t>(65535.0f * adc_pairs[i].y_value);
            }
        } else if (adc_pairs[i].analog_dpad == DpadMode::DPAD_MODE_RIGHT_ANALOG) {
            if ( joystickMid == 0x8000 ) {
                gamepad->state.rx = static_cast<uint16_t>(std::ceil(65535.0f * adc_pairs[i].x_value));
                gamepad->state.ry = static_cast<uint16_t>(std::ceil(65535.0f * adc_pairs[i].y_value));
            } else { // 0x7FFF
                gamepad->state.rx = static_cast<uint16_t>(65535.0f * adc_pairs[i].x_value);
                gamepad->state.ry = static_cast<uint16_t>(65535.0f * adc_pairs[i].y_value);
            }
        }
    }
}

float AnalogInput::readPin(int stick_num, Pin_t pin_adc, uint16_t center) {
    adc_select_input(pin_adc);
    uint16_t adc_value = adc_read();
    if (adc_pairs[stick_num].auto_calibration) {
        if (adc_value > center) {
            adc_value = map(adc_value, center, ADC_MAX, ADC_MAX / 2, ADC_MAX);
        } else if (adc_value == center) {
            adc_value = ADC_MAX / 2;
        } else {
            adc_value = map(adc_value, 0, center, 0, ADC_MAX / 2);
        }
    }
    return ((float)adc_value) / ADC_MAX;
}

float AnalogInput::emaCalculation(int stick_num, float ema_value, float ema_previous) {
    return (adc_pairs[stick_num].ema_smoothing * ema_value) + ((1.0f - adc_pairs[stick_num].ema_smoothing) * ema_previous);
}

uint16_t AnalogInput::map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float AnalogInput::magnitudeCalculation(int stick_num, adc_instance & adc_inst) {
    adc_inst.x_magnitude = adc_inst.x_value - ANALOG_CENTER;
    adc_inst.y_magnitude = adc_inst.y_value - ANALOG_CENTER;
    return adc_pairs[stick_num].error_rate * std::sqrt((adc_inst.x_magnitude * adc_inst.x_magnitude) + (adc_inst.y_magnitude * adc_inst.y_magnitude));
}

void AnalogInput::radialDeadzone(int stick_num, adc_instance & adc_inst) {
    float scaling_factor = (adc_inst.xy_magnitude - adc_pairs[stick_num].in_deadzone) / (adc_pairs[stick_num].out_deadzone - adc_pairs[stick_num].in_deadzone);
    if (adc_pairs[stick_num].forced_circularity == true) {
        scaling_factor = std::fmin(scaling_factor, ANALOG_CENTER);
    }
    adc_inst.x_value = ((adc_inst.x_magnitude / adc_inst.xy_magnitude) * scaling_factor) + ANALOG_CENTER;
    adc_inst.y_value = ((adc_inst.y_magnitude / adc_inst.xy_magnitude) * scaling_factor) + ANALOG_CENTER;
    adc_inst.x_value = std::clamp(adc_inst.x_value, ANALOG_MINIMUM, ANALOG_MAX);
    adc_inst.y_value = std::clamp(adc_inst.y_value, ANALOG_MINIMUM, ANALOG_MAX);
}