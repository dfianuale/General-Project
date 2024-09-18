// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>


Adafruit_MPU6050 mpu;
double GRAVITY_CONSTANT = 9.81;
double VELOCITY_SUM = 0;
double POSITION_SUM = 0;
double ABS_DEADBAND = 10;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.println("Successfully configured MPU6050");
  delay(100);
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("AccelX:");
  Serial.print(a.acceleration.x);
  Serial.print(",");

  Serial.print("AccelY:");
  Serial.print(a.acceleration.y);
  Serial.print(",");


  //X axis is normal to driving axis
  //Y axis is perpendicular
  
  //0 is pointed straight down
  // double angle = acos(a.acceleration.x/GRAVITY_CONSTANT);

  // Serial.print("Angle:");
  // Serial.print(angle * (M_PI/180));
  // Serial.print(",");



  // double accelDueToGravY = sin(angle)*GRAVITY_CONSTANT;

  // double trueYaccel = a.acceleration.y-accelDueToGravY;



  // Serial.print("TrueAcel:");
  // Serial.print(trueYaccel);
  // Serial.print(",");

  //TODO: Make this integrate with respect to time instead of with respect to arduino loops
  if(abs(a.acceleration.y)>ABS_DEADBAND){
  VELOCITY_SUM+=a.acceleration.y;
  }
  Serial.print(" Velocity: ");
  Serial.print(VELOCITY_SUM);
  Serial.print(",");
  
  POSITION_SUM+=VELOCITY_SUM;
  Serial.print(" Position: ");
  Serial.print(POSITION_SUM);

  Serial.println();

  


  delay(10);
}