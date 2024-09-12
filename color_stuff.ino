/*/////////////////////////////////////////////////////////////
// Authors: Parth Kabaria and Petras Swissler
// Description: Processing and filtering color light data from color sensor
*//////////////////////////////////////////////////////////////

#include <Wire.h>
#include <math.h>
#include "Adafruit_TCS34725.h"

// Define Pins
#define PIN_BUTTON  5
#define PIN_LED     13

// White and Black Thresholds
#define THRESHOLD_WHITE 3700
#define THRESHOLD_BLACK 800
#define THRESH_WHITE_OFFSET 500
#define THRESH_BLACK_OFFSET 200

// Hue Thresholds
#define RED_MIN_0   0
#define YELLOW_MIN  0.125
#define GREEN_MIN   0.25
#define CYAN_MIN    0.35
#define BLUE_MIN    0.53
#define MAGENTA_MIN 0.7
#define RED_MIN_1   0.85

// Color Labels
#define COLOR_WHITE     0
#define COLOR_BLACK     1
#define COLOR_RED       2
#define COLOR_YELLOW    3
#define COLOR_GREEN     4
#define COLOR_CYAN      5
#define COLOR_BLUE      6
#define COLOR_MAGENTA   7

// Global Variables
Adafruit_TCS34725 color_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
uint16_t max_brightness = 0;
uint16_t min_brightness = 65000;

// Calc Hues
float   calcHue(float, float, float);
uint8_t determineColor(float, uint16_t, uint16_t, uint16_t);

// Setup function. Nothing Interesting Here
void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  delay(3000);
  Serial.println("Press Button");
}

// Loop function. See how I'm detecting the color.
void loop() {
  // Capture to Excel (for getting everythign set)
  /*while(digitalRead(PIN_BUTTON))
  {
    // Do Nothing
  }
  delay(1000);
  uint16_t r;
  uint16_t g;
  uint16_t b;
  uint16_t c;
  while(digitalRead(PIN_BUTTON))
  {
    color_sensor.getRawData(&r, &g, &b, &c);
    Serial.print(r); Serial.print(", ");
    Serial.print(g); Serial.print(", ");
    Serial.print(b); Serial.print(", ");
    Serial.print(c); Serial.println();
    //delay(50);
  }
  Serial.println("------------------------------------------------------");
  delay(1000);*/

  ///////////////////////////////////////////////////
  // USE FOR ROBOT
  // Determine Color
  uint16_t r;
  uint16_t g;
  uint16_t b;
  uint16_t c;
  color_sensor.getRawData(&r, &g, &b, &c);

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

  ///////////////////////////////////////////////////
  // Print stuff (this isn't necessary for the robot)
  Serial.print(c); Serial.print(" "); Serial.print(max_brightness); Serial.print(" "); Serial.print(min_brightness); Serial.print(" ");
  switch(color_index)
  {
    case COLOR_WHITE:   
      Serial.println("Color is WHITE"   ); 
      break;
    case COLOR_BLACK:   
      Serial.println("Color is BLACK"   ); 
      break;
    case COLOR_RED:     
      Serial.println("Color is RED"     ); 
      break;
    case COLOR_YELLOW:  
      Serial.println("Color is YELLOW"  ); 
      break;
    case COLOR_GREEN:   
      Serial.println("Color is GREEN"   ); 
      break;
    case COLOR_CYAN:    
      Serial.println("Color is CYAN"    ); 
      break;
    case COLOR_BLUE:    
      Serial.println("Color is BLUE"    ); 
      break;
    case COLOR_MAGENTA: 
      Serial.println("Color is MAGENTA" ); 
      break;
  }
}


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
  uint8_t color;

  // This function is just annoying...
  if(brightness > thresh_white)
  {
    color = COLOR_WHITE;
  }
  else
  {
    if(brightness < thresh_black)
    {
      color = COLOR_BLACK;
    }
    else
    {
      if(hue > RED_MIN_1)
      {
        color = COLOR_RED;
      }
      else
      {
        if(hue > MAGENTA_MIN)
        {
          color = COLOR_MAGENTA;
        }
        else
        {
          if(hue > BLUE_MIN)
          {
            color = COLOR_BLUE;
          }
          else
          {
            if(hue > CYAN_MIN)
            {
              color = COLOR_CYAN;
            }
            else
            {
              if(hue > GREEN_MIN)
              {
                color = COLOR_GREEN;
              }
              else
              {
                if(hue > YELLOW_MIN)
                {
                  color = COLOR_YELLOW;
                }
                else
                {
                  //if(hue > RED_MIN_0)
                  //{
                    color = COLOR_RED;
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
