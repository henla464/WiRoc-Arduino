#ifndef __InboundRadioNodeMenu_H__
#define __InboundRadioNodeMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>
#include "Settings.h"
#include "Radio.h"

class InboundRadioNodeMenu : public AbstractState 
{
  public:
    InboundRadioNodeMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Select() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Left() override;
    bool AddNode(uint8_t nodeNumber);
  private:
    void printMenu();
    void printArrow();
    uint8_t cursorIndex;
    uint8_t firstRowIndex;
    uint8_t numberOfNodes;
    uint8_t nodes[5];
    bool initialized;
};

#endif // __InboundRadioNodeMenu_H__
