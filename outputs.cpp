#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Functions related to robot measurement
*//////////////////////////////////////////////////////////////
// Includes
#include "outputs.h"
/////////////////////////////////////////////////////////////////////////
void outputs(robot* controller, Adafruit_NeoPixel*  rgb_led)
{
    safetyControl(controller);
}

/////////////////////////////////////////////////////////////////////////
void setLEDs(robot* controller, Adafruit_NeoPixel*  rgb_led)
{
    rgb_led->begin();
    rgb_led->setPixelColor(controller->rgb_r, controller->rgb_g, controller->rgb_b, 0);
    rgb_led->show();

    digitalWrite(DO_COLOR_SENSOR_LED, controller->color_sensor_white_led);
}

void runMotors(float forward, float spin)
{
    // Assume: forward and spin are -1...1 
    // Assume: PWM is using 8-bit resolution
    float a_speed = max(min(255 * forward + 255 * spin, (float)255), (float)-255);
    float b_speed = max(min(255 * forward - 255 * spin, (float)255), (float)-255);

    if((a_speed == 0)&&(b_speed == 0))
    {
        digitalWrite(DO_HB_SLP, LOW);
    }
    else
    {
        digitalWrite(DO_HB_SLP, HIGH);

        if(a_speed > 0)
        {
            ledcWrite(PWM_CH_A1, a_speed);
            ledcWrite(PWM_CH_A2, 0);
        }
        else
        {
            ledcWrite(PWM_CH_A1, 0);
            ledcWrite(PWM_CH_A2, a_speed);
        }

        if(b_speed > 0)
        {
            ledcWrite(PWM_CH_B1, a_speed);
            ledcWrite(PWM_CH_B2, 0);
        }
        else
        {
            ledcWrite(PWM_CH_B1, 0);
            ledcWrite(PWM_CH_B2, a_speed);
        }
    }    
}

void safetyControl(robot* controller)
{
    // TODO: Add some sort of logic to prevent rapid changes in motor directions
}
