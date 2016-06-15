#include "EraseDBMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

EraseDBMenu::EraseDBMenu() : AbstractState()
{
}

void EraseDBMenu::Init()
{
  AbstractState::Init();
  ClearScreen();
  lcd.setCursor(0,0);
  lcd.print("Select to erase");
}

AbstractState* EraseDBMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

AbstractState* EraseDBMenu::Select()
{
  resetTime();
  if (MessageDatabase::DoesDBExist())
  {
    if (MessageDatabase::EraseDB())
    {
      LCDStates::TheSplashMenu.Init("Erased", false);
    } else {
      LCDStates::TheSplashMenu.Init("Erase failed", false);
    }
  } else {
    LCDStates::TheSplashMenu.Init("DB not found", false);
  }
  
  return (AbstractState*)&LCDStates::TheSplashMenu;
}

AbstractState* EraseDBMenu::Left()
{
  return (AbstractState*)&LCDStates::TheSettingsMenu;
}
