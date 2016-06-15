#ifndef __TestReceiveMenu_H__
#define __TestReceiveMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"

class TestReceiveMenu : public AbstractState 
{
  public:
    TestReceiveMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Left() override;
  private:
    void printMenu();
};

#endif // __TestReceiveMenu_H__
