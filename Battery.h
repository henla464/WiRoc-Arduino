#ifndef __Battery_H__
#define __Battery_H__
#include <Arduino.h>

class Battery
{
  public:
    static uint16_t FullBatteryValue;
    static void Init(uint8_t fullBatteryValue);
    static uint8_t GetBatteryPercentage();
    static float GetBatteryVoltage();
    static void UpdateBattery();
  private:
    static uint16_t ReferenceFullBatteryValue;
    static uint8_t DifferenceFromFullBatteryValue[];
    static float ScalingFactor;
    static uint16_t CurrentBatteryValue;
};

#endif // __Battery_H__
