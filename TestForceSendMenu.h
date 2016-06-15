#ifndef __TestForceSendMenu_H__
#define __TestForceSendMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"

class TestForceSendMenu : public AbstractState 
{
  public:
    TestForceSendMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Select() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Left() override;
  private:
    uint8_t interval;
    uint32_t lastTimeSent;
    uint8_t cardNumber;
    bool isSending;
    void printMenu();
};

#endif // __TestForceSendMenu_H__
