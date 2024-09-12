#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler and Parth Kabaria
// Description: Basically a container for robot variables and
                other logic.
*//////////////////////////////////////////////////////////////
// Include Libraries
#include <Arduino.h>
#include "robot.h"
#include <vector>

#include <Wire.h>
#include <math.h>
#include "Adafruit_TCS34725.h"
//#include "baseline_functionality_demonstration.ino"

//F_light_report code begins here

#ifndef DEF_INCLUDES_H
#define DEF_INCLUDES_H

#include "def_includes.h"

//Serial.println("In robot.cpp");

// Global Variables
Adafruit_TCS34725  color_sensor_robot = (TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

uint16_t max_brightness = 0;
uint16_t min_brightness = 65000;

// Calc Hues
float   calcHue(float, float, float);
uint8_t determineColor(float, uint16_t, uint16_t, uint16_t);

uint16_t ii = 0;

//F_light_report code ends here

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
        
        case BEHAVIOR_GPS:
            behaviorGPS();
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
  //startSenseColor()
    switch(action)
    {
        case ACTION_EXAMPLE_INIT:
            // No initialization actions, transition to action_forward
            action = ACTION_EXAMPLE_FORWARD;
            break;
        // ------------------------------------------------
        case ACTION_EXAMPLE_FORWARD:
            // Drive directly forward
            drive_forward = 1;
            drive_spin = 0;
            // Condition: if the level of red light falls below 200 (of 255), transition to spin action
            if(rgb_r > 200)
            {
                action = ACTION_EXAMPLE_SPIN;
            }
            break;
        // ------------------------------------------------
        case ACTION_EXAMPLE_SPIN:
            // Spin in place
            drive_forward = 0;
            drive_spin = 1;
            // Condition: if level of red light goes above threshold, go forward again
            if(rgb_b > 200)
            {
                action = ACTION_EXAMPLE_FORWARD;
            }
            break;
        // ------------------------------------------------
        default:
            // nothing
            break;
    }
}

