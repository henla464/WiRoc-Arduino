#ifndef __BatteryMenu_H__
#define __BatteryMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Battery.h"

class BatteryMenu : public AbstractState 
{
  public:
    BatteryMenu();
    virtual void Init() override;
    virtual AbstractState* Left() override;
    virtual AbstractState* Tick() override;
  private:
    uint32_t lastUpdateMillis;
    void printBatteryNumbers();
};

#endif // __BatteryMenu_H__
