#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Functions related to robot measurement
*//////////////////////////////////////////////////////////////
// Includes
#include "inputs.h"
/////////////////////////////////////////////////////////////////////////
// Functions
void inputs(robot* controller, Adafruit_MAX17048*  lipo_monitor, Adafruit_MPU6050*   imu, Adafruit_TCS34725*  color_sensor, QMC5883LCompass*    compass)
{
    inputsFromDevices(controller, lipo_monitor, imu, color_sensor, compass);
    inputsFromHardwarePins(controller);
}

//--------------------------------
void inputsFromDevices(robot* controller, Adafruit_MAX17048*  lipo_monitor, Adafruit_MPU6050*   imu, Adafruit_TCS34725*  color_sensor, QMC5883LCompass*    compass)
{
    controller->setInputTime(millis());

    // IMU
    sensors_event_t a, g, temp;
    imu->getEvent(&a, &g, &temp);

    controller->ax = a.acceleration.x;
    controller->ay = a.acceleration.y;
    controller->az = a.acceleration.z;
    controller->gx = g.gyro.x;
    controller->gy = g.gyro.y;
    controller->gz = g.gyro.z;
    controller->temperature = temp.temperature;

    // Magnetometer
    compass->read();
    controller->mx = compass->getX();
    controller->my = compass->getY();
    controller->mz = compass->getZ();

    // Battery Monitor
    controller->battery_voltage = lipo_monitor->cellVoltage();
    controller->battery_percent = lipo_monitor->cellPercent();

    // Color Sensor
    float red, green, blue;
    color_sensor->getRGB(&red, &green, &blue);

    controller->sense_red = red;
    controller->sense_blue = blue;
    controller->sense_green = green;
}
void inputsFromHardwarePins(robot* controller)
{
    controller->user_button_pushed = !digitalRead(DI_USER_BUTTON);
}

/////////////////////////////////////////////////////////////////////////