void robot::behaviorGPS(void)
{

  //Serial.println("In behaviorGPS function");

  // Determine Color
  uint16_t r;
  uint16_t g;
  uint16_t b;
  uint16_t c;
  color_sensor_robot.getRawData(&r, &g, &b, &c);

  // Automatic calibration
  min_brightness = min(min_brightness, c);
  max_brightness = max(max_brightness, c);

  // Calculate the hue
  //Serial.print(r); Serial.print(" ");
  //Serial.print(g);Serial.print(" ");
  //Serial.print(b);Serial.print(" ");
  float hue = calcHue((float)r, (float)g, (float)b);

  // Turn it into discrete colors
  uint8_t color_index = determineColor(hue, c, max_brightness - THRESH_WHITE_OFFSET, min_brightness + THRESH_BLACK_OFFSET);
  //uint8_t color_index = determineColor(hue, c, THRESHOLD_WHITE, THRESHOLD_BLACK);
  //Serial.print(color_index); Serial.print(" ");

  switch(color_index){

    //case COLOR_YELLOW:
    case COLOR_RED:
      Serial.println("Sensed red");
      gps_time_start = millis();
      break;

    case COLOR_GREEN:
      Serial.println("Sensed green");
      gps_time_end = millis();
      pos_y = (gps_time_end - gps_time_start) / (totalVerticalTime);
      Serial.println("Y position:");
      Serial.print(pos_y);
      break;

    case COLOR_MAGENTA:
    case COLOR_CYAN:
    case COLOR_BLUE:
      Serial.println("Sensed blue");
      gps_time_end = millis();
      // Make this a configurable quantity
      pos_x = (gps_time_end - gps_time_start - totalHorizontalTime) / (totalHorizontalTime);
      Serial.println("X position:");
      Serial.println(pos_x);
      break;

    case COLOR_WHITE:   
      Serial.println("Color is WHITE"   ); 
      break;
    case COLOR_BLACK:   
      Serial.println("Color is BLACK"   ); 
      break;

    default:
      // nothing
      Serial.println("No recognized color sensed"   );                                                                                                                                                                                                                                          
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

    // Here is where to put the color interpretation code
}

void robot::setInputTime(uint32_t _time)
{
    last_time = this_time;
    this_time = _time;
    dt = (this_time - last_time) / 1000;
}

//F_light_report code starts here
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void robot::waitForButtonPress() 
{
    Serial.println("Press the button to continue...");
    while (digitalRead(DI_USER_BUTTON) == HIGH) 
    {
        // Wait for button press
    }
    while (digitalRead(DI_USER_BUTTON) == LOW) 
    {
        // Wait for button release
    }
    delay(500); // Debounce delay
}

/////////////////////////////////////////////////////////////////////////////////////
TCS34725_RGB robot::readAverageRGBValues(Adafruit_TCS34725& sensor, int numReadings) 
{
    TCS34725_RGB sumRGB = {0.0f, 0.0f, 0.0f};
    
    for (int i = 0; i < numReadings; ++i) 
    {
        uint16_t r, g, b, c;
        //color_sensor_light.getRawData(&r, &g, &b, &c);
        
        /*Serial.print("# "); // Having a unique character for new line is very useful
        Serial.print(r);
        Serial.print(", ");
        Serial.print(g);
        Serial.print(", ");
        Serial.print(b);
        Serial.print(", ");
        Serial.print(c);
        Serial.println(";");*/
        
        sumRGB.R += r;
        sumRGB.G += g;
        sumRGB.B += b;
        
    }

    TCS34725_RGB averageRGB = 
    {
        sumRGB.R / numReadings,
        sumRGB.G / numReadings,
        sumRGB.B / numReadings
        
    };
    /*
      Serial.print("Average RGB: ");
      Serial.print(averageRGB.R);
      Serial.print(", ");
      Serial.print(averageRGB.G);
      Serial.print(", ");
      Serial.println(averageRGB.B);
    */

    return averageRGB;
}

TCS34725_RGB robot::normalizeRGB(const TCS34725_RGB& rgb, const TCS34725_RGB& referenceBlack, const TCS34725_RGB& referenceWhite) 
{
    TCS34725_RGB normalizedRGB = 
    {
        (rgb.R - referenceBlack.R) / (referenceWhite.R - referenceBlack.R),
        (rgb.G - referenceBlack.G) / (referenceWhite.G - referenceBlack.G),
        (rgb.B - referenceBlack.B) / (referenceWhite.B - referenceBlack.B)
    };

    return normalizedRGB;
}

float robot::normalizedRGBto_L(const TCS34725_RGB& normalizedRGB)
{
    float R = normalizedRGB.R;
    float G = normalizedRGB.G;
    float B = normalizedRGB.B;
    
    float Cmax = std::max({R, G, B});
    float Cmin = std::min({R, G, B});
    float L = (Cmax + Cmin) / 2; // Calculate lightness here
   
    return L;
}

float robot::normalizedRGBtoHue(const TCS34725_RGB& normalizedRGB) 
{
    float R = normalizedRGB.R;
    float G = normalizedRGB.G;
    float B = normalizedRGB.B;

    float Cmax = std::max({R, G, B});
    float Cmin = std::min({R, G, B});
 
    float delta = Cmax - Cmin;
    float hue = 0.0f;
    
    if (delta != 0.0f) 
    {
    
        if (Cmax == R) 
        {
            hue = std::fmod((G - B) / delta, 6.0f);
        } 
        else if (Cmax == G) 
        {
            hue = (B - R) / delta + 2.0f;
        } 
        else 
        {
            hue = (R - G) / delta + 4.0f;
        }
        hue *= 60.0f;
        if (hue < 0.0f) 
        {
            hue += 360.0f;
        }
            hue /= 360.0f;
     }
    return hue;
  
}


float robot::cal_diff_W(float L)

{
    float diff_W = 1 - L;
    
    return diff_W;
};

float calcHue(float red, float green, float blue)
{
  float hue;
  // Intemediate Values
  float c_max = max(max(red, green), blue);
  float c_min = min(min(red, green), blue);
  float delta = c_max - c_min;

  // Calculate things
  if(delta != 0)
  {
    if(red == c_max)
    {
      hue = (green - blue) / delta;
    }
    if(green == c_max)
    {
      hue = 2.0 + (blue - red) / delta;
    }
    if(blue == c_max)
    {
      hue = 4.0 + (red - green) / delta;
    }

    hue = hue / 6.0;
    hue = fmodf(hue, 1.0);
  }
  else
  {
    hue = 0.0;
  }

  return hue;
}

uint8_t determineColor(float hue, uint16_t brightness, uint16_t thresh_white, uint16_t thresh_black)
{

  //Serial.println("In determineColor()");
  uint8_t color;

  // This function is just annoying...
  if(brightness > thresh_white)
  {
    color = COLOR_WHITE;
    //Serial.println("found white");
  }
  else
  {
    if(brightness < thresh_black)
    {
      color = COLOR_BLACK;
      //Serial.println("found black");
    }
    else
    {
      if(hue > RED_MIN_1)
      {
        color = COLOR_RED;
        //Serial.println("found red");
      }
      else
      {
        if(hue > MAGENTA_MIN)
        {
          color = COLOR_MAGENTA;
          //Serial.println("found magenta");
        }
        else
        {
          if(hue > BLUE_MIN)
          {
            color = COLOR_BLUE;
            //Serial.println("found blue");
          }
          else
          {
            if(hue > CYAN_MIN)
            {
              color = COLOR_CYAN;
              //Serial.println("found cyan");
            }
            else
            {
              if(hue > GREEN_MIN)
              {
                color = COLOR_GREEN;
                //Serial.println("found green");
              }
              else
              {
                if(hue > YELLOW_MIN)
                {
                  color = COLOR_YELLOW;
                  //Serial.println("found yellow");
                }
                else
                {
                  //if(hue > RED_MIN_0)
                  //{
                    color = COLOR_RED;
                    //Serial.println("found red");
                  //}
                }
              }
            }
          }
        }
      }
    }
  }

  return color;
}

#endif

//F_light_report code ends here
