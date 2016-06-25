#include "SplashMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

SplashMenu::SplashMenu() : AbstractState()
{
}

void SplashMenu::Init(String textToDisplay, AbstractState* returnToMenu)
{
  text = textToDisplay;
  rtnToMenu = returnToMenu;
}

void SplashMenu::Init()
{
  AbstractState::Init();
  ClearScreen();
  uint8_t startPosition = max(0, 7-text.length()/2);
  lcd.setCursor(startPosition,0);
  lcd.print(text);
}

AbstractState* SplashMenu::Tick()
{
    if (millis()-millisAtInit > 1500)
    {
      return rtnToMenu;
      /*
      if (returnToNodeMenu) 
      {
        return (AbstractState*)&LCDStates::TheInboundRadioNodeMenu;
      } 
      else
      {
        return (AbstractState*)&LCDStates::TheSettingsMenu;
      }*/
    }
    return NULL;  
}

