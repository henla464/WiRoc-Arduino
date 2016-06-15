#ifndef __StationNumberMenu_H__
#define __StationNumberMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"
#include "Radio.h"

class StationNumberMenu : public AbstractState 
{
  public:
    StationNumberMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Select() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Left() override;
  private:
    uint8_t nodeNumber;
    void printNodeNumber();
};

#endif // __StationNumberMenu_H__
