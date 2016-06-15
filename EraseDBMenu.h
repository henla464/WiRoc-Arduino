#ifndef __EraseDBMenu_H__
#define __EraseDBMenu_H__
#include <Arduino.h>
#include "AbstractState.h"
#include <LiquidCrystal.h>

class EraseDBMenu : public AbstractState 
{
  public:
    EraseDBMenu();
    virtual void Init() override;
    virtual AbstractState* Tick();
    virtual AbstractState* Select() override;
    virtual AbstractState* Left() override;
  private:
};

#endif // __EraseDBMenu_H__
