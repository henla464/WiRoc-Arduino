#ifndef __TestMenu_H__
#define __TestMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>

class TestMenu : public AbstractState 
{
  public:
    TestMenu();
    virtual void Init() override;
    virtual AbstractState* Select() override;
    virtual AbstractState* Tick() override;
    virtual AbstractState* Up() override;
    virtual AbstractState* Down() override;
    virtual AbstractState* Left() override;
    virtual AbstractState* Right() override;
  private:
    void printMenu();
    void printArrow();
    AbstractState* GetSelected();
    uint8_t cursorIndex;
    uint8_t firstRowIndex;
    static char TestMenuItems[][15];
};

#endif // __TestMenu_H__
