#ifndef __HiddenMenu_H__
#define __HiddenMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"

class HiddenMenu : public AbstractState 
{
  public:
    HiddenMenu();
    virtual void Init() override;
    virtual AbstractState* Left() override;
    virtual AbstractState* Tick() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Select() override;
  private:
    uint32_t lastUpdateMillis;
    uint8_t hwVersion;
    void Print();
};

#endif // __HiddenMenu_H__
