#ifndef __AddNodeMenu_H__
#define __AddNodeMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"
#include "Radio.h"

class AddNodeMenu : public AbstractState 
{
  public:
    AddNodeMenu();
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

#endif // __AddNodeMenu_H__
