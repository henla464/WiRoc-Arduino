#include "Battery.h"
#include <Arduino.h>

uint16_t Battery::FullBatteryValue = 9040;
uint16_t Battery::ReferenceFullBatteryValue = 9040;
float Battery::ScalingFactor = 1;
uint16_t Battery::CurrentBatteryValue = Battery::FullBatteryValue;
uint8_t Battery::DifferenceFromFullBatteryValue[] = {16, 24, 33, 41, 49, 59, 63, 69, 74, 81, 84, 86, 87, 88, 90, 97, 106, 124, 140, 154};

void Battery::Init(uint8_t fullBatteryValue)
{
  FullBatteryValue = fullBatteryValue;
  ScalingFactor = FullBatteryValue / ReferenceFullBatteryValue;
}

uint8_t Battery::GetBatteryPercentage()
{
  for (int8_t i = 19; i >= 0; i--)
  {
    if (CurrentBatteryValue < FullBatteryValue - ScalingFactor*10*DifferenceFromFullBatteryValue[i])
    {
      return 100-5*(i+1);
    }
  }
  return 100;
}

float Battery::GetBatteryVoltage()
{
  return (CurrentBatteryValue*5.0/ 10240);
}


void Battery::UpdateBattery()
{
  int sensorValue = analogRead(A15) * 10;
  CurrentBatteryValue = 0.995*CurrentBatteryValue + 0.005*sensorValue;
  //Serial2.print(sensorValue); Serial2.print(" "); Serial2.println(CurrentBatteryValue);
  //delay(500);
}

