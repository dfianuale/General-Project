/*//////////////////////////////////////////////////////////////
// Author: Petras Swissler and Roberto Torres
// Description: Example code to start from for the breakout-
                based robot using the Adafruit ESP32-S3 Feather
*///////////////////////////////////////////////////////////////
// Include Libraries
#ifndef DEF_INCLUDES_H
#define DEF_INCLUDES_H

#include "def_includes.h"

// Define the TCS34725_RGB structure here
typedef struct 
{
    float R;
    float G;
    float B;
}
TCS34725_RGB;

// Global Variables


//Adafruit_MAX17048  lipo_monitor;
Adafruit_NeoPixel  rgb_led(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
//Adafruit_MPU6050   imu;
Adafruit_TCS34725  color_sensor(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
//QMC5883LCompass    compass;

// Declare your functions
TCS34725_RGB readAverageRGBValues(Adafruit_TCS34725& sensor, int numReadings);
TCS34725_RGB normalizeRGB(const TCS34725_RGB& rgb, const TCS34725_RGB& referenceBlack, const TCS34725_RGB& referenceWhite);
float normalizedRGBtoHue(const TCS34725_RGB& normalizedRGB);


robot controller;

uint16_t ii = 0;

////////////////////////////////////////////////////////////////////
////// Button pressing function 
void waitForButtonPress() 
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
    delay(100); // Debounce delay
}

/////////////////////////////////////////////////////////////////////////////////////
////// RGB values converted to HUE and Lightness

TCS34725_RGB readAverageRGBValues(Adafruit_TCS34725& sensor, int numReadings) 
{
    TCS34725_RGB sumRGB = {0.0f, 0.0f, 0.0f};
    
    for (int i = 0; i < numReadings; ++i) 
    {
        uint16_t r, g, b, c;
        color_sensor.getRawData(&r, &g, &b, &c);
        
        Serial.print("# "); // Having a unique character for new line is very useful
        Serial.print(r);
        Serial.print(", ");
        Serial.print(g);
        Serial.print(", ");
        Serial.print(b);
        Serial.print(", ");
        Serial.print(c);
        Serial.println(";");
        
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
      Serial.print("Average RGB: ");
      Serial.print(averageRGB.R);
      Serial.print(", ");
      Serial.print(averageRGB.G);
      Serial.print(", ");
      Serial.println(averageRGB.B);

    return averageRGB;
}

TCS34725_RGB normalizeRGB(const TCS34725_RGB& rgb, const TCS34725_RGB& referenceBlack, const TCS34725_RGB& referenceWhite) 
{
    TCS34725_RGB normalizedRGB = 
    {
        (rgb.R - referenceBlack.R) / (referenceWhite.R - referenceBlack.R),
        (rgb.G - referenceBlack.G) / (referenceWhite.G - referenceBlack.G),
        (rgb.B - referenceBlack.B) / (referenceWhite.B - referenceBlack.B)
    };

    return normalizedRGB;
}
/////////////////////////////////////////////////////////////////////////////////////
////// converted to Lightness

float normalizedRGBto_L(const TCS34725_RGB& normalizedRGB)
{
    float R = normalizedRGB.R;
    float G = normalizedRGB.G;
    float B = normalizedRGB.B;
    
    float Cmax = std::max({R, G, B});
    float Cmin = std::min({R, G, B});
    float L = (Cmax + Cmin) / 2; // Calculate lightness here
   
    return L;
}
/////////////////////////////////////////////////////////////////////////////////////
////// converted to HUE

float normalizedRGBtoHue(const TCS34725_RGB& normalizedRGB) 
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

/////////////////////////////////////////////////////////////////////////////////////
////// Comparison between the Lightness values of random_color and White
  
float cal_diff_W(float L)

{
    float diff_W = 1 - L;
    
    return diff_W;
};

/////////////////////////////////////////////////////////////////////////////////////
////// Comparison between HUE value of random_color and each model HUE value 

int findClosestHue(float hue, const std::vector<float>& targetHues) 
{
    float closestDifference = std::abs(hue - targetHues[0]);
    int closestIndex = 0;

    for (size_t i = 1; i < targetHues.size(); ++i) 
    {
        float difference = std::abs(hue - targetHues[i]);
        if (difference < closestDifference) 
        {
            closestDifference = difference;
            closestIndex = static_cast<int>(i);
        }
    }

    return closestIndex;
    
}

///////////////////////////////////////////////////////////////
// Setup
void setup() 
{
    //boardSetup(&rgb_led, &lipo_monitor, &imu, &color_sensor, &compass);
    //controller.behavior = BEHAVIOR_HEARTBEAT;
    Wire1.setPins(SDA, SCL);
    pinConfig();
    serialSetup();
    rgb_led.begin();
    rgb_led.setPixelColor(0, 0, 25, 0);
    rgb_led.show();
    Serial.begin(115200); // Initialize Serial communication
    Serial.println("SYSTEM ON");
}

///////////////////////////////////////////////////////////////
// Main Loop
void loop() 
{
    Adafruit_TCS34725 sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
 
    waitForButtonPress();
    Serial.println("Press the button to read black reference...");
    waitForButtonPress();
    TCS34725_RGB blackReference = readAverageRGBValues(sensor, 5); 

    waitForButtonPress();
    Serial.println("Press the button to read white reference...");
    waitForButtonPress();
    TCS34725_RGB whiteReference = readAverageRGBValues(sensor, 5);


    std::vector<float> targetHues = {0, 0.99, 0.27235, 0.63862, 0.52428};

    while (true) // Continuously loop until manually reset
    {
        waitForButtonPress();
        Serial.println("Press the button to read a random color...");
        waitForButtonPress();
        TCS34725_RGB randomColor = readAverageRGBValues(sensor, 5);

        TCS34725_RGB normalizedRandomColor = normalizeRGB(randomColor, blackReference, whiteReference);
        float hue = normalizedRGBtoHue(normalizedRandomColor);
        float L = normalizedRGBto_L(normalizedRandomColor);
        float diff_W = cal_diff_W(L);
    
        Serial.print("Hue for RGB (");
        Serial.print(normalizedRandomColor.R);
        Serial.print(", ");
        Serial.print(normalizedRandomColor.G);
        Serial.print(", ");
        Serial.print(normalizedRandomColor.B);
        Serial.print("): ");
        Serial.println(hue);
        
        Serial.print("Lightness for RGB: ");
        Serial.println(L);
 
        Serial.print("Difference between White and Random color: ");
        Serial.println(diff_W);
    
        if (L < 0.05)
        {
          Serial.println("It's black");
        }
        else if(diff_W < 0.05)
        {
          Serial.println("It's white");
        }
        else 
        {
          Serial.println("Closest Target Hue: ");

          int closestIndex = findClosestHue(hue, targetHues);
          Serial.print(targetHues[closestIndex]);
          Serial.print(" at Index ");
          Serial.println(closestIndex);
          Serial.println();
        }
    }
}
#endif 
