#ifndef __InboundRadioModeMenu_H__
#define __InboundRadioModeMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"
#include "Radio.h"

class InboundRadioModeMenu : public AbstractState 
{
  public:
    InboundRadioModeMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Select() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Left() override;
    static char Modes[][17];
  private:
    uint8_t radioModeIndex;
    void printMenu();
    void initRadioModeIndex();
    
};

#endif // __InboundRadioModeMenu_H__
