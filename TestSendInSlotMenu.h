#ifndef __TestSendInSlotMenu_H__
#define __TestSendInSlotMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"
#include "MessageDatabase.h"
#include "SIStation.h"

class TestSendInSlotMenu : public AbstractState 
{
  public:
    TestSendInSlotMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Select() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Left() override;
  private:
    uint8_t interval;
    uint32_t cardNumber;
    uint32_t lastTimeSent;
    bool isSending;
    void printMenu();
};

#endif // __TestSendInSlotMenu_H__
