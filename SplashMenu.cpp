#include "SplashMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

//bool SplashMenu::returnToNodeMenu;
//String SplashMenu::text;

SplashMenu::SplashMenu() : AbstractState()
{
}

void SplashMenu::Init(String textToDisplay, bool shouldReturnToNodeMenu)
{
  text = textToDisplay;
  returnToNodeMenu = shouldReturnToNodeMenu;
}

void SplashMenu::Init()
{
  millisAtInit = millis();
  ClearScreen();
  lcd.setCursor(3,0);
  lcd.print(text);
}

AbstractState* SplashMenu::Tick()
{
    if (millis()-millisAtInit > 1500)
    {
      if (returnToNodeMenu) 
      {
        return (AbstractState*)&LCDStates::TheInboundRadioNodeMenu;
      } 
      else
      {
        return (AbstractState*)&LCDStates::TheSettingsMenu;
      }
    }
    return NULL;  
}

