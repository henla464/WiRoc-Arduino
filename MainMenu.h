#ifndef __MainMenu_H__
#define __MainMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"
#include "Battery.h"

class MainMenu : public AbstractState 
{
  public:
    MainMenu();
    virtual void Init() override;
    virtual AbstractState* Select() override;
    virtual AbstractState* Tick() override;
  private:
    uint32_t lastUpdateMillis;
    void printNumbers();
};

#endif // __MainMenu_H__
