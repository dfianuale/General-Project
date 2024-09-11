#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Basically a container for robot variables and
                other logic.
*//////////////////////////////////////////////////////////////
// Include Libraries
#include <Arduino.h>
#include "robot.h"

// Methods -------------------------------------------
// Instantiates class
robot::robot(void) {}
robot::~robot(void) {}

// Logic methods
void robot::runLogic(void)
{
    resetOutputsToNull();
    // This is the main logic function
    switch(behavior)
    {
        case BEHAVIOR_IDLE:
            // Do Nothing
            break;

        case BEHAVIOR_HEARTBEAT:
            behaviorReport();
            break;

        case BEHAVIOR_EXAMPLE:
            behaviorExample();
            break;

        default:
            // Do Nothing
            break;
    }
}

///////////////////////////////////////////////////////////////
// Behavior methods ------------------------------------
///////////////////////////////////////////////////////////////
void robot::resetOutputsToNull(void)
{
    drive_forward = 0;
    drive_spin = 0;

    rgb_r = 0;
    rgb_g = 0;
    rgb_b = 0;
    color_sensor_white_led = 0;
}

void robot::behaviorReport(void)
{
    // Print all sensor information then delay for some duration
    drawHoriz(20, '#');
    Serial.print(F("Time, dt:           "));
    Serial.print(this_time);
    Serial.print(F(", "));
    Serial.print(dt);
    Serial.println();

    Serial.print(F("Accelerometer xyz:  "));
    Serial.print(ax);
    Serial.print(F(", "));
    Serial.print(ay);
    Serial.print(F(", "));
    Serial.print(az);
    Serial.println();

    Serial.print(F("Gyroscope xyz:      "));
    Serial.print(gx);
    Serial.print(F(", "));
    Serial.print(gy);
    Serial.print(F(", "));
    Serial.print(gz);
    Serial.println();

    Serial.print(F("Magnetometer xyz:   "));
    Serial.print(mx);
    Serial.print(F(", "));
    Serial.print(my);
    Serial.print(F(", "));
    Serial.print(mz);
    Serial.println();

    Serial.print(F("Temperature:        "));
    Serial.print(temperature);
    Serial.println();

    Serial.print(F("Battery V, pct:     "));
    Serial.print(battery_voltage);
    Serial.print(F(", "));
    Serial.print(battery_percent);
    Serial.println();

    Serial.print(F("Color rgb sense:    "));
    Serial.print(sense_red);
    Serial.print(F(", "));
    Serial.print(sense_green);
    Serial.print(F(", "));
    Serial.print(sense_blue);
    Serial.println();
}

void robot::behaviorExample(void)
{
    switch(action)
    {
        case ACTION_INIT:
            break;
        case ACTION_FORWARD:
            break;
        case ACTION_SPIN:
            break;
        default:
            // nothing
            break;
    }
}
///////////////////////////////////////////////////////////////
// Input methods ---------------------------------------
///////////////////////////////////////////////////////////////
void robot::inputIMU(float _ax, float _ay, float _az, float _gx, float _gy, float _gz)
{
    ax = _ax;
    ay = _ay;
    az = _az;
    // Todo: find unit vector

    gx = _gx;
    gy = _gy;
    gz = _gz;
    // Todo: make the calibration happen. probably not needed
}

void robot::inputMag(float _mx, float _my, float _mz)
{
    mx = _mx;
    my = _my;
    mz = _mz;
    // Todo: Need to account for how these magnetometer readings actually point to north
}

void robot::inputTemperature(float _temperature)
{
    temperature = _temperature;
}

void robot::inputBattery(float _voltage, float _percent)
{
    battery_voltage = _voltage;
    battery_percent = _percent;
}

void robot::inputColor(float _red, float _green, float _blue)
{
    sense_red = _red;
    sense_green = _green;
    sense_blue = _blue;
}

void robot::setInputTime(uint32_t _time)
{
    last_time = this_time;
    this_time = _time;
    dt = (this_time - last_time) / 1000;
}
